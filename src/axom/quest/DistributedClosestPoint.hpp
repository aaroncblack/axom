// Copyright (c) 2017-2022, Lawrence Livermore National Security, LLC and
// other Axom Project Developers. See the top-level LICENSE file for details.
//
// SPDX-License-Identifier: (BSD-3-Clause)

#ifndef QUEST_DISTRIBUTED_CLOSEST_POINT_H_
#define QUEST_DISTRIBUTED_CLOSEST_POINT_H_

#include "axom/config.hpp"
#include "axom/core.hpp"
#include "axom/slic.hpp"
#include "axom/slam.hpp"
#include "axom/sidre.hpp"
#include "axom/primal.hpp"
#include "axom/spin.hpp"

#include "axom/fmt.hpp"

#include "conduit_blueprint.hpp"
#include "conduit_blueprint_mpi.hpp"
#include "conduit_relay_mpi.hpp"
#include "conduit_relay_io.hpp"

#include <list>
#include <vector>
#include <set>
#include <cstdlib>
#include <cmath>

#ifndef AXOM_USE_MPI
  #error This file requires Axom to be configured with MPI
#endif
#include "mpi.h"

namespace axom
{
namespace quest
{
namespace internal
{
/** 
 * \brief Utility function to get a typed pointer to the beginning of an array 
 * stored by a conduit::Node
 */
template <typename T>
T* getPointer(conduit::Node& node)
{
  T* ptr = node.value();
  return ptr;
}

/** 
 * \brief Utility function to create an axom::ArrayView over the array 
 * of native types stored by a conduit::Node
 */
template <typename T>
axom::ArrayView<T> ArrayView_from_Node(conduit::Node& node, int sz)
{
  T* ptr = node.value();
  return axom::ArrayView<T>(ptr, sz);
}

/** 
 * \brief Template specialization of ArrayView_from_Node for Point<double,2>
 *
 * \warning Assumes the underlying data is an MCArray with stride 2 access
 */
template <>
axom::ArrayView<primal::Point<double, 2>> ArrayView_from_Node(conduit::Node& node,
                                                              int sz)
{
  using PointType = primal::Point<double, 2>;

  PointType* ptr = static_cast<PointType*>(node.data_ptr());
  return axom::ArrayView<PointType>(ptr, sz);
}

/**
 * \brief Sends a conduit node along with its schema using MPI_Isend
 * 
 * \param [in] node node to send
 * \param [in] dest ID of MPI rank to send to
 * \param [in] tag tag for MPI message
 * \param [in] comm MPI communicator to use
 * \note Adapted from conduit's relay::mpi::send_using_schema to use 
 * non-blocking \a MPI_Isend instead of blocking \a MPI_Send
 */
int isend_using_schema(const conduit::Node& node, int dest, int tag, MPI_Comm comm)
{
  conduit::Schema s_data_compact;

  // schema will only be valid if compact and contig
  if(node.is_compact() && node.is_contiguous())
  {
    s_data_compact = node.schema();
  }
  else
  {
    node.schema().compact_to(s_data_compact);
  }

  std::string snd_schema_json = s_data_compact.to_json();

  conduit::Schema s_msg;
  s_msg["schema_len"].set(conduit::DataType::int64());
  s_msg["schema"].set(conduit::DataType::char8_str(snd_schema_json.size() + 1));
  s_msg["data"].set(s_data_compact);

  // create a compact schema to use
  conduit::Schema s_msg_compact;
  s_msg.compact_to(s_msg_compact);

  conduit::Node n_msg(s_msg_compact);
  // these sets won't realloc since schemas are compatible
  n_msg["schema_len"].set((int64)snd_schema_json.length());
  n_msg["schema"].set(snd_schema_json);
  n_msg["data"].update(node);

  auto msg_data_size = n_msg.total_bytes_compact();

  MPI_Request mpiRequest;
  int mpi_error = MPI_Isend(const_cast<void*>(n_msg.data_ptr()),
                            static_cast<int>(msg_data_size),
                            MPI_BYTE,
                            dest,
                            tag,
                            comm,
                            &mpiRequest);

  MPI_Request_free(&mpiRequest);

  if(static_cast<int>(mpi_error) != MPI_SUCCESS)
  {
    char check_mpi_err_str_buff[MPI_MAX_ERROR_STRING];
    int check_mpi_err_str_len = 0;
    MPI_Error_string(mpi_error, check_mpi_err_str_buff, &check_mpi_err_str_len);

    SLIC_ERROR(
      fmt::format("MPI call failed: error code = {} error message = {}",
                  mpi_error,
                  check_mpi_err_str_buff));
  }

  return mpi_error;
}

}  // namespace internal

template <int NDIMS = 2, typename ExecSpace = axom::SEQ_EXEC>
class DistributedClosestPoint
{
public:
  // TODO: generalize to 3D
  static_assert(NDIMS == 2,
                "DistributedClosestPoint only currently supports 2D");

  static constexpr int DIM = NDIMS;
  using PointType = primal::Point<double, DIM>;
  using BoxType = primal::BoundingBox<double, DIM>;
  using PointArray = axom::Array<PointType>;
  using BoxArray = axom::Array<BoxType>;
  using BVHTreeType = spin::BVH<DIM, ExecSpace>;

private:
  struct MinCandidate
  {
    /// Squared distance to query point
    double minSqDist {numerics::floating_point_limits<double>::max()};
    /// Index within mesh of closest element
    int minElem {-1};
    /// MPI rank of closest element
    int minRank {-1};
  };

public:
  DistributedClosestPoint(
    int allocatorID = axom::execution_space<ExecSpace>::allocatorID())
    : m_allocatorID(allocatorID)
  {
    MPI_Comm_rank(MPI_COMM_WORLD, &m_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &m_nranks);
  }

public:  // Query properties
  void setVerbosity(bool isVerbose) { m_isVerbose = isVerbose; }

public:
  /** 
   * Utility function to set the array of points
   *
   * \param [in] meshGroup The root group of a mesh blueprint
   * \note This function currently supports mesh blueprints with the "point" topology
   */
  void setObjectMesh(const conduit::Node& mesh_node, const std::string& coordset)
  {
    // Perform some simple error checking
    SLIC_ASSERT(this->isValidBlueprint(mesh_node));

    // Extract the dimension and number of points from the coordinate values group
    auto valuesPath = fmt::format("coordsets/{}/values", coordset);
    SLIC_ASSERT(mesh_node.has_path(valuesPath));
    auto& values = mesh_node[valuesPath];

    const int dim = extractDimension(values);
    const int N = extractSize(values);
    SLIC_ASSERT(dim == NDIMS);

    // Extract pointers to the coordinate data
    // Note: The following assumes the coordinates are contiguous with stride NDIMS
    // TODO: Generalize to support other strides

    // Copy the data into the point array of primal points
    PointArray pts(N, N);
    const std::size_t nbytes = sizeof(double) * dim * N;
    axom::copy(pts.data(), values["x"].data_ptr(), nbytes);

    m_points = PointArray(pts, m_allocatorID);  // copy point array to ExecSpace
  }

  bool generateBVHTree()
  {
    const int npts = m_points.size();
    axom::Array<BoxType> boxesArray(npts, npts, m_allocatorID);
    auto boxesView = boxesArray.view();
    axom::for_all<ExecSpace>(
      npts,
      AXOM_LAMBDA(axom::IndexType i) { boxesView[i] = BoxType {m_points[i]}; });

    // Build bounding volume hierarchy
    m_bvh.setAllocatorID(m_allocatorID);
    int result = m_bvh.initialize(boxesView, npts);

    return (result == spin::BVH_BUILD_OK);
  }

  /**
   * \brief Computes the closest point within the objects for each query point
   * in the provided particle mesh, provided in the mesh blueprint rooted at \a meshGroup
   * 
   * \param meshGroup The root of a mesh blueprint for the query points
   * \param coordset The coordinate set for the query points
   * 
   * Uses the \a coordset coordinate set of the provided blueprint mesh
   * 
   * The particle mesh must contain the following fields:
   *   - cp_rank: Will hold the rank of the object point containing the closest point
   *   - cp_index: Will hold the index of the object point containing the closest point
   *   - closest_point: Will hold the position of the closest point
   * 
   * \note The current implementation assumes that the coordinates and closest_points and contiguous
   * with stride NDIMS. We intend to loosen this restriction in the future
   */
  void computeClosestPoints(conduit::Node& mesh_node,
                            const std::string& coordset) const
  {
    // Utility function to dump a conduit node on each rank, e.g. for debugging
    auto dumpNode = [=](const conduit::Node& n,
                        const std::string&& fname,
                        const std::string& protocol = "json") {
      conduit::relay::io::save(n, fname, protocol);
    };

    // Perform some simple error checking
    SLIC_ASSERT(this->isValidBlueprint(mesh_node));

    // create conduit node containing data that has to xfer between ranks
    conduit::Node xfer_node;
    {
      // clang-format off
      auto& coords = mesh_node[fmt::format("coordsets/{}/values", coordset)];
      const int dim = extractDimension(coords);
      const int npts = extractSize(coords);

      xfer_node["npts"] = npts;
      xfer_node["dim"] = dim;
      xfer_node["src_rank"] = m_rank;
      xfer_node["coords"].set_external(internal::getPointer<double>(coords["x"]), dim * npts);
      xfer_node["cp_index"].set_external(internal::getPointer<axom::IndexType>(mesh_node["fields/cp_index/values"]), npts);
      xfer_node["cp_rank"].set_external(internal::getPointer<axom::IndexType>(mesh_node["fields/cp_rank/values"]), npts);
      xfer_node["closest_point"].set_external(internal::getPointer<double>(mesh_node["fields/closest_point/values/x"]), dim * npts);
      xfer_node["debug/min_distance"].set_external(internal::getPointer<double>(mesh_node["fields/min_distance/values"]), npts);
      // clang-format on
    }

    if(m_isVerbose)
    {
      dumpNode(xfer_node, fmt::format("round_{}_r{}_begin.json", 0, m_rank));
    }

    // Find initial values on this rank
    computeLocalClosestPoints(xfer_node, true);

    if(m_isVerbose)
    {
      dumpNode(xfer_node, fmt::format("round_{}_r{}_end.json", 0, m_rank));
    }

    if(m_nranks > 1)
    {
      // arbitrary tags for sending data to other ranks and getting it back
      const int tag_before = 1234;
      const int tag_after = 4321;

      // NOTE: uses a naive algorithm to computed distributed closest points
      // Every rank sends it data to every other rank.
      // TODO: Devise a more efficient algorithm to only send data to ranks with closer points
      for(int i = 1; i < m_nranks; ++i)
      {
        if(m_rank == 0)
        {
          SLIC_INFO(fmt::format("=======  Round {}/{} =======", i, m_nranks));
        }
        const int dst_rank = (m_rank + i) % m_nranks;
        const int rec_rank = (m_rank - i + m_nranks) % m_nranks;

        if(m_isVerbose)
        {
          SLIC_INFO(fmt::format("Rank {} -- sending to dst {}", m_rank, dst_rank));
        }

        if(m_isVerbose)
        {
          dumpNode(xfer_node, fmt::format("round_{}_r{}_begin.json", i, m_rank));
        }

        // send and receive the query point data
        conduit::Node rec_node;
        {
          internal::isend_using_schema(xfer_node,
                                       dst_rank,
                                       tag_before,
                                       MPI_COMM_WORLD);
          conduit::relay::mpi::recv_using_schema(rec_node,
                                                 rec_rank,
                                                 tag_before,
                                                 MPI_COMM_WORLD);
        }

        const int src_rank = rec_node["src_rank"].value();
        if(m_isVerbose)
        {
          dumpNode(
            rec_node,
            fmt::format("round_{}_r{}_comm_from_{}_A.json", i, m_rank, src_rank));
        }

        // compute the local data
        computeLocalClosestPoints(rec_node, false);

        if(m_isVerbose)
        {
          dumpNode(
            rec_node,
            fmt::format("round_{}_r{}_comm_from_{}_B.json", i, m_rank, src_rank));
        }

        // update results
        conduit::Node proc_node;
        {
          internal::isend_using_schema(rec_node,
                                       src_rank,
                                       tag_after,
                                       MPI_COMM_WORLD);
          conduit::relay::mpi::recv_using_schema(proc_node,
                                                 dst_rank,
                                                 tag_after,
                                                 MPI_COMM_WORLD);
        }

        if(m_isVerbose)
        {
          dumpNode(
            proc_node,
            fmt::format("round_{}_r{}_comm_from_{}_C.json", i, m_rank, dst_rank));
        }

        // copy data to mesh_node from proc_node
        const int npts = proc_node["npts"].value();
        axom::copy(xfer_node["cp_rank"].data_ptr(),
                   proc_node["cp_rank"].data_ptr(),
                   npts * sizeof(axom::IndexType));
        axom::copy(xfer_node["cp_index"].data_ptr(),
                   proc_node["cp_index"].data_ptr(),
                   npts * sizeof(axom::IndexType));
        axom::copy(xfer_node["closest_point"].data_ptr(),
                   proc_node["closest_point"].data_ptr(),
                   npts * sizeof(PointType));

        if(m_isVerbose)
        {
          dumpNode(mesh_node,
                   axom::fmt::format("round_{}_r{}_end.json", i, m_rank));

          SLIC_ASSERT_MSG(
            conduit::blueprint::mcarray::is_interleaved(
              mesh_node["fields/closest_point/values"]),
            fmt::format("After copy on iteration {}, 'closest_point' field of "
                        "'mesh_node' is not interleaved",
                        i));
        }

        MPI_Barrier(MPI_COMM_WORLD);
        slic::flushStreams();
      }
    }
  }

  void computeLocalClosestPoints(conduit::Node& xfer_node, bool is_first) const
  {
    using axom::primal::squared_distance;
    using int32 = axom::int32;

    // Extract the dimension and number of points from the coordinate values group
    const int dim = xfer_node["dim"].value();
    const int npts = xfer_node["npts"].value();
    SLIC_ASSERT(dim == NDIMS);

    /// Extract fields from the input node as ArrayViews
    auto queryPts =
      internal::ArrayView_from_Node<PointType>(xfer_node["coords"], npts);
    auto cpIndexes =
      internal::ArrayView_from_Node<axom::IndexType>(xfer_node["cp_index"], npts);
    auto cpRanks =
      internal::ArrayView_from_Node<axom::IndexType>(xfer_node["cp_rank"], npts);
    auto closestPts =
      internal::ArrayView_from_Node<PointType>(xfer_node["closest_point"], npts);

    /// Create ArrayViews in ExecSpace that are compatible with fields
    // TODO: Avoid copying arrays (here and at the end) if both are on the host
    auto cp_idx = is_first
      ? axom::Array<axom::IndexType>(npts, npts, m_allocatorID)
      : axom::Array<axom::IndexType>(cpIndexes, m_allocatorID);
    auto cp_ranks = is_first
      ? axom::Array<axom::IndexType>(npts, npts, m_allocatorID)
      : axom::Array<axom::IndexType>(cpRanks, m_allocatorID);

    /// PROBLEM: The striding does not appear to be retained by conduit relay
    ///          We might need to transform it? or to use a single array w/ pointers into it?
    auto cp_pos = is_first ? axom::Array<PointType>(npts, npts, m_allocatorID)
                           : axom::Array<PointType>(closestPts, m_allocatorID);

    // DEBUG
    auto minDist =
      internal::ArrayView_from_Node<double>(xfer_node["debug/min_distance"],
                                            npts);
    auto cp_dist = is_first ? axom::Array<double>(npts, npts, m_allocatorID)
                            : axom::Array<double>(minDist, m_allocatorID);
    ;
    auto query_min_dist = cp_dist.view();
    // END DEBUG

    if(is_first)
    {
      cp_ranks.fill(-1);
      cp_idx.fill(-1);
    }
    auto query_inds = cp_idx.view();
    auto query_ranks = cp_ranks.view();
    auto query_pos = cp_pos.view();

    /// Create an ArrayView in ExecSpace that is compatible with queryPts
    PointArray execPoints(queryPts, m_allocatorID);
    auto query_pts = execPoints.view();

    // Get a device-useable iterator
    auto it = m_bvh.getTraverser();
    const int rank = m_rank;

    AXOM_PERF_MARK_SECTION(
      "ComputeClosestPoints",
      axom::for_all<ExecSpace>(
        npts,
        AXOM_LAMBDA(int32 idx) mutable {
          PointType qpt = query_pts[idx];

          MinCandidate curr_min {};
          if(query_ranks[idx] >= 0)  // i.e. we've already found a candidate closest
          {
            curr_min.minSqDist = squared_distance(qpt, query_pos[idx]);
            curr_min.minElem = query_inds[idx];
            curr_min.minRank = query_ranks[idx];
          }

          auto searchMinDist = [&](int32 current_node, const int32* leaf_nodes) {
            const int candidate_idx = leaf_nodes[current_node];
            const PointType candidate_pt = m_points[candidate_idx];
            const double sq_dist = squared_distance(qpt, candidate_pt);

            if(sq_dist < curr_min.minSqDist)
            {
              curr_min.minSqDist = sq_dist;
              curr_min.minElem = candidate_idx;
              curr_min.minRank = rank;
            }
          };

          auto traversePredicate = [&](const PointType& p,
                                       const BoxType& bb) -> bool {
            return squared_distance(p, bb) <= curr_min.minSqDist;
          };

          // Traverse the tree, searching for the point with minimum distance.
          it.traverse_tree(qpt, searchMinDist, traversePredicate);

          // If modified, update the fields that changed
          if(curr_min.minRank == rank)
          {
            // SLIC_INFO(
            //   fmt::format("[On rank {}] Updating dist for particle {} ::"
            //               "{{ cp: {}->{}; "
            //               "dist: {}->{}; "
            //               "rank: {}->{}; "
            //               "elem: {}->{}}}",
            //               rank,
            //               idx,
            //               query_pos[idx],             //cp
            //               m_points[curr_min.minElem],
            //               query_min_dist[idx],        // dist
            //               sqrt(curr_min.minSqDist),
            //               query_ranks[idx],           // rank
            //               curr_min.minRank,
            //               query_inds[idx],            // index
            //               curr_min.minElem));

            query_inds[idx] = curr_min.minElem;
            query_ranks[idx] = curr_min.minRank;
            query_pos[idx] = m_points[curr_min.minElem];

            //DEBUG
            query_min_dist[idx] = sqrt(curr_min.minSqDist);
          }
          // else
          // {
          //   SLIC_INFO(
          //     fmt::format("[On rank {}] Didn't update dist for particle {} ::"
          //                 "{{ cp: {}; "
          //                 "dist: {}; "
          //                 "rank: {}; "
          //                 "elem: {}}}",
          //                 rank,
          //                 idx,
          //                 query_pos[idx],
          //                 query_min_dist[idx],
          //                 query_ranks[idx],
          //                 query_inds[idx]));
          // }
        }););

    axom::copy(cpIndexes.data(),
               query_inds.data(),
               cpIndexes.size() * sizeof(axom::IndexType));
    axom::copy(cpRanks.data(),
               query_ranks.data(),
               cpRanks.size() * sizeof(axom::IndexType));
    axom::copy(closestPts.data(),
               query_pos.data(),
               closestPts.size() * sizeof(PointType));

    // DEBUG
    axom::copy(minDist.data(),
               query_min_dist.data(),
               minDist.size() * sizeof(double));
  }

private:
  /// Check validity of blueprint group
  bool isValidBlueprint(const conduit::Node& mesh_node) const
  {
    bool success = true;
    conduit::Node info;
    if(!conduit::blueprint::mpi::verify("mesh", mesh_node, info, MPI_COMM_WORLD))
    {
      SLIC_INFO(
        fmt::format("Invalid blueprint on rank {} for particle mesh: \n{}",
                    m_rank,
                    info.to_yaml()));
      success = false;
    }

    return success;
  }

  /// Helper function to extract the dimension from the coordinate values group
  int extractDimension(const conduit::Node& values_node) const
  {
    SLIC_ASSERT(values_node.has_child("x"));
    return values_node.has_child("z") ? 3 : (values_node.has_child("y") ? 2 : 1);
  }

  /// Helper function to extract the number of points from the coordinate values group
  int extractSize(const conduit::Node& values_node) const
  {
    SLIC_ASSERT(values_node.has_child("x"));
    return values_node["x"].dtype().number_of_elements();
  }

  /**
   * \brief Extracts a field \a fieldName from the mesh blueprint
   * 
   * \tparam T The type for the underlying array
   * \param mesh_node The conduit node at the root of the mesh blueprint
   * \param field_name The name of the field
   * \param field_template Template string for the path to the field
   * \param num_points The size of the field
   * \return An arrayview over the field data
   */
  template <typename T>
  axom::ArrayView<T> extractField(conduit::Node& mesh_node,
                                  std::string&& field_name,
                                  std::string&& path_template,
                                  int num_points) const
  {
    const std::string path = axom::fmt::format(path_template, field_name);
    SLIC_ASSERT_MSG(
      mesh_node.has_path(path),
      fmt::format(
        "Input to `computeClosestPoint()` must have a field named `{}`",
        field_name));

    return internal::ArrayView_from_Node<T>(mesh_node[path], num_points);
  }

private:
  PointArray m_points;
  BoxArray m_boxes;
  BVHTreeType m_bvh;

  int m_allocatorID;
  bool m_isVerbose {false};

  int m_rank;
  int m_nranks;
};

}  // end namespace quest
}  // end namespace axom

#endif  //  QUEST_DISTRIBUTED_CLOSEST_POINT_H_
