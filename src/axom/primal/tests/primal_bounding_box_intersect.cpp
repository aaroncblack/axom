// Copyright (c) 2017-2020, Lawrence Livermore National Security, LLC and
// other Axom Project Developers. See the top-level COPYRIGHT file for details.
//
// SPDX-License-Identifier: (BSD-3-Clause)

#include "axom/primal/operators/detail/intersect_bounding_box_impl.hpp"

#include "gtest/gtest.h"

//------------------------------------------------------------------------------
// UNIT TESTS
//------------------------------------------------------------------------------
TEST( primal_bounding_box_intersect, aabb_aabb_adjacent )
{
  constexpr double LO = 0.0f;
  constexpr double HI = 1.0f;
  constexpr int NUM_OFFSETS = 3;
  const double OFFSETS[] = { -1.0f, 0.0f, 1.0f };

  using namespace axom::primal::detail;

  // Check 2D adjacent and self-intersecting bounding boxes for intersection
  for ( int i = 0; i < NUM_OFFSETS; i++ )
  {
	for ( int j = 0; j < NUM_OFFSETS; j++ )
	{
	  EXPECT_TRUE( intersect_bounding_box( LO + OFFSETS[i], HI + OFFSETS[i],
		                                   LO + OFFSETS[j], HI + OFFSETS[j],
		                                   LO, HI, LO, HI ) );   	  
	}
  }

  // Check 3D adjacent and self-intersecting bounding boxes for intersection
  for ( int i = 0; i < NUM_OFFSETS; i++ )
  {
	for ( int j = 0; j < NUM_OFFSETS; j++ )
	{
	  for ( int k = 0; k < NUM_OFFSETS; k++ )
	  {
	    EXPECT_TRUE( intersect_bounding_box( LO + OFFSETS[i], HI + OFFSETS[i],
		                                     LO + OFFSETS[j], HI + OFFSETS[j],
		                                     LO + OFFSETS[k], HI + OFFSETS[k],
		                                     LO, HI, LO, HI, LO, HI ) );  
	  } 	  
	}
  }

}

//------------------------------------------------------------------------------
TEST( primal_bounding_box_intersect, aabb_aabb_non_intersecting )
{
  constexpr double LO = 0.0f;
  constexpr double HI = 1.0f;
  constexpr int NUM_OFFSETS = 3;
  const double OFFSETS[] = { -1.01f, 0.0f, 1.01f };

  using namespace axom::primal::detail;

  // Check 2D bounding boxes for non-intersection
  for ( int i = 0; i < NUM_OFFSETS; i++ )
  {
	for ( int j = 0; j < NUM_OFFSETS; j++ )
	{
	  // Ignore identity box
	  if ( i != 1 && j != 1 )
	  {
        EXPECT_FALSE( intersect_bounding_box( LO + OFFSETS[i],
        	                                  HI + OFFSETS[i],
		                                      LO + OFFSETS[j],
		                                      HI + OFFSETS[j],
		                                      LO, HI, LO, HI ) ); 
	  }  	  
	}
  }
	
  // Check 3D bounding boxes for non-intersection
  for ( int i = 0; i < NUM_OFFSETS; i++ )
  {
	for ( int j = 0; j < NUM_OFFSETS; j++ )
	{
	  for ( int k = 0; k < NUM_OFFSETS; k++ )
	  {
	    // Ignore identity box
	    if ( i != 1 && j != 1 )
	    {
          EXPECT_FALSE( intersect_bounding_box( LO + OFFSETS[i], 
          	                                    HI + OFFSETS[i],
		                                        LO + OFFSETS[j], 
		                                        HI + OFFSETS[j],
		                                        LO + OFFSETS[j], 
		                                        HI + OFFSETS[j],
		                                        LO, HI, LO, HI, LO, HI ) );
	    } 
	  } 	  
	}
  }

}

//------------------------------------------------------------------------------
#include "axom/slic/core/UnitTestLogger.hpp"
using axom::slic::UnitTestLogger;

int main(int argc, char* argv[])
{
  int result = 0;

  ::testing::InitGoogleTest(&argc, argv);

  UnitTestLogger logger;  // create & initialize test logger,

  // finalized when exiting main scope

  result = RUN_ALL_TESTS();

  return result;
} 