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

} //namespace HelperFunctions
