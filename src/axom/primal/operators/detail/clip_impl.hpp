// Copyright (c) 2017-2021, Lawrence Livermore National Security, LLC and
// other Axom Project Developers. See the top-level LICENSE file for details.
//
// SPDX-License-Identifier: (BSD-3-Clause)

/*!
 * \file clip_impl.hpp
 *
 * \brief Helper functions for the primal clipping operators
 */

#ifndef AXOM_PRIMAL_CLIP_IMPL_HPP_
#define AXOM_PRIMAL_CLIP_IMPL_HPP_

#include "axom/primal/geometry/Point.hpp"
#include "axom/primal/geometry/Triangle.hpp"
#include "axom/primal/geometry/BoundingBox.hpp"
#include "axom/primal/geometry/Polygon.hpp"
#include "axom/primal/geometry/Polyhedron.hpp"
#include "axom/primal/geometry/Octahedron.hpp"
#include "axom/primal/geometry/Plane.hpp"
#include "axom/primal/geometry/Segment.hpp"
#include "axom/primal/geometry/Tetrahedron.hpp"

#include "axom/primal/operators/intersect.hpp"
#include "axom/primal/operators/orientation.hpp"

namespace axom
{
namespace primal
{
namespace detail
{
/*! Returns true when index is even */
inline bool isEven(int index) { return (index & 1) == 0; }

/*!
 * \brief Specialized point plane classifier for axis aligned planes
 *
 * \param [in] pt The plane to classify
 * \param [in] index The index of the axis aligned plane. See below for mapping
 * \param [in] val The plane's coordinate with respect to the given axis
 * \param [in] eps A parameter for thickening width of the plane (default 1e-8)
 *
 * Mapping of index to axis
 * * 0 -> -x axis
 * * 1 -> +x axis
 * * 2 -> -y axis
 * * 3 -> +y axis
 * * 4 -> -z axis
 * * 5 -> +z axis
 *
 * \return An OrientedSide (ON_POSITIVE_SIDE, ON_NEGATIVE_SIDE, ON_BOUNDARY)
 *         value based on the relative orientations of point pt and the
 *         corresponding plane associated with index.
 * \see OrientedPlane enum
 */
template <typename T, int NDIMS>
int classifyPointAxisPlane(const Point<T, NDIMS>& pt,
                           int index,
                           T val,
                           const double eps = 1e-8)
{
  // Note: we are exploiting the fact that the planes are axis aligned
  // So the dot product is +/- the given coordinate.
  // In general, we would need to call distance(pt, plane) here
  T dist = isEven(index) ? val - pt[index / 2] : pt[index / 2] - val;

  if(dist > eps)
  {
    return ON_POSITIVE_SIDE;
  }
  if(dist < -eps)
  {
    return ON_NEGATIVE_SIDE;
  }

  return ON_BOUNDARY;
}

/*!
 * \brief Finds the clipping intersection point between points a and b.
 *
 * \param [in] a The point behind the plane
 * \param [in] b The point in front of the plane
 * \param [in] index The index of the axis aligned plane.
 * \param [in] val The plane's coordinate with respect to the given axis
 * \return The point between a and b whose corresponding coordinate is val
 *
 * \see classifyPointAxisPlane for description of how index maps to coordinates.
 */
template <typename T, int NDIMS>
Point<T, NDIMS> findIntersectionPoint(const Point<T, NDIMS>& a,
                                      const Point<T, NDIMS>& b,
                                      int index,
                                      T val)
{
  using PointType = Point<T, NDIMS>;

  // Need to find a parameter t for the point pt, such that,
  // * 0 <= t <= 1
  // * pt = a + t * (b-a)
  // * pt[ index/2]  == val

  T t = (val - a[index / 2]) / (b[index / 2] - a[index / 2]);
  SLIC_ASSERT(0. <= t && t <= 1.);

  PointType ret = PointType(a.array() + t * (b.array() - a.array()));
  SLIC_ASSERT(classifyPointAxisPlane(ret, index, val) == ON_BOUNDARY);

  return ret;
}

/*!
 * \brief Clips the vertices of the polygon to be behind the plane.
 *
 * This is a specialization of the Sutherland-Hodgeman clipping algorithm
 * for axis-aligned planes
 *
 * \param [in] prevPoly  An input polygon with the vertices to clip
 * \param [out] currentPoly An output polygon whose coordinates are clipped
 *                          against this plane.
 * \param [in] index The index of the axis aligned plane.
 * \param [in] val The plane's coordinate with respect to the given axis
 *
 * \note Algorithm for robust clipping against "thick" planes derived from
 *       Section 8.3 of Christer Ericson's "Real-Time Collision Detection"
 *       and is based on the Sutherland-Hodgeman clipping algorithm.
 *       We are only keeping the "back" polygon, w.r.t. that algorithm.
 * \see classifyPointAxisPlane for description of how index maps to coordinates.
 */
template <typename T, int NDIMS>
void clipAxisPlane(const Polygon<T, NDIMS>* prevPoly,
                   Polygon<T, NDIMS>* currentPoly,
                   int index,
                   T val)
{
  using PointType = Point<T, NDIMS>;

  currentPoly->clear();
  int numVerts = prevPoly->numVertices();

  if(numVerts == 0)
  {
    return;
  }

  // Initialize point a with the last vertex of the polygon
  const PointType* a = &(*prevPoly)[numVerts - 1];
  int aSide = classifyPointAxisPlane(*a, index, val);

  for(int i = 0; i < numVerts; ++i)
  {
    const PointType* b = &(*prevPoly)[i];
    int bSide = classifyPointAxisPlane(*b, index, val);

    switch(bSide)
    {
    case ON_POSITIVE_SIDE:
      if(aSide == ON_NEGATIVE_SIDE)
      {
        currentPoly->addVertex(findIntersectionPoint(*a, *b, index, val));
      }
      break;
    case ON_BOUNDARY:
      if(aSide == ON_NEGATIVE_SIDE)
      {
        currentPoly->addVertex(*b);
      }
      break;
    case ON_NEGATIVE_SIDE:
      switch(aSide)
      {
      case ON_POSITIVE_SIDE:
        currentPoly->addVertex(findIntersectionPoint(*a, *b, index, val));
        currentPoly->addVertex(*b);
        break;
      case ON_BOUNDARY:
        currentPoly->addVertex(*a);
        currentPoly->addVertex(*b);
        break;
      case ON_NEGATIVE_SIDE:
        currentPoly->addVertex(*b);
        break;
      }
      break;
    }

    // swap a and b
    a = b;
    aSide = bSide;
  }
}

/*!
 * \brief Finds the clipped intersection Polyhedron between Octahedron
 *        oct and Tetrahedron tet.
 *
 * \param [in] oct The octahedron
 * \param [in] tet The tetrahedron
 * \param [in] eps The tolerance for plane point orientation.
 * \return The Polyhedron formed from clipping the octahedron with a tetrahedron.
 *
 */
template <typename T, int NDIMS>
Polyhedron<T, NDIMS> clipOctahedron(const Octahedron<T, NDIMS>& oct,
                                    const Tetrahedron<T, NDIMS>& tet,
                                    double eps = 1.e-24)
{
  using BoxType = BoundingBox<T, NDIMS>;
  using PlaneType = Plane<T, NDIMS>;
  using PointType = Point<T, NDIMS>;
  using SegmentType = Segment<T, NDIMS>;

  // Initialize our polyhedron to return
  Polyhedron<T, NDIMS> poly;

  poly.addVertex(oct[0].data());
  poly.addVertex(oct[1].data());
  poly.addVertex(oct[2].data());
  poly.addVertex(oct[3].data());
  poly.addVertex(oct[4].data());
  poly.addVertex(oct[5].data());

  poly.addNeighbors(poly[0], {1, 5, 4, 2});
  poly.addNeighbors(poly[1], {0, 2, 3, 5});
  poly.addNeighbors(poly[2], {0, 4, 3, 1});
  poly.addNeighbors(poly[3], {1, 2, 4, 5});
  poly.addNeighbors(poly[4], {0, 5, 3, 2});
  poly.addNeighbors(poly[5], {0, 1, 3, 4});

  //Bounding Box of Polyhedron
  BoxType polyBox(&oct[0], 6);

  // Initialize planes from tetrahedron vertices
  // (Ordering here matters to get the correct winding)
  PlaneType planes[4] = {PlaneType(tet[1].data(), tet[3].data(), tet[2].data()),
                         PlaneType(tet[0].data(), tet[2].data(), tet[3].data()),
                         PlaneType(tet[0].data(), tet[3].data(), tet[1].data()),
                         PlaneType(tet[0].data(), tet[1].data(), tet[2].data())};

  //Clip octahedron by each plane
  for(int planeIndex = 0; planeIndex < 4; planeIndex++)
  {
    // Each bit value indicates if that Polyhedron vertex is formed from
    // Octahedron clipping with a plane.
    unsigned int clipped = 0;

    PlaneType plane = planes[planeIndex];

    // Check that plane intersects Polyhedron
    if(intersect(plane, polyBox))
    {
      // Loop over Polyhedron vertices
      int numVerts = poly.numVertices();
      for(int i = 0; i < numVerts; i++)
      {
        int orientation = plane.getOrientation(poly[i].data(), eps);

        // Vertex is under the plane
        if(orientation == ON_NEGATIVE_SIDE)
        {
          // Check neighbors for vertex above the plane (edge clipped by plane)
          int numNeighbors = poly.getNeighbors(i).size();
          for(int j = 0; j < numNeighbors; j++)
          {
            int neighborIndex = poly.getNeighbors(i)[j];

            int neighborOrientation =
              plane.getOrientation(poly[neighborIndex].data(), eps);

            // Insert new vertex to polyhedron, where edge intersects plane.
            if(neighborOrientation == ON_POSITIVE_SIDE)
            {
              int newVertexIndex = poly.numVertices();

              T lerp_val;
              SegmentType seg(PointType(poly[i].data()),
                              PointType(poly[neighborIndex].data()));
              intersect(plane, seg, lerp_val);
              poly.addVertex((seg.at(lerp_val)).data());

              poly.addNeighbors(poly[newVertexIndex], {i, neighborIndex});

              // Label the new vertex as a clipped created vertex
              clipped |= 1 << newVertexIndex;

              // Update current vertex's & neighbor's neighbors with the
              // new vertex
              poly.getNeighbors(i)[j] = newVertexIndex;
              for(unsigned int k = 0; k < poly.getNeighbors(neighborIndex).size();
                  k++)
              {
                if(poly.getNeighbors(neighborIndex)[k] == i)
                {
                  poly.getNeighbors(neighborIndex)[k] = newVertexIndex;
                }
              }
            }
          }
        }
      }  // end of loop over Polyhedron vertices

      // Fix the neighbors list

      int nverts = poly.numVertices();

      // Make copy of current neighbors
      std::vector<std::vector<int>> old_neighbors(nverts);
      for(int i = 0; i < nverts; i++)
      {
        old_neighbors[i] = poly.getNeighbors(i);
      }

      for(int i = 0; i < nverts; i++)
      {
        // Check clipped created vertices first, then vertices on the plane
        int vIndex = (i + numVerts) % nverts;
        int vOrientation = plane.getOrientation(poly[vIndex].data(), eps);

        if(((clipped & (1 << vIndex))) || vOrientation == ON_BOUNDARY)
        {
          for(unsigned int j = 0; j < poly.getNeighbors(vIndex).size(); j++)
          {
            int neighborIndex = poly.getNeighbors(vIndex)[j];
            int neighborOrientation =
              plane.getOrientation(poly[neighborIndex].data(), eps);

            // This neighbor is below the plane
            if(neighborOrientation == ON_NEGATIVE_SIDE)
            {
              // Look for 1st vertex along this face not below the plane.
              int iprev = vIndex;
              int inext = neighborIndex;
              int itmp = inext;

              int val = 0;

              while((plane.getOrientation(poly[inext].data(), eps) ==
                     ON_NEGATIVE_SIDE) &&
                    (val++ < nverts))
              {
                itmp = inext;
                for(unsigned int ni = 0; ni < poly.getNeighbors(inext).size();
                    ni++)
                {
                  if(poly.getNeighbors(inext)[ni] == iprev)
                  {
                    inext = (ni == 0)
                      ? poly.getNeighbors(
                          inext)[poly.getNeighbors(inext).size() - 1]
                      : poly.getNeighbors(inext)[ni - 1];
                    break;
                  }
                }

                iprev = itmp;
              }

              // Remove neighbor from list if vertex found was already a neighbor or
              // is the vertex we are currently checking for.
              if(poly.getNeighbors(
                   vIndex)[(j + 1) % poly.getNeighbors(vIndex).size()] == inext ||
                 inext == vIndex)
              {
                poly.getNeighbors(vIndex)[j] = -1;
              }

              // Otherwise update neighbor lists of vertex found and vertex we are checking for.
              else
              {
                poly.getNeighbors(vIndex)[j] = inext;

                if((clipped & (1 << inext)))
                {
                  poly.getNeighbors(inext).insert(
                    poly.getNeighbors(inext).begin(),
                    vIndex);
                  old_neighbors[inext].insert(old_neighbors[inext].begin(), -1);
                }
                else
                {
                  int offset;
                  for(unsigned int oi = 0; oi < old_neighbors[inext].size(); oi++)
                  {
                    if(old_neighbors[inext][oi] == iprev)
                    {
                      offset = oi;
                      break;
                    }

                    // Max offset
                    if(oi == old_neighbors[inext].size() - 1)
                    {
                      offset = old_neighbors[inext].size();
                    }
                  }
                  poly.getNeighbors(inext).insert(
                    poly.getNeighbors(inext).begin() + offset,
                    vIndex);
                  old_neighbors[inext].insert(
                    old_neighbors[inext].begin() + offset,
                    vIndex);
                }
              }
            }
          }
        }
      }

      // Remove neighbors below the plane
      for(int i = 0; i < nverts; i++)
      {
        poly.getNeighbors(i).erase(std::remove(poly.getNeighbors(i).begin(),
                                               poly.getNeighbors(i).end(),
                                               -1),
                                   poly.getNeighbors(i).end());
      }

      // Generate new bounding box for polyhedron
      polyBox = BoxType();

      // Dictionary for old indices to new indices positions
      std::vector<int> newIndices(poly.numVertices());

      int curIndex = 0;
      for(int i = 0; i < poly.numVertices(); i++)
      {
        if(plane.getOrientation(poly[i].data(), eps) < ON_NEGATIVE_SIDE)
        {
          newIndices[i] = curIndex++;
          polyBox.addPoint(PointType(poly[i].data()));
        }
      }

      // Renumbering neighbors
      for(int i = 0; i < poly.numVertices(); i++)
      {
        if(plane.getOrientation(poly[i].data(), eps) < ON_NEGATIVE_SIDE)
        {
          for(unsigned int j = 0; j < poly.getNeighbors(i).size(); j++)
          {
            poly.getNeighbors(i)[j] = newIndices[poly.getNeighbors(i)[j]];
          }
        }
      }

      // Remove vertices below the plane from polyhedron
      for(int i = 0; i < poly.numVertices(); i++)
      {
        if(plane.getOrientation(poly[i].data(), eps) == ON_NEGATIVE_SIDE)
        {
          poly.getVertices().erase(poly.getVertices().begin() + i);
          poly.getNeighbors().erase(poly.getNeighbors().begin() + i);
          i--;
        }
      }
    }
  }

  return poly;
}

}  // namespace detail
}  // namespace primal
}  // namespace axom

#endif  // AXOM_PRIMAL_CLIP_IMPL_HPP_
