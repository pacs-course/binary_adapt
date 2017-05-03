#include "MyFunctors.h"

#include <math.h>

namespace MyFunctions
{
	double SqrtX::operator() (const Geometry::Point<1>& p)const
	{
		return sqrt(p[0]);
	};
	std::string SqrtX::Name()const
	{
		return "sqrt(x)";
	};

	double HalfStep::operator() (const Geometry::Point<1>& p)const
	{
		double val = p[0];
		if (val > 0.5)
			return 1.0;
		return 0.0;
	};
	std::string HalfStep::Name()const
	{
		return "I(0.5, +inf)";
	};

	double HalfX::operator() (const Geometry::Point<1>& p)const
	{
		double val = p[0];
		if (val >= 0.5)
			return val;
		
		return 0.0;
	};
	std::string HalfX::Name()const
	{
		return "x * I(0.5, +inf)";
	};

	double HalfSquare::operator() (const Geometry::Point<1>& p)const
	{
		double val = p[0];
		if (val >= 0.5)
			return val*val;
		
		return 0.0;
	};
	std::string HalfSquare::Name()const
	{
		return "x^2 * I(0.5, +inf)";
	};

	double HalfTwenty::operator() (const Geometry::Point<1>& p)const
	{
		double val = p[0];
		if (val >= 0.5)
			return pow(val, 20);
		
		return 0.0;
	};
	std::string HalfTwenty::Name()const
	{
		return "x^20 * I(0.5, +inf)";
	};

	double HalfSqrt::operator() (const Geometry::Point<1>& p)const
	{
		double val = p[0];
		if (val > 0.5)
			return sqrt(val);
		return 0.0;
	};
	std::string HalfSqrt::Name()const
	{
		return "sqrt(x) * I(0.5, +inf)";
	};

} //namespace MyFunctions
