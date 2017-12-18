#include "DynamicLoadConfiguration.h"

#include "StdFElement.h"

using namespace std;
using namespace Geometry;
using namespace FiniteElements;

TEST_F (LoadTest, IsRegistered)
{
	clog << endl << "Starting IsRegistered" << endl;

	auto& quad_fac (QuadratureFactory<1>::Instance());
	auto quad_ptr = quad_fac.create (IntervalType);
	EXPECT_NE (quad_ptr.get(), nullptr);

	clog << "IsRegisterd ended" << endl << endl;
};


TEST_F (LoadTest, StdIntegration)
{
	clog << endl << "Starting StdIntegration" << endl;

	//interval (-1,1)
	StdIperCube<1> std_interval;

	clog << "I check the simmetry of the quadrature nodes" << endl;
	auto points = std_interval.GetQuadPoints();

	size_t L = points.Size();
	for (size_t i = 0; i < L; ++i)
	{
		auto point = points[i];
		auto r_point = points[L - i - 1];

		double err = point + r_point;

		string s1 = "Point "
					+ to_string (point)
					+ " simmetry: error = "
					+ to_string (err);
		string s2 = "The point "
					+ to_string (point)
					+ " is not symmetric: error = "
					+ to_string (err);
		clog << s1 << endl;
		EXPECT_LT (err, 1E-4) << s2;
	}

	clog << "I check the simmetry of the quadrature weights" << endl;
	auto weights = std_interval.GetQuadWeights();
	size_t j = weights.Size() - 1;
	for (size_t i = 0; i < weights.Size(); ++i)
	{
		auto err = weights[i] - weights[j];

		string s1 = "Weight #"
					+ to_string (i)
					+ " valued "
					+ to_string (weights[i])
					+ " simmetry: error = "
					+ to_string (err);
		string s2 = "The weight #"
					+ to_string (i)
					+ " valued "
					+ to_string (weights[i])
					+ " is not symmetric: error = "
					+ to_string (err);
		clog << s1 << endl;
		EXPECT_LT (err, 1E-4) << s2;

		--j;
	}


	size_t order = std_interval.QuadratureOrder();
	clog << "I compute the integral in (-1,1) of x^" << order <<
		 ", the highest power exactly integrated" << endl;
	double val = std_interval.Integrate ([&order]						//*NOPAD*
										 (const Point<1>& x)			//*NOPAD*
										 {return pow (x[0], order);});	//*NOPAD*
	double solex = order % 2 ? 0 : 2;
	solex /= (order + 1);
	double err = abs (val - solex);

	string s = "Integrating x^"
			   + to_string (order)
			   + " in (-1,1)" + "\n"
			   + "Exact solution: "
			   + to_string (solex) + "\n"
			   + "Computed solution: "
			   + to_string (val);
	clog << s << endl;
	EXPECT_LT (err, 1E-14) << s;

	--order;
	clog << "I compute the integral in (-1,1) of x^" << order << endl;
	val = std_interval.Integrate ( [&order]								//*NOPAD*
								   (const Point<1>& x)					//*NOPAD*
								   {return pow (x[0], order);});		//*NOPAD*
	solex = order % 2 ? 0 : 2;
	solex /= (order + 1);
	err = abs (val - solex);

	s	= "Integrating x^"
		  + to_string (order)
		  + " in (-1,1)" + "\n"
		  + "Exact solution: "
		  + to_string (solex) + "\n"
		  + "Computed solution: "
		  + to_string (val);
	clog << s << endl;
	EXPECT_LT (err, 1E-14) << s;


	//	clog << "Calcolo l'integrale tra -1 e 1 di 1E+30 * x^99" << endl;
	//	double val5 = std_interval.Integrate([](const Point<1>& x){return pow(10.0, 30) * pow(x[0], 99);});
	//	double solex5 = 0;
	//	clog << "Soluzione esatta : " << solex5 << endl;
	//	clog << "Soluzione numerica : " << val5 << endl;
	//	clog << "errore : " << abs(val5 - solex5) << endl;


	//standard square
	StdIperCube<2> std_square;

	//I check that the 2d points are the tensorization of the 1d ones
	//	auto one_d_points = std_interval.GetQuadPoints();
	//	auto one_d_weights = std_interval.GetQuadWeights();
	//	auto two_d_points = std_square.GetQuadPoints();
	//	auto two_d_weights = std_square.GetQuadWeights();

	//	clog << "BANANA: ";
	//	clog << one_d_points.size() << endl;
	//	for (auto i = 0; i < one_d_points.size(); ++i)
	//		clog << one_d_points[i] << "	";
	//	clog << endl;

	//	clog << "BANANA: ";
	//	clog << two_d_points.size() << endl;
	//	for (auto i = 0; i < two_d_points.size(); ++i)
	//		clog << two_d_points[i] << "	";
	//	clog << endl;

	//	clog << "BANANA: ";
	//	clog << one_d_weights.size() << endl;
	//	for (auto i = 0; i < one_d_weights.size(); ++i)
	//		clog << one_d_weights[i] << "	";
	//	clog << endl;

	//	clog << "BANANA: ";
	//	clog << two_d_weights.size() << endl;
	//	for (auto i = 0; i < two_d_weights.size(); ++i)
	//		clog << two_d_weights[i] << "	";
	//	clog << endl;

	order = std_square.QuadratureOrder();
	clog << "I compute the integral in (-1,1)x(-1,1) of x^"
		 << order
		 << " + y^"
		 << order << endl;

	val = std_square.Integrate ( [&order]											//*NOPAD*
								 (const Point<2>& x)								//*NOPAD*
								 {return pow (x[0], order) + pow (x[1], order);});	//*NOPAD*
	solex = order % 2 ? 0 : 2;
	solex /= (order + 1);
	solex *= 4;
	err = abs (val - solex);

	s = "Integrating x^"
	  + to_string (order)
	  + " + y^"
	  + to_string (order)
	  + " in (-1,1)x(-1,1)" + "\n"
	  + "Exact solution: "
	  + to_string (solex) + "\n"
	  + "Computed solution: "
	  + to_string (val) + "\n";

	clog << s << endl;
	EXPECT_LT (err, 1E-14) << s;

	--order;
	clog << "I compute the integral in (-1,1)x(-1,1) of x^" << order << " + y^" <<
		 order << endl;
	val = std_square.Integrate ( [&order]											//*NOPAD*
								 (const Point<2>& x)								//*NOPAD*
								 {return pow (x[0], order) + pow (x[1], order);});	//*NOPAD*
	solex = order % 2 ? 0 : 2;
	solex /= (order + 1);
	solex *= 4;
	err = abs (val - solex);

	s = "Integrating x^"
	  + to_string (order)
	  + " + y^"
	  + to_string (order)
	  + " in (-1,1)x(-1,1)" + "\n"
	  + "Exact solution: "
	  + to_string (solex) + "\n"
	  + "Computed solution: "
	  + to_string (val) + "\n";
	clog << s << endl;
	EXPECT_LT (err, 1E-14) << s;

	++order;
	clog << "I compute the integral in (-1,1)x(-1,1) of x^" << order << " * y^" <<
		 order << endl;
	val = std_square.Integrate ( [&order]											//*NOPAD*
								 (const Point<2>& x)								//*NOPAD*
								 {return pow (x[0], order) * pow (x[1], order);});	//*NOPAD*
	solex = order % 2 ? 0 : 2;
	solex /= (order + 1);
	solex *= solex;
	err = abs (val - solex);

	s = "Integrating x^"
	  + to_string (order)
	  + " * y^"
	  + to_string (order)
	  + " in (-1,1)x(-1,1)" + "\n"
	  + "Exact solution: "
	  + to_string (solex) + "\n"
	  + "Computed solution: "
	  + to_string (val) + "\n";

	clog << s << endl;
	EXPECT_LT (err, 1E-14) << s;


	StdElement<2, TriangleType> std_triangle;

	order = std_triangle.QuadratureOrder();
	clog << "I compute the integral on the standard triangle = co{(-1,-1),(1,-1),(-1,1)} of x^"
		 << order << " + y^" << order << endl;
	val = std_square.Integrate ( [&order]											//*NOPAD*
								 (const Point<2>& x)								//*NOPAD*
								 {return pow (x[0], order) + pow (x[1], order);});	//*NOPAD*
	solex = 2 / ((order + 1) * (order + 2));
	err = abs (val - solex);

	s = "Integrating x^"
	  + to_string (order)
	  + " + y^"
	  + to_string (order)
	  + " in co{(-1,-1),(1,-1),(-1,1)}" + "\n"
	  + "Exact solution: "
	  + to_string (solex) + "\n"
	  + "Computed solution: "
	  + to_string (val) + "\n";

	clog << s << endl;
	EXPECT_LT (err, 1E-14) << s;


	//TODO : check other integrals
	clog << "StdIntegration ended" << endl << endl;
};

TEST_F (LoadTest, IntervalLegendreOrthonormality)
{
	clog << endl << "Starting IntervalLegendreOrthonormality" << endl;

	clog << "Constructing the standard interval" << endl;

	StdFIperCube<1, LegendreType> sfic;
	size_t order = sfic.QuadratureOrder();

	clog << "Checking the orthogonality on the standard interval" << endl;
	for (size_t k (0); k < order; ++k)
		for (size_t i = k + 1; i + k <= order; ++i)
		{
			double I = sfic.L2Prod	(
				[&] (const Point<1>& p) { return sfic.EvaluateBasisFunction (i, p);},
				[&] (const Point<1>& p) { return sfic.EvaluateBasisFunction (k, p);});

			string s1 	= "Legendre functions #"
						+ to_string (i)
						+ " and #"
						+ to_string (k)
						+ " L2 product = "
						+ to_string (I);
			string s2 	= "Legendre functions #"
						+ to_string (i)
						+ " and #"
						+ to_string (k)
						+ " are not orthogonal: L2 product = "
						+ to_string (I);
			clog << s1 << endl;
			EXPECT_LT (I, 1E-10) << s2;
		}


	clog << endl << endl
		 << "Checking the normality on the standard interval :"
		 << endl;

	//I call InitNorms so I compute it ones here.
	//During the for cicles the norms vector on the std element will not be modified
	StdFElementInterface<1, LegendreType>::InitNorms (order / 2);

	//order is a size_t, so when divided by two it's truncated downward to the last value
	//for which the exactness of the quadrature rule for the L2 norm of the function still holds
	for (size_t i = 0; i < order / 2; ++i)
	{
		double I = sfic.BasisNormSquared (i);
		I *= static_cast<double> (i) + 0.5;
		auto err = abs (I - 1);

		string s1 	= "Legendre function #"
					+ to_string (i)
					+ " norm = "
					+ to_string (I);
		string s2 	= "Legendre function #"
					+ to_string (i)
					+ " is not normal: Integral = "
					+ to_string (I);
		clog << s1 << endl;
		EXPECT_LT (err, 1E-10) << s2;
	}
	//TODO: add automated checks on values not exactly integrated

	clog << "IntervalLegendreOrthonormality ended" << endl << endl;

};

TEST_F (LoadTest, SquareLegendreOrthonormality)
{
	clog << endl << "Starting SquareLegendreOrthonormality" << endl;

	clog << "Constructing the standard square" << endl;

	StdFIperCube<2, LegendreType> std_square;
	auto order = std_square.QuadratureOrder();
	auto exactness_size = std_square.BasisSize (order);

	clog << "Checking the orthogonality on the standard square" << endl;

	size_t iteration_end = (exactness_size < 50) ? exactness_size : 50;
	for (size_t k (0); k < iteration_end; ++k)
		for (size_t i = k + 1; i + k <= iteration_end; ++i)
		{
			double I = std_square.L2Prod (
				[&] (const Point<2>& p) { return std_square.EvaluateBasisFunction (i, p);},
				[&] (const Point<2>& p) { return std_square.EvaluateBasisFunction (k, p);});

			string s1 	= "Legendre functions #"
						+ to_string (i)
						+ " and #"
						+ to_string (k)
						+ " L2 product = "
						+ to_string (I);
			string s2 	= "Legendre functions #"
						+ to_string (i)
						+ " and #"
						+ to_string (k)
						+ " are not orthogonal: L2 product = "
						+ to_string (I);
			clog << s1 << endl;
			EXPECT_LT (I, 1E-10) << s2;
		}

	clog << endl << endl
		 << "Checking the normality on the standard square :" << endl;

	size_t k1 = 0, k2 = 0;
	size_t deg = 0;
	//order is a size_t, so when divided by two it's truncated downward to the last value
	//for which the exactness of the quadrature rule for the L2 norm of the function still holds
	size_t normality_size = std_square.BasisSize (order / 2);

	iteration_end = normality_size;

	//I call InitNorms so I compute it ones here.
	//During the for cicles the norms vector on the std element will not be modified
	StdFElementInterface<2, LegendreType>::InitNorms (order / 2);

	for (size_t i = 0; i < iteration_end; ++i)
	{
		double I = std_square.BasisNormSquared (i);

		double K1 = (static_cast<double> (k1) + 0.5);
		double K2 = (static_cast<double> (k2) + 0.5);

		I *= K1 * K2;
		auto err = abs (I - 1);

		string s1 	= "Legendre function #"
					+ to_string (i)
					+ " norm = "
					+ to_string (I);
		string s2 	= "Legendre function #"
					+ to_string (i)
					+ " is not normal: Integral = "
					+ to_string (I);
		clog << s1 << endl;
		EXPECT_LT (err, 1E-10) << s2;

		(k2 == deg) ? (k2 = 0, k1 = ++deg) : (--k1, ++k2);
	}
	//TODO: add automated checks on values not exactly integrated

	clog << "SquareLegendreOrthonormality ended" << endl << endl;

};

TEST_F (LoadTest, WarpedOrthogonality)
{
	clog << endl << "WarpedOrthogonality" << endl;

	clog << "Constructing the standard triangle" << endl;
	StdFElement<2, TriangleType, WarpedType> std_triangle;
	auto order = std_triangle.QuadratureOrder();
	auto exactness_size = std_triangle.BasisSize (order);

	clog << "Checking the orthogonality on the standard triangle" << endl;

	size_t iteration_end = (exactness_size < 50) ? exactness_size : 50;
	for (size_t k = 0; k < iteration_end; ++k)
		for (size_t i = k + 1; i + k <= iteration_end; ++i)
		{
			double I = std_triangle.L2Prod (
				[&] (const Point<2>& p) { return std_triangle.EvaluateBasisFunction (i, p);},
				[&] (const Point<2>& p) { return std_triangle.EvaluateBasisFunction (k, p);});

			string s1 	= "Warped functions #"
						+ to_string (i)
						+ " and #"
						+ to_string (k)
						+ " L2 product = "
						+ to_string (I);
			string s2	 = "Warped functions #"
						+ to_string (i)
						+ " and #"
						+ to_string (k)
						+ " are not orthogonal: L2 product = "
						+ to_string (I);
			clog << s1 << endl;
			EXPECT_LT (I, 1E-10) << s2;
		}

	//TODO: add automated checks on values not exactly integrated

	clog << "WarpedOrthogonality ended" << endl << endl;
};

