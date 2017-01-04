#ifndef __TYPE_ENUMERATIONS_H
#define __TYPE_ENUMERATIONS_H

/*
	enumerations of available types for the finite element basis and for the geometries
	a third-party type is present in both cases to allow external users of this library
	to add new types without modify the library code and behaviour
	in this case the user must do the registration in the correspondent factories
	to use his proper version of the library classes
*/

namespace FiniteElements
{
	enum BasisType
	{
		TrivialType	= 0,
		LegendreType = 1,

		InvalidFeType = 99,

		ThirdPartyFeType = 100
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

		InvalidElementType = 99,

		ThirdPartyElementType = 100
	};

} //namespace Geometry

#endif //__TYPE_ENUMERATIONS_H
