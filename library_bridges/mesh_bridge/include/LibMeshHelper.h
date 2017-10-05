#ifndef __LIBMESH_HELPER_H
#define __LIBMESH_HELPER_H

#include <enum_elem_type.h> //libMesh::ElemType
#include <point.h> //libmesh::point

#include "TypeEnumerations.h"
#include "Point.h"

#include <type_traits> //enable_if

namespace LibmeshBinary
{
/*
	renaming libMesh enumerators for the use in my program
	since in libMesh there are multiple types describing the same geometry, in this way a potential switch is easier, since it must be done only ones here
	cannot use the using syntax since I'm renaming enumerators, not types
*/
	//TODO: find a more c++ style way to do it
	#define LibmeshInvalidType libMesh::INVALID_ELEM
	#define LibmeshIntervalType libMesh::EDGE2
	#define LibmeshTriangleType libMesh::TRI3
	#define LibmeshSquareType libMesh::QUAD4
	#define LibmeshCubeType libMesh::HEX8

/*
	conversion from Geometry::ElementType to the correspondent libMesh::ElemType,
	necessary for the initialization of the libMesh::QBase object
	I use the fact that ElementType is known at compile time,
	so the conversion is done in the template specialization
*/
	template <Geometry::ElementType Type = Geometry::InvalidElementType>
		libMesh::ElemType ConvertType()
		{
			return LibmeshInvalidType;
		};

	template <>
		libMesh::ElemType ConvertType<Geometry::IntervalType> ();

	template <>
		libMesh::ElemType ConvertType<Geometry::SquareType> ();

	template <>
		libMesh::ElemType ConvertType<Geometry::TriangleType> ();

	template <>
		libMesh::ElemType ConvertType<Geometry::CubeType> ();

/*
	It does the conversion in the opposite direction
	type is known at runtime (it's the output of the pure virtual method type() of the libMesh::Elem class)
	so I cannot define the function as the previous one
*/
	Geometry::ElementType ConvertLibmeshType (libMesh::ElemType type);

/*
	conversion from libMesh::point to Geometry::Point<dim>
	libmesh::point has 3 coordinates, so for dim > 3 it does not make sense
*/
	template <size_t dim, typename std::enable_if< (dim < 4), size_t>::type = 0>
		Geometry::Point<dim> ConvertPoint(const libMesh::Point& point)
		{
			Geometry::Point<dim> result;
			size_t i(0);
			for (auto& iter : result)
				iter = point(i++);

			return result;
		};



} //namespace LibmeshBinary
#endif //__LIBMESH_HELPER_H
