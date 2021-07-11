// Copyright (c) 2017-2021, Lawrence Livermore National Security, LLC and
// other Axom Project Developers. See the top-level LICENSE file for details.
//
// SPDX-License-Identifier: (BSD-3-Clause)

#include "axom/config.hpp"

#ifndef AXOM_USE_C2C
  #error These tests should only be included when Axom is configured with C2C
#endif

#include "axom/quest/readers/C2CReader.hpp"
#include "axom/slic.hpp"
#include "axom/mint.hpp"
#include "axom/primal.hpp"
#include "fmt/fmt.hpp"

// gtest includes
#include "gtest/gtest.h"

// C/C++ includes
#include <cstdio>
#include <string>
#include <fstream>
#include <limits>
#include <math.h>

// namespace aliases
namespace mint = axom::mint;
namespace primal = axom::primal;
namespace quest = axom::quest;

namespace
{
static const std::string C2C_LINE_FILENAME = "test_line.contour";
static const std::string C2C_CIRCLE_FILENAME = "test_circle.contour";
static const std::string C2C_SQUARE_FILENAME = "test_square.contour";
static const std::string C2C_SPLINE_FILENAME = "test_spline.contour";
}  // end anonymous namespace

/// Writes out a c2c file for a circle
void writeSimpleCircle(const std::string& filename)
{
  std::ofstream c2cFile(filename, std::ios::out);
  c2cFile
    << "piece = circle(origin=(0cm, 0cm), radius=1cm, start=0deg, end=360deg)"
    << std::endl;
}

/// Writes out a c2c file for a line
void writeSimpleLine(const std::string& filename)
{
  std::ofstream c2cFile(filename, std::ios::out);
  c2cFile << "piece = line(start=(0cm, 0cm), end=(1cm, 1cm))" << std::endl;
}

/// Writes out a c2c file for a unit square
void writeSquare(const std::string& filename)
{
  std::ofstream c2cFile(filename, std::ios::out);
  c2cFile << "point = start" << std::endl;
  c2cFile << "piece = line(start=(0cm, 0cm), end=(1cm, 0cm))" << std::endl;
  c2cFile << "piece = line()" << std::endl;
  c2cFile << "piece = line(start=(1cm, 1cm), end=(0cm, 1cm))" << std::endl;
  c2cFile << "piece = line(end=start)" << std::endl;
}

/// Writes out a c2c file for a rectangle cut by a cosine wave
void writeSpline(const std::string& filename)
{
  std::vector<std::string> pts;
  // generate a cubic spline approximation to a cosine wave
  // with domain [0, 2*PI] and range defined by a y-offset, amplitude and frequency
  const double offset = 1.;
  const double amplitude = .5;
  const double freq = 3;
  const int NPTS = 2 * freq;
  for(int i = 0; i <= NPTS; ++i)
  {
    double x = 2 * M_PI * static_cast<double>(i) / NPTS;
    double y = offset + amplitude * cos(freq * x);
    pts.emplace_back(fmt::format("{} {}", x, y));
  }

  std::ofstream c2cFile(filename, std::ios::out);
  // output sine wave spline
  c2cFile << "point = spline_start" << std::endl;
  c2cFile << fmt::format(
               "piece = rz(units=cm, spline=cubic, beginTan=-90deg, "
               "endTan=-90deg, rz={})",
               fmt::join(pts.rbegin(), pts.rend(), "\n\t\t"))
          << std::endl;
  c2cFile << "point = spline_end" << std::endl;

  // add straight edges within first quadrant
  c2cFile << "piece = line(end=(0cm,0cm))" << std::endl;
  c2cFile << fmt::format("piece = line(end=({}cm,0cm))", 2 * M_PI) << std::endl;
  c2cFile << "piece = line(end=spline_start)" << std::endl;
}

TEST(quest_c2c_reader, basic_read)
{
  std::string fileName = C2C_CIRCLE_FILENAME;
  writeSimpleCircle(fileName);

  quest::C2CReader reader;
  reader.setFileName(fileName);

  reader.read();
  reader.log();
}

TEST(quest_c2c_reader, interpolate_circle)
{
  std::string fileName = C2C_CIRCLE_FILENAME;
  writeSimpleCircle(fileName);

  quest::C2CReader reader;
  reader.setFileName(fileName);

  reader.read();
  reader.log();

  const int DIM = 2;
  using MeshType = mint::UnstructuredMesh<mint::SINGLE_SHAPE>;
  MeshType* mesh = new MeshType(DIM, mint::SEGMENT);

  int segmentsPerPiece = 100;
  reader.getLinearMesh(mesh, segmentsPerPiece);

  SLIC_INFO(fmt::format("Mesh has {} nodes and {} cells",
                        mesh->getNumberOfNodes(),
                        mesh->getNumberOfCells()));
  EXPECT_EQ(segmentsPerPiece + 1, mesh->getNumberOfNodes());
  EXPECT_EQ(segmentsPerPiece, mesh->getNumberOfCells());

  // This is a unit circle; check that its vertices have unit magnitude
  {
    double* x = mesh->getCoordinateArray(mint::X_COORDINATE);
    double* y = mesh->getCoordinateArray(mint::Y_COORDINATE);
    const int numPts = mesh->getNumberOfNodes();
    for(int i = 0; i < numPts; ++i)
    {
      double mag = primal::Vector<double, 2> {x[i], y[i]}.norm();
      EXPECT_DOUBLE_EQ(1., mag);
    }
  }

  mint::write_vtk(mesh, "test_circle.vtk");

  delete mesh;
}

TEST(quest_c2c_reader, interpolate_square)
{
  std::string fileName = C2C_SQUARE_FILENAME;
  writeSquare(fileName);

  quest::C2CReader reader;
  reader.setFileName(fileName);

  reader.read();
  reader.log();

  const int DIM = 2;
  using MeshType = mint::UnstructuredMesh<mint::SINGLE_SHAPE>;
  MeshType* mesh = new MeshType(DIM, mint::SEGMENT);

  int segmentsPerPiece = 10;
  reader.getLinearMesh(mesh, segmentsPerPiece);

  SLIC_INFO(fmt::format("Mesh has {} nodes and {} cells",
                        mesh->getNumberOfNodes(),
                        mesh->getNumberOfCells()));

  const int expVerts = 4 * (segmentsPerPiece + 1);
  EXPECT_EQ(expVerts, mesh->getNumberOfNodes());

  const int expSegs = 4 * segmentsPerPiece;
  EXPECT_EQ(expSegs, mesh->getNumberOfCells());

  mint::write_vtk(mesh, "test_square.vtk");
}

TEST(quest_c2c_reader, interpolate_spline)
{
  std::string fileName = C2C_SPLINE_FILENAME;
  writeSpline(fileName);

  quest::C2CReader reader;
  reader.setFileName(fileName);

  reader.read();
  reader.log();

  const int DIM = 2;
  using MeshType = mint::UnstructuredMesh<mint::SINGLE_SHAPE>;
  MeshType* mesh = new MeshType(DIM, mint::SEGMENT);

  int segmentsPerPiece = 200;
  reader.getLinearMesh(mesh, segmentsPerPiece);

  SLIC_INFO(fmt::format("Mesh has {} nodes and {} cells",
                        mesh->getNumberOfNodes(),
                        mesh->getNumberOfCells()));

  mint::write_vtk(mesh, "test_spline.vtk");
}

//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  axom::slic::SimpleLogger logger;

  return RUN_ALL_TESTS();
}
