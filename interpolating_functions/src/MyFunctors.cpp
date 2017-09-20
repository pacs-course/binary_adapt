#include "MyFunctors.h"

#include <math.h>

using namespace std;

namespace MyFunctions
{
	double SqrtX::operator() (const Geometry::Point<1>& p) const
	{
		return sqrt(p[0]);
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
		double val = p[0];
		if (val > 0.5)
			return 1.0;
		return 0.0;
	};
	string HalfStep::Formula()const
	{
		return "I(0.5, +inf)";
	};
	string HalfStep::ID()const
	{
		return "half_step";
	};

	double HalfX::operator() (const Geometry::Point<1>& p) const
	{
		double val = p[0];
		if (val >= 0.5)
			return val;
		
		return 0.0;
	};
	string HalfX::Formula()const
	{
		return "x * I(0.5, +inf)";
	};
	string HalfX::ID()const
	{
		return "half_x";
	};

	double HalfSquare::operator() (const Geometry::Point<1>& p) const
	{
		double val = p[0];
		if (val >= 0.5)
			return val*val;
		
		return 0.0;
	};
	string HalfSquare::Formula()const
	{
		return "x^2 * I(0.5, +inf)";
	};
	string HalfSquare::ID()const
	{
		return "half_square";
	};

	double HalfTwenty::operator() (const Geometry::Point<1>& p) const
	{
		double val = p[0];
		if (val >= 0.5)
			return pow(val, 20);
		
		return 0.0;
	};
	string HalfTwenty::Formula()const
	{
		return "x^20 * I(0.5, +inf)";
	};
	string HalfTwenty::ID()const
	{
		return "half_twenty";
	};

	double HalfSqrt::operator() (const Geometry::Point<1>& p) const
	{
		double val = p[0];
		if (val > 0.5)
			return sqrt(val);
		return 0.0;
	};
	string HalfSqrt::Formula()const
	{
		return "sqrt(x) * I(0.5, +inf)";
	};
	string HalfSqrt::ID()const
	{
		return "half_sqrt_x";
	};

} //namespace MyFunctions
