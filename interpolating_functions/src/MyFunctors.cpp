#include "MyFunctors.h"

#include <math.h>

using namespace std;

namespace MyFunctions
{
	double SqrtX::operator() (const Geometry::Point<1>& p) const
	{
		return sqrt(p);
	};
	string SqrtX::Formula()const
	{
		return "sqrt(x)";
	};
	string SqrtX::ID()const
	{
		return "sqrt_x";
	};

	double HalfStep::operator() (const Geometry::Point<1>& p) const
	{
		double val = p;
		if (val > 0.5)
			return 1.0;
		return 0.0;
	};
	string HalfStep::Formula()const
	{
		return "(x > 0.5)";
	};
	string HalfStep::ID()const
	{
		return "half_0";
	};

	double HalfX::operator() (const Geometry::Point<1>& p) const
	{
		double val = p;
		if (val >= 0.5)
			return val;
		
		return 0.0;
	};
	string HalfX::Formula()const
	{
		return "x * (x > 0.5)";
	};
	string HalfX::ID()const
	{
		return "half_1";
	};

	double HalfSquare::operator() (const Geometry::Point<1>& p) const
	{
		double val = p;
		if (val > 0.5)
			return val*val;
		
		return 0.0;
	};
	string HalfSquare::Formula()const
	{
		return "x^2 * (x > 0.5)";
	};
	string HalfSquare::ID()const
	{
		return "half_2";
	};

	double HalfTwenty::operator() (const Geometry::Point<1>& p) const
	{
		double val = p;
		if (val >= 0.5)
			return Helpers::Power<20>(val);
		
		return 0.0;
	};
	string HalfTwenty::Formula()const
	{
		return "x^20 * (x > 0.5)";
	};
	string HalfTwenty::ID()const
	{
		return "half_20";
	};

	double HalfSqrt::operator() (const Geometry::Point<1>& p) const
	{
		double val = p;
		if (val > 0.5)
			return sqrt(val);
		return 0.0;
	};
	string HalfSqrt::Formula()const
	{
		return "sqrt(x) * (x > 0.5)";
	};
	string HalfSqrt::ID()const
	{
		return "half_sqrt_x";
	};

	double X2PlusY2::operator() (const Geometry::Point<2>& x)const
	{
		return x[0]*x[0] + x[1]*x[1];
	};
	std::string X2PlusY2::Formula()const
	{
		return "x^2 + y^2";
	};
	std::string X2PlusY2::ID()const
	{
		return "x_2_plus_y_2";
	};

} //namespace MyFunctions
