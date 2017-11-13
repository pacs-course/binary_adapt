#include "BinaryTreeHelper.h"

using namespace std;

namespace HelperFunctions
{
	string BasisTypeConverter(FiniteElements::BasisType const & id)
	{
		switch(id)
		{
			case(FiniteElements::TrivialType)		: return "TrivialType";
			case(FiniteElements::LegendreType)		: return "LegendreType";  
			case(FiniteElements::WarpedType)			: return "WarpedType";  
			case(FiniteElements::InvalidFeType)		: return "InvalidFeType";
			case(FiniteElements::ThirdPartyFeType)	: return "ThirdPartyFeType";
		}
		return "NULLSTRING";
	};

	string ElementTypeConverter(Geometry::ElementType const & id)
	{
		switch(id)
		{
			case(Geometry::IntervalType)				: return "IntervalType";
			case(Geometry::SquareType)					: return "SquareType";
			case(Geometry::TriangleType)				: return "TriangleType";
			case(Geometry::CubeType)					: return "CubeType";  
			case(Geometry::InvalidElementType)		: return "InvalidElementType";
			case(Geometry::ThirdPartyElementType)	: return "ThirdPartyElementType";
		}
		return "NULLSTRING";
	};


	double IntPower (double basis, size_t exp)
	{
#ifdef BASIC
		double power = 1;
		if (exp)
		{
			power = basis;
			for (size_t j = 1; j < exp; ++j)
				power *= basis;
		}
		return power;
#else //BASIC

		if (exp == 0)
			return 1.0;
		if (exp == 1)
			return basis;
#ifndef RECURSIVE
		double power = basis;
		size_t cur_exp = 2;
		for (; cur_exp <= exp; cur_exp *= 2)
			power *= power;

		for (cur_exp /= 2; cur_exp < exp; ++cur_exp)
			power *= basis;
		return power;
#else //RECURSIVE

		if (exp == 2)
			return basis * basis;

		double half_exp_result = IntPower(basis * basis, exp/2);
		return exp % 2 ? basis * half_exp_result : half_exp_result;
#endif //RECURSIVE

#endif //BASIC
	};

	template <>
		double Power<0> (double)
		{
			return 1.0;
		};

	template <>
		double Power<1> (double basis)
		{
			return basis;
		};

	template <>
		double Power<2> (double basis)
		{
			return basis * basis;
		};

} //namespace HelperFunctions
