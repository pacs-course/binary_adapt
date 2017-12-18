#include "BasicConfiguration.h"

#include "Maps.h"

using namespace std;
using namespace Geometry;

TEST_F (BasicTest, PointTest)
{
	clog << endl << "Starting PointTest" << endl;

	Point<1> p_test ({1});
	EXPECT_EQ (p_test[0], 1) << "1d construction by initializer list failed";
	double p = p_test;
	EXPECT_EQ (p, 1) << "1d cast to double failed";

	Point<1> p_copy (p_test);
	Point<1> p1 = p_copy;

	EXPECT_EQ (p1[0], 1) << "Assignment operator failed";

	Point<2> p2 ({2, 3});
	EXPECT_EQ (p2[0], 2) << "2d construction by initializer list failed";
	EXPECT_EQ (p2[1], 3) << "2d construction by initializer list failed";

	Point<3> p3 = p1.Tensor (p2);
	EXPECT_EQ (p3[0], 1) << "Tensor product failed";
	EXPECT_EQ (p3[1], 2) << "Tensor product failed";
	EXPECT_EQ (p3[2], 3) << "Tensor product failed";

	Point<3> p4 = p3 ;
	EXPECT_EQ (p4[0], 1) << "Product by scalar failed";
	EXPECT_EQ (p4[1], 2) << "Product by scalar failed";
	EXPECT_EQ (p4[2], 3) << "Product by scalar failed";

	double scalar (2);
	p4 *= scalar;
	EXPECT_EQ (p4[0], 2) << "Product by scalar failed";
	EXPECT_EQ (p4[1], 4) << "Product by scalar failed";
	EXPECT_EQ (p4[2], 6) << "Product by scalar failed";

	Point<3> p5 = p4 - p3;

	auto diff = abs (p5.Abs() - sqrt (14));
	EXPECT_LT (diff, 1E-4) << "Vector norm failed";

	auto dist_diff = abs (p5.distance (p4) - sqrt (14));
	EXPECT_LT (dist_diff, 1E-4) << "Vector distance failed";
	//TODO: test other member functions

	clog << "PointTest ended" << endl << endl;
};

TEST_F (BasicTest, MapsTest)
{
	clog << endl << "Starting MapsTest" << endl;

	clog << "I create the map from (-1,1) to (1,2)" << endl;

	NodesVector<1> interval (2);

	Point<1> left = 1;
	interval.Insert (0, left);
	Point<1> right = 2;
	interval.Insert (1, right);

	IntervalMap map1;
	map1.Init (interval);
	clog << "Map created" << endl;

	Point<1> p1 ({-1});
	Point<1> p2 ({0});
	Point<1> p3 ({1});

	Point<1> mapped_p1 = map1.Evaluate (p1);
	auto err = abs (mapped_p1[0] - 1);
	EXPECT_LT (err, 1E-4) << "Point -1 expected to be mapped in 1, is mapped in "
						  << mapped_p1[0];

	Point<1> mapped_p2 = map1.Evaluate (p2);
	err = abs (mapped_p2[0] - 1.5);
	EXPECT_LT (err, 1E-4) << "Point 0 expected to be mapped in 1.5, is mapped in "
						  << mapped_p2[0];

	Point<1> mapped_p3 = map1.Evaluate (p3);
	err = abs (mapped_p3[0] - 2);
	EXPECT_LT (err, 1E-4) << "Point 1 expected to be mapped in 2, is mapped in "
						  << mapped_p3[0];


	clog << "I test the inverse map" << endl;

	Point<1> inverse_p1 = map1.ComputeInverse (mapped_p1);
	err = abs (p1[0] - inverse_p1[0]);
	EXPECT_LT (err, 1E-4) << "Point 1 expected to be inverse mapped in -1, is mapped in "
						  << inverse_p1[0];

	Point<1> inverse_p2 = map1.ComputeInverse (mapped_p2);
	err = abs (p2[0] - inverse_p2[0]);
	EXPECT_LT (err, 1E-4) << "Point 1.5 expected to be inverse mapped in 0, is mapped in "
						  << inverse_p2[0];

	Point<1> inverse_p3 = map1.ComputeInverse (mapped_p3);
	err = abs (p3[0] - inverse_p3[0]);
	EXPECT_LT (err, 1E-4) << "Point 2 expected to be inverse mapped in 1, is mapped in "
						  << inverse_p3[0];

	clog << "2D test" << endl << endl;

	clog << "I create the triangular map between co{(1,1),(2,1),(2,2)}"
		 << " and the reference triangle, which is co{(-1,-1),(1,-1),(-1,1)}"
		 << endl;
	NodesVector<2> triangle (3);

	Point<2> bottom_left ({1, 1});
	triangle.Insert (0, bottom_left);
	Point<2> bottom_right ({2, 1});
	triangle.Insert (1, bottom_right);
	Point<2> top ({2, 2});
	triangle.Insert (2, top);

	TriMap map2;
	map2.Init (triangle);
	clog << "Map created" << endl;


	Point<2> p4 ({-1, -1});
	Point<2> p5 ({1, -1});
	Point<2> p6 ({ (-1.0 / 3), (-1.0 / 3)});
	Point<2> p7 ({-1, 1});

	Point<2> mapped_p4 = map2.Evaluate (p4);
	err = abs (mapped_p4[0] - 1 + mapped_p4[1] - 1);
	EXPECT_LT (err, 1E-4) << "Point (-1,-1) expected to be mapped in (1,1), is mapped in ("
						  << mapped_p4[0]
						  << ","
						  << mapped_p4[1]
						  << ")";

	Point<2> mapped_p5 = map2.Evaluate (p5);
	err = abs (mapped_p5[0] - 2 + mapped_p5[1] - 1);
	EXPECT_LT (err, 1E-4) << "Point (1,-1) expected to be mapped in (2,1), is mapped in ("
						  << mapped_p5[0]
						  << ","
						  << mapped_p5[1]
						  << ")";

	Point<2> mapped_p6 = map2.Evaluate (p6);
	err = abs (mapped_p6[0] - (5.0 / 3) + mapped_p6[1] - (4.0 / 3));
	EXPECT_LT (err, 1E-4) << "Point (-1/3,-1/3) expected to be mapped in (5/3,4/3), is mapped in ("
						  << mapped_p6[0]
						  << ","
						  << mapped_p6[1]
						  << ")";

	Point<2> mapped_p7 = map2.Evaluate (p7);
	err = abs (mapped_p7[0] - 2 + mapped_p7[1] - 2);
	EXPECT_LT (err, 1E-4) << "Point (1,-1) expected to be mapped in (2,2), is mapped in ("
						  << mapped_p7[0]
						  << ","
						  << mapped_p7[1]
						  << ")";


	clog << "I test the inverse map" << endl;

	Point<2> inverse_p4 = map2.ComputeInverse (mapped_p4);
	err = (p4 - inverse_p4).Abs();
	EXPECT_LT (err, 1E-4) << "Point (1,1) expected to be inverse mapped in (-1,-1), is mapped in ("
						  << inverse_p4[0]
						  << ","
						  << inverse_p4[1]
						  << ")";

	Point<2> inverse_p5 = map2.ComputeInverse (mapped_p5);
	err = (p5 - inverse_p5).Abs();
	EXPECT_LT (err, 1E-4) << "Point (2,1) expected to be inverse mapped in (1,-1), is mapped in ("
						  << inverse_p5[0]
						  << ","
						  << inverse_p5[1]
						  << ")";

	Point<2> inverse_p6 = map2.ComputeInverse (mapped_p6);
	err = (p6 - inverse_p6).Abs();
	EXPECT_LT (err, 1E-4) << "Point (5/3,4/3) expected to be inverse mapped in (-1/3,-1/3), is mapped in ("
						  << inverse_p6[0]
						  << ","
						  << inverse_p6[1]
						  << ")";

	Point<2> inverse_p7 = map2.ComputeInverse (mapped_p7);
	err = (p7 - inverse_p7).Abs();
	EXPECT_LT (err, 1E-4) << "Point (2,2) expected to be inverse mapped in (-1,1), is mapped in ("
						  << inverse_p7[0]
						  << ","
						  << inverse_p7[1]
						  << ")";

	clog << "I create the triangular map between co{(1.5,1.5),(2,1),(2,2)} "
		 << "and the reference triangle, which is co{(-1,-1),(1,-1),(-1,1)}"
		 << endl;
	NodesVector<2> triangle2 (3);

	Point<2> bottom_left2 ({1.5, 1.5});
	triangle2.Insert (0, bottom_left2);
	Point<2> bottom_right2 ({2, 1});
	triangle2.Insert (1, bottom_right2);
	Point<2> top2 ({2, 2});
	triangle2.Insert (2, top2);

	TriMap trimap2;
	trimap2.Init (triangle2);
	clog << "Map created" << endl;


	mapped_p4 = trimap2.Evaluate (p4);
	err = abs (mapped_p4[0] - 1.5 + mapped_p4[1] - 1.5);
	EXPECT_LT (err, 1E-4) << "Point (-1,-1) expected to be mapped in (1.5,1.5), is mapped in ("
						  << mapped_p4[0]
						  << ","
						  << mapped_p4[1]
						  << ")";

	mapped_p5 = trimap2.Evaluate (p5);
	err = abs (mapped_p5[0] - 2 + mapped_p5[1] - 1);
	EXPECT_LT (err, 1E-4) << "Point (1,-1) expected to be mapped in (2,1), is mapped in ("
						  << mapped_p5[0]
						  << ","
						  << mapped_p5[1]
						  << ")";

	mapped_p7 = trimap2.Evaluate (p7);
	err = abs (mapped_p7[0] - 2 + mapped_p7[1] - 2);
	EXPECT_LT (err, 1E-4) << "Point (1,-1) expected to be mapped in (2,2), is mapped in ("
						  << mapped_p7[0]
						  << ","
						  << mapped_p7[1]
						  << ")";


	clog << "I test the map between the std square, "
		 << "which is co{(-1,-1),(1,-1),(-1,1),(1,1)} and the reference triangle, "
		 << "which is co{(-1,-1),(1,-1),(-1,1)}"
		 << endl;
	StdTriMap map3;
	Point<2> q_bottom_left ({-1, -1});
	Point<2> q_bottom_right ({1, -1});
	Point<2> q_top_left ({-1, 1});
	Point<2> q_top_right ({1, 1});

	Point<2> t_bottom_left ({-1, -1});
	Point<2> t_bottom_right ({1, -1});
	Point<2> t_top ({-1, 1});

	Point<2> mapped_point;

	mapped_point = map3.Evaluate (q_bottom_left);
	err = (mapped_point - t_bottom_left).Abs();
	EXPECT_LT (err, 1E-4) << "Point (-1,-1) expected to be mapped in (-1,-1), is mapped in ("
						  << mapped_point[0]
						  << ","
						  << mapped_point[1]
						  << ")";

	mapped_point = map3.Evaluate (q_bottom_right);
	err = (mapped_point - t_bottom_right).Abs();
	EXPECT_LT (err, 1E-4) << "Point (1,-1) expected to be mapped in (1,-1), is mapped in ("
						  << mapped_point[0]
						  << ","
						  << mapped_point[1]
						  << ")";

	mapped_point = map3.Evaluate (q_top_left);
	err = (mapped_point - t_top).Abs();
	EXPECT_LT (err, 1E-4) << "Point (-1,1) expected to be mapped in (-1,1), is mapped in ("
						  << mapped_point[0]
						  << ","
						  << mapped_point[1]
						  << ")";

	mapped_point = map3.Evaluate (q_top_right);
	err = (mapped_point - t_top).Abs();
	EXPECT_LT (err, 1E-4) << "Point (1,1) expected to be mapped in (-1,1), is mapped in ("
						  << mapped_point[0]
						  << ","
						  << mapped_point[1]
						  << ")";

	clog << "I test the inverse map" << endl;

	mapped_point = map3.ComputeInverse (t_bottom_left);
	err = (mapped_point - q_bottom_left).Abs();
	EXPECT_LT (err, 1E-4) << "Point (-1,-1) expected to be inverse mapped in (-1,-1), is mapped in ("
						  << mapped_point[0]
						  << ","
						  << mapped_point[1]
						  << ")";

	mapped_point = map3.ComputeInverse (t_bottom_right);
	err = (mapped_point - q_bottom_right).Abs();
	EXPECT_LT (err, 1E-4) << "Point (1,-1) expected to be inverse mapped in (1,-1), is mapped in ("
						  << mapped_point[0]
						  << ","
						  << mapped_point[1]
						  << ")";

	mapped_point = map3.ComputeInverse (t_top);
	err = abs (mapped_point[1] - t_top[1]);
	EXPECT_TRUE (std::isnan (mapped_point[0]) &&
				 (err < 1E-4)) << "Point (-1,1) expected to be inverse mapped in (nan,1), is mapped in ("
							   << mapped_point[0]
							   << ","
							   << mapped_point[1]
							   << ")";

	clog << "MapsTest ended" << endl << endl;
};


