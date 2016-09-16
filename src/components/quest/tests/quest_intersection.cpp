/*
 * Copyright (c) 2015, Lawrence Livermore National Security, LLC.
 * Produced at the Lawrence Livermore National Laboratory.
 *
 * All rights reserved.
 *
 * This source code cannot be distributed without permission and further
 * review from Lawrence Livermore National Laboratory.
 */

/*!
 *******************************************************************************
 * \file
 *
 * \date Jan 5, 2016
 * \author George Zagaris (zagaris2@llnl.gov)
 *******************************************************************************
 */

#include "gtest/gtest.h"

#include "quest/BoundingBox.hpp"
#include "quest/Intersection.hpp"
#include "quest/Point.hpp"
#include "quest/Ray.hpp"
#include "quest/Segment.hpp"
#include "quest/Triangle.hpp"
#include "quest/Vector.hpp"


TEST( quest_intersection, ray_segment_intersection )
{
  typedef quest::Point< double,2 >   PointType;
  typedef quest::Segment< double,2 > SegmentType;
  typedef quest::Vector< double,2 >  VectorType;
  typedef quest::Ray< double,2 >     RayType;

  // STEP 0: construct segment
  PointType A(0.0);
  PointType B(1.0,1);
  SegmentType S( A, B );

  // STEP 1: construct ray
  PointType origin = PointType::make_point( 0.5,-0.5 );
  VectorType direction;
  direction[0] = 0.0;
  direction[1] = 0.5;
  RayType R( origin,direction.unitVector() );

  // STEP 2: compute intersection
  PointType ip;
  bool intersects = quest::intersect( R, S, ip );
  EXPECT_TRUE( intersects );
  EXPECT_DOUBLE_EQ(0.5,ip[0]);
  EXPECT_DOUBLE_EQ(0.0,ip[1]);

  // STEP 3: construct non-intersecting ray
  origin[1] = 0.5; // shift R up
  RayType R2( origin, direction.unitVector() );
  bool intersects2 = quest::intersect( R2, S, ip );
  EXPECT_FALSE( intersects2 );
}


TEST( quest_intersection, triangle_aabb_intersection )
{
  static int const DIM = 3;
  typedef quest::Point< double,DIM >   PointType;
  typedef quest::Triangle< double,DIM > TriangleType;
  typedef quest::BoundingBox< double,DIM > BoundingBoxType;

  double xArr[3] = { 1., 0., 0.};
  double yArr[3] = { 0., 1., 0.};
  double zArr[3] = { 0., 0., 1.};

  PointType ptX(xArr);
  PointType ptY(yArr);
  PointType ptZ(zArr);

  TriangleType unitTri( ptX, ptY, ptZ );
  BoundingBoxType unitBB( PointType::zero(), PointType::ones());

  EXPECT_TRUE( quest::intersect(unitTri, unitBB));

  // Let's first move the bounding box around
  BoundingBoxType v0_BB( ptX );
  v0_BB.expand(.1);
  SLIC_INFO("Testing v0 bounding box: " << v0_BB << " against unit triangle");
  EXPECT_TRUE( v0_BB.contains(ptX));
  EXPECT_TRUE( quest::intersect(unitTri, v0_BB) );

  BoundingBoxType v1_BB( ptY );
  v1_BB.expand(.1);
  SLIC_INFO("Testing v1 bounding box: " << v1_BB << " against unit triangle");
  EXPECT_TRUE( v1_BB.contains(ptY));
  EXPECT_TRUE( quest::intersect(unitTri, v1_BB) );

  BoundingBoxType v2_BB( ptZ );
  v2_BB.expand(.1);
  SLIC_INFO("Testing v2 bounding box: " << v2_BB << " against unit triangle");
  EXPECT_TRUE( v2_BB.contains(ptZ));
  EXPECT_TRUE( quest::intersect(unitTri, v2_BB) );


  BoundingBoxType mid_BB( PointType::zero());
  mid_BB.addPoint( PointType(0.9));
  SLIC_INFO("Testing bounding box: " << mid_BB << " against unit triangle.  Note -- BB should intersect interior of triangle");
  EXPECT_TRUE( quest::intersect(unitTri, mid_BB) );

  BoundingBoxType high_BB( PointType::ones());
  high_BB.addPoint( PointType(0.5));
  SLIC_INFO("Testing bounding box: " << high_BB << " against unit triangle.  Note -- BB should not intersect interior of triangle");
  EXPECT_FALSE( quest::intersect(unitTri, high_BB) );

  BoundingBoxType out_BB( PointType::ones());
  out_BB.addPoint( PointType(2));
  SLIC_INFO("Testing bounding box: " << out_BB << " against unit triangle.  Note -- BB should not intersect triangle");
  EXPECT_FALSE( quest::intersect(unitTri, out_BB) );


  BoundingBoxType negBB(PointType(-5), PointType(-10));
  SLIC_INFO("Testing bounding box: " << negBB << " against unit triangle.  Note -- BB should not intersect triangle");
  EXPECT_FALSE( quest::intersect(unitTri, negBB) );


  // Test new triangle whose edge crosses the BB
  double t2_0[3] = { 10., 0., 0.};
  double t2_1[3] = { -10.,0., 0.};
  double t2_2[3] = { 0., 100., 0};

  TriangleType xyTri( t2_0, t2_1, t2_2);
  BoundingBoxType bbOrigin(PointType::zero() );
  bbOrigin.expand(1.);
  SLIC_INFO("Testing bounding box: " << bbOrigin << " against triangle " << xyTri << ".  Note -- BB should not intersect triangle");
  EXPECT_TRUE( quest::intersect(xyTri, bbOrigin) );


  BoundingBoxType bbOrigin2(PointType::zero() );
  bbOrigin.addPoint( PointType(-1.));
  bbOrigin.addPoint( PointType::make_point(-1.,1.,1.));
  SLIC_INFO("Testing bounding box: " << bbOrigin2<< " against triangle " << xyTri << ".  Note -- BB should not intersect triangle");
  EXPECT_TRUE( quest::intersect(xyTri, bbOrigin2) );

  BoundingBoxType bbAbove(PointType::ones() );
  bbAbove.addPoint( PointType(2.));
  SLIC_INFO("Testing bounding box: " << bbAbove << " against triangle " << xyTri << ".  Note -- BB should not intersect triangle");
  EXPECT_FALSE( quest::intersect(xyTri, bbAbove) );

  BoundingBoxType bbBelow;
  bbBelow.addPoint( PointType(-1.));
  bbBelow.addPoint( PointType(-2.));
  SLIC_INFO("Testing bounding box: " << bbBelow << " against triangle " << xyTri << ".  Note -- BB should not intersect triangle");
  EXPECT_FALSE( quest::intersect(xyTri, bbBelow) );

  BoundingBoxType bbPoint_OnTri;
  bbPoint_OnTri.addPoint( PointType::make_point(0.,1.,0.));
  SLIC_INFO("Testing point bounding box: " << bbPoint_OnTri << " against triangle " << xyTri << ".  Note -- BB is a point on triangle");
  EXPECT_TRUE( quest::intersect(xyTri, bbPoint_OnTri) );

  BoundingBoxType bbPoint_OutsideTri;
  bbPoint_OutsideTri.addPoint( PointType::make_point(1.,1.,1.));
  SLIC_INFO("Testing point bounding box: " << bbPoint_OutsideTri << " against triangle " << xyTri << ".  Note -- BB is a point outside triangle");
  EXPECT_FALSE( quest::intersect(xyTri, bbPoint_OutsideTri) );

  BoundingBoxType bbInvalid;
  SLIC_INFO("Testing point bounding box: " << bbInvalid << " against triangle " << xyTri << ".  Note -- BB is invalid (empty)");
  EXPECT_FALSE( quest::intersect(xyTri, bbInvalid) );
}



TEST( quest_intersection, triangle_aabb_intersection_fromData )
{
    static int const DIM = 3;
    typedef quest::Point< double,DIM >   PointType;
    typedef quest::Triangle< double,DIM > TriangleType;
    typedef quest::BoundingBox< double,DIM > BoundingBoxType;


    PointType v0 = PointType::make_point(-31.015,63.7756,55.0043);
    PointType v1 = PointType::make_point(-29.0086,59.2982,58.0078);
    PointType v2 = PointType::make_point(-29.2009,70.1039,61.3229);

    TriangleType tri(v0,v1,v2);

    BoundingBoxType box0(PointType::make_point(-39.2793,46.3735,53.3791), PointType::make_point(-26.1692,60.1549,57.0148));
    BoundingBoxType box1(PointType::make_point(-39.2793,60.1549,53.3791), PointType::make_point(-26.1692,73.9362,57.0148));
    BoundingBoxType box2(PointType::make_point(-39.2793,46.3735,57.0148), PointType::make_point(-26.1692,60.1549,60.6506));
    BoundingBoxType box3(PointType::make_point(-39.2793,60.1549,57.0148), PointType::make_point(-26.1692,73.9362,60.6506));
    BoundingBoxType box4(PointType::make_point(-39.2793,46.3735,60.6506), PointType::make_point(-26.1692,60.1549,64.2863));
    BoundingBoxType box5(PointType::make_point(-39.2793,60.1549,60.6506), PointType::make_point(-26.1692,73.9362,64.2863));

    SLIC_INFO("Testing point bounding box: " << box0 << " against triangle " << tri );
    EXPECT_FALSE( quest::intersect(tri, box0 ));

    SLIC_INFO("Testing point bounding box: " << box1 << " against triangle " << tri );
    EXPECT_TRUE( quest::intersect(tri, box1 ));

    //
    asctoolkit::slic::setLoggingMsgLevel( asctoolkit::slic::message::Debug);

    SLIC_INFO("Testing point bounding box: " << box2 << " against triangle " << tri );
    EXPECT_TRUE( quest::intersect(tri, box2 ));

    asctoolkit::slic::setLoggingMsgLevel( asctoolkit::slic::message::Warning);

    SLIC_INFO("Testing point bounding box: " << box3 << " against triangle " << tri );
    EXPECT_TRUE( quest::intersect(tri, box3 ));

    SLIC_INFO("Testing point bounding box: " << box4 << " against triangle " << tri );
    EXPECT_FALSE( quest::intersect(tri, box4 ));

    SLIC_INFO("Testing point bounding box: " << box5 << " against triangle " << tri );
    EXPECT_TRUE( quest::intersect(tri, box5 ));


}

TEST( quest_intersection, triangle_aabb_intersection_fromData2 )
{
    static int const DIM = 3;
    typedef quest::Point< double,DIM >   PointType;
    typedef quest::Triangle< double,DIM > TriangleType;
    typedef quest::BoundingBox< double,DIM > BoundingBoxType;

    // Triangle 569
    TriangleType tri(PointType::make_point(0,5,0), PointType::make_point(-0.665356,4.93844,-0.411212), PointType::make_point(-0.665356,4.93844,0.411212));

    // {pt: (8,15,8); level: 4}
    BoundingBoxType box0(PointType::make_point(0,4.375,0), PointType::make_point(0.625,5,0.625));
    // {pt: (6,15,7); level: 4}
    BoundingBoxType box1(PointType::make_point(-1.25,4.375,-0.625), PointType::make_point(-0.625,5,0));
    // {pt: (6,15,8); level: 4}
    BoundingBoxType box2(PointType::make_point(-1.25,4.375,0), PointType::make_point(-0.625,5,0.625));

    // Block index {pt: (16,31,16); level: 5}
    BoundingBoxType box3(PointType::make_point(0,4.6875,0), PointType::make_point(0.3125,5,0.3125));

    // Block index {pt: (8,15,8); level: 4}
    BoundingBoxType box4(PointType::make_point(0,4.375,0), PointType::make_point(0.625,5,0.625));

    asctoolkit::slic::setLoggingMsgLevel( asctoolkit::slic::message::Info);


    SLIC_INFO("Testing point bounding box: " << box0 << " against triangle " << tri );
    EXPECT_TRUE( quest::intersect(tri, box0));

    SLIC_INFO("Testing point bounding box: " << box1 << " against triangle " << tri );
    EXPECT_TRUE( quest::intersect(tri, box1));

    SLIC_INFO("Testing point bounding box: " << box2 << " against triangle " << tri );
    EXPECT_TRUE( quest::intersect(tri, box2));

    SLIC_INFO("Testing point bounding box: " << box3 << " against triangle " << tri );
    EXPECT_TRUE( quest::intersect(tri, box3));

    SLIC_INFO("Testing point bounding box: " << box4 << " against triangle " << tri );
    EXPECT_TRUE( quest::intersect(tri, box4));

    asctoolkit::slic::setLoggingMsgLevel( asctoolkit::slic::message::Warning);
}

TEST( quest_intersection, triangle_triangle_intersection )
{

  typedef quest::Triangle< double,2 > Triangle2;
  typedef quest::Triangle< double,3 > Triangle3;
  typedef quest::Point< double,2 >   Point2;
  typedef quest::Point< double,3 >   Point3;
  typedef quest::Vector< double,3 >   Vector3;
  // Triangle 569
  Triangle2 tri1(Point2::make_point(0.0,5.0), Point2::make_point(5.0,5.0), Point2::make_point(0.0,0.0));
  Triangle2 tri2(Point2::make_point(0.0,5.0), Point2::make_point(5.0,5.0), Point2::make_point(0.0,0.0));

  asctoolkit::slic::setLoggingMsgLevel( asctoolkit::slic::message::Info);


  SLIC_INFO("Testing two identical 2D triangles: " << tri1 << " and " << tri2 );
  EXPECT_TRUE( quest::intersect(tri1, tri2));


  const Triangle2 tri3(Point2::make_point(-1.0,-1.0), Point2::make_point(-5.0,-5.0), Point2::make_point(-7.0,-8.0));
  SLIC_INFO("Testing two non-intersecting 2D triangles: " << tri1 << " and " << tri2 );
  EXPECT_FALSE( quest::intersect(tri1, tri3));

  //two overlapping 2d triangles:
  const Triangle2 tri4= Triangle2(Point2::make_point(4.3,4.05), Point2::make_point(-1.0,-0.06), Point2::make_point(7.3,-1.3));
  const Triangle2 tri5= Triangle2(Point2::make_point(2.9,1.6), Point2::make_point(-1.5,1.5), Point2::make_point(0.8,5.1));
  SLIC_INFO("Two weird 2d triangles " << tri5 << " and " << tri4 );
  EXPECT_TRUE( quest::intersect(tri4, tri5));

  //two identical 3D traingles
  const Triangle3 tri3d_1(Point3::make_point(-1.0,-1.0,-1.0), Point3::make_point(-2.0,-5.0, -5.0), Point3::make_point(-4.0,-8.0, -8.0));
  const Triangle3 tri3d_2(Point3::make_point(-1.0,-1.0,-1.0), Point3::make_point(-2.0,-5.0, -5.0), Point3::make_point(-4.0,-8.0, -8.0));
  SLIC_INFO("Testing two identical 3d triangles: " << tri3d_1 << " and " << tri3d_2 );
  const bool x= quest::intersect(tri3d_1, tri3d_2);
  EXPECT_TRUE(x);

  //two non-intesecting triangles (trivial case)
  const Triangle3  tri3d_3= Triangle3(Point3::make_point(1.0,1.0,1.0), Point3::make_point(5.0,5.0, 5.0), Point3::make_point(8.0,7.0, 92.0));
  SLIC_INFO("Testing trivial case of 2 non-intersecting triangles: " << tri3d_1 << " and " << tri3d_3 );
  EXPECT_FALSE( quest::intersect(tri3d_1, tri3d_3));

  //two 3D triangles sharing a segment
  const Triangle3 tri3d_4= Triangle3(Point3::make_point(-1.0,-1.0,-1.0), Point3::make_point(-5.0,-6.0, -5.0), Point3::make_point(8.0,8.0, 8.0));
  SLIC_INFO("Testing trivial case of 2 non-intersecting triangles: " << tri3d_1 << " and " << tri3d_4 );
  EXPECT_TRUE( quest::intersect(tri3d_1, tri3d_4));


  //two 3D triangles sharing a vertex
  const Triangle3 tri3d_5= Triangle3(Point3::make_point(-1.0,-1.0,-1.0), Point3::make_point(-2.3,-4.7, -5.4), Point3::make_point(8.0,8.0, 8.0));
  SLIC_INFO("Testing trivial case of 2 non-intersecting triangles: " << tri3d_1 << " and " << tri3d_5 );
  EXPECT_TRUE( quest::intersect(tri3d_1, tri3d_5));


  //future work: test triangle triangle with a bunch of random test cases

  srand (1);  //we want same random number sequence everytime to make sure our tests don't differ on a case to case basis

  double scaleFactor=3.0;
#define RANDOM_NUM(SCALE) 1.0*( (double)rand() / (double)RAND_MAX )


  //Randomly generate a bunch of intersecting triangles (whose intersections form segments) and test them
  for (int i=0; i<5000; i++) {
    //Step 1: Construct a random triangle
    Point3 A= Point3::make_point(RANDOM_NUM(1.0), RANDOM_NUM(1.0), RANDOM_NUM(1.0));
    Point3 B= Point3::make_point(RANDOM_NUM(1.0), RANDOM_NUM(1.0), RANDOM_NUM(1.0));
    Point3 C= Point3::make_point(RANDOM_NUM(1.0), RANDOM_NUM(1.0), RANDOM_NUM(1.0));
    Triangle3 randomTriangle= Triangle3(A,B,C);

    //Step 2: Construct two random points on the triangle.  Rarely, a point is not made correctly, so
    //throw it in a while loop to make sure that both points are on the triangle
    Point3 P= Point3::make_point(-1.0,-1.0,-1.0);
    Point3 Q= Point3::make_point(-1.0,-1.0,-1.0);

    double a1= RANDOM_NUM(1.0);
    double a2= RANDOM_NUM(1.0);
    double a3= RANDOM_NUM(1.0);

    double n1= (a1/(a1+a2+a3));
    double n2= (a2/(a1+a2+a3));
    double n3= (a3/(a1+a2+a3));


    double P_x= n1*A[0]+n2*B[0]+n3*C[0];
    double P_y= n1*A[1]+n2*B[1]+n3*C[1];
    double P_z= n1*A[2]+n2*B[2]+n3*C[2];
    P= Point3::make_point(P_x,P_y,P_z);

    a1= RANDOM_NUM(1.0);
    a2= RANDOM_NUM(1.0);
    a3= RANDOM_NUM(1.0);

    n1= (a1/(a1+a2+a3));
    n2= (a2/(a1+a2+a3));
    n3= (a3/(a1+a2+a3));

    double Q_x= n1*A[0]+n2*B[0]+n3*C[0];
    double Q_y= n1*A[1]+n2*B[1]+n3*C[1];
    double Q_z= n1*A[2]+n2*B[2]+n3*C[2];

    Q= Point3::make_point(Q_x,Q_y,Q_z);


    /*PQ is so random segment on the triangle.  We create a vertex called vertex1 and use
      it to create the triangle formed by P', Q' and vertex1. */

    //Step 3: choose some vertex away from the triangle
    Point3 vertex1 = Point3::make_point(RANDOM_NUM(1.0),RANDOM_NUM(1.0),RANDOM_NUM(1.0));

    //Step 4:
    //we scale the segments formed by both vertex 1 and P and by vertex 1 and Q so that we now
    //have a triangle whose base is not necessarily on the plane formed by ABC
    Vector3 vertex2Direction = Vector3(Q, vertex1);
    Vector3 vertex3Direction = Vector3(P, vertex1);

    double randomScale = RANDOM_NUM(scaleFactor)+2.0;

    //construct the other two vertices of the triangle
    Point3 vertex2 = Point3::make_point(vertex1[0]-2*vertex2Direction[0], vertex1[1]-2*vertex2Direction[1],
					vertex1[2]-2*vertex2Direction[2]);
    Point3 vertex3 = Point3::make_point(vertex1[0]-2*vertex3Direction[0], vertex1[1]-2*vertex3Direction[1],
					vertex1[2]-2*vertex3Direction[2]);

    Triangle3 before = Triangle3(vertex1, Q, P);
    Triangle3 intersectingTriangle= Triangle3(vertex1, vertex2, vertex3);
    //Step 5: run our intersection test as long as the generated triangles are not degenerate
    bool test= true;
    //this code should eventually all be consolidated into the do while loop, especially because I am
    //because of the lines below.
    if (!(randomTriangle.degenerate() || intersectingTriangle.degenerate())) {
      SLIC_INFO("\n\n\n Triangles are not degenerate... testing "<< randomTriangle<<intersectingTriangle);
      test=quest::intersect(randomTriangle, intersectingTriangle);
    }
    char coords[3]={'x','y','z'};
    // if (!test) {

    //   std::ofstream failure; //still don't know where this is writing too...
    //   failure.open("failure.txt",std::fstream::app);

    //   for (int j=0; j<3; j++) {
    // 	failure<<coords[j]<<"1 = ["<<randomTriangle.A()[j] << ","<< randomTriangle.B()[j]<< ","<<
    // 	  randomTriangle.C()[j] <<"]\n";
    //   }
    //   for (int j=0; j<3; j++) {
    // 	failure<<coords[j]<<"2 = ["<<intersectingTriangle.A()[j] << ","<< intersectingTriangle.B()[j]
    // 	       << ","<< intersectingTriangle.C()[j] <<"]\n";
    //   }
    //   failure.close();
    // }


    //SLIC_INFO("Testing randomly generated traingle: " << randomTriangle << " against triangle " << 
    //intersectingTriangle );
  EXPECT_TRUE( test);
  if (!test)  SLIC_INFO("Testing randomly generated traingle failed: " <<
			randomTriangle << " against triangle " << intersectingTriangle );

}

asctoolkit::slic::setLoggingMsgLevel( asctoolkit::slic::message::Warning);
}


TEST( quest_intersection, triangle_aabb_intersection_boundaryFace )
{
  static int const DIM = 3;
  typedef quest::Point< double,DIM >   PointType;
  typedef quest::Triangle< double,DIM > TriangleType;
  typedef quest::BoundingBox< double,DIM > BoundingBoxType;

  TriangleType tri(PointType::make_point(0,5,0), PointType::make_point(0,5,5), PointType::make_point(0,5,5));

  BoundingBoxType box0(PointType::make_point(-10,-10,-10), PointType::make_point(0,10,10));
  BoundingBoxType box1(PointType::make_point(0,-10,-10), PointType::make_point(10,10,10));

  asctoolkit::slic::setLoggingMsgLevel( asctoolkit::slic::message::Debug);


  SLIC_INFO("Testing point bounding box: " << box0 << " against triangle " << tri );
  EXPECT_TRUE( quest::intersect(tri, box0));

  SLIC_INFO("Testing point bounding box: " << box1 << " against triangle " << tri );
  EXPECT_TRUE( quest::intersect(tri, box1));

  // ---

  // Airfoil triangle 206
  TriangleType tri2(PointType::make_point(0.0340691,-1,0.0236411)
                    , PointType::make_point(0.028589,-1,0.0221062)
                    , PointType::make_point(0.0207793,-1,-0.0295674));
  // Block: (134,128,310) @ level 9
  BoundingBoxType box2( PointType::make_point(0.0230077,-1,-0.0208459)
                        , PointType::make_point(0.0268708,-0.992188,-0.0201394));

  SLIC_INFO("Testing point bounding box: " << box2
	    << " against triangle " << tri2
	    << "\n\t -- intersects? " << (quest::intersect(tri2, box2) ? "yes":"no")
	    //<< "\n\t -- distance: " << (quest::distance(tri2, box2) ? "yes":"no")
    );
  //EXPECT_TRUE( quest::intersect(tri, box1));

  asctoolkit::slic::setLoggingMsgLevel( asctoolkit::slic::message::Warning);
}


TEST( quest_intersection, ray_aabb_intersection_general3D )
{
  static int const DIM = 3;
  typedef quest::Point< double, DIM >   PointType;
  typedef quest::Ray< double,DIM > RayType;
  typedef quest::BoundingBox< double, DIM > BoundingBoxType;
  typedef quest::Vector< double, DIM >  VectorType;


  // STEP 1: construct ray
  PointType origin = PointType::make_point( 0.0,0.0,0.0 );
  VectorType direction;
  direction[0] = 1.0;
  direction[1] = 1.0;
  direction[2] = 1.0;
  RayType R( origin,direction.unitVector() );


  BoundingBoxType box0(PointType::make_point(5.0,5.0,5.0), PointType::make_point(10.0,10.0,10.0));
  BoundingBoxType box1(PointType::make_point(-5.0,-5.0,-5.0), PointType::make_point(-1.0,-1.0,-1.0));

  asctoolkit::slic::setLoggingMsgLevel( asctoolkit::slic::message::Debug);
  PointType ip;

  bool intersects = quest::intersect(R, box0, ip);
  SLIC_INFO("Testing point bounding box: " << box0 << " against ray ");
  SLIC_INFO("Point at: "<<ip);
  EXPECT_TRUE( intersects);

  intersects = quest::intersect(R, box1, ip);
  SLIC_INFO("Testing point bounding box: " << box1 << " against ray ");
  SLIC_INFO("Point at: "<<ip);
  EXPECT_TRUE( !(intersects));


  //asctoolkit::slic::setLoggingMsgLevel( asctoolkit::slic::message::Warning);
}


TEST( quest_intersection, ray_aabb_intersection_tinyDirectionVector3D )
{
    static int const DIM = 3;
    typedef quest::Point< double, DIM >   PointType;
    typedef quest::Ray< double,DIM > RayType;
    typedef quest::BoundingBox< double, DIM > BoundingBoxType;
    typedef quest::Vector< double, DIM >  VectorType;


    // STEP 1: construct ray
    PointType origin = PointType::make_point( 11.0,11.0,11.0 );
    VectorType direction;
    direction[0] = 0.0;
    direction[1] = 0.0;
    direction[2] = 0.0;
    RayType R( origin,direction.unitVector() );


    BoundingBoxType box0(PointType::make_point(5.0,5.0,5.0), PointType::make_point(10.0,10.0,10.0));
    BoundingBoxType box1(PointType::make_point(-5.0,-5.0,-5.0), PointType::make_point(-1.0,-1.0,-1.0));

    asctoolkit::slic::setLoggingMsgLevel( asctoolkit::slic::message::Debug);
    PointType ip;

    bool intersects = quest::intersect(R, box0, ip);
    SLIC_INFO("Testing point bounding box: " << box0 << " against ray ");
    SLIC_INFO("Point at: "<<ip);
    EXPECT_FALSE(intersects);

    intersects = quest::intersect(R, box1, ip);
    SLIC_INFO("Testing point bounding box: " << box1 << " against ray ");
    SLIC_INFO("Point at: "<<ip);
    EXPECT_TRUE( !(intersects));


    //asctoolkit::slic::setLoggingMsgLevel( asctoolkit::slic::message::Warning);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
#include "slic/UnitTestLogger.hpp"
using asctoolkit::slic::UnitTestLogger;

int main(int argc, char * argv[])
{
  int result = 0;

  ::testing::InitGoogleTest(&argc, argv);

  UnitTestLogger logger;  // create & initialize test logger,

  asctoolkit::slic::setLoggingMsgLevel( asctoolkit::slic::message::Warning);

  // finalized when exiting main scope

  result = RUN_ALL_TESTS();

  return result;
}
