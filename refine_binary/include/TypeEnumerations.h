#ifndef __TYPE_ENUMERATIONS_H
#define __TYPE_ENUMERATIONS_H

namespace FiniteElements
{
	enum BasisType
	{
		TrivialType	= 0,
		LegendreType = 1,
		InvalidFeType = 99
	};

} //namespace FiniteElements

namespace Geometry
{
	enum ElementType
	{
		IntervalType = 0,
		SquareType	 = 1,
		TriangleType = 2,
		CubeType		 = 3,
		InvalidElementType = 99
	};

} //namespace Geometry

#endif //__TYPE_ENUMERATIONS_H
