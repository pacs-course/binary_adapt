#ifndef __LIBMESH_HELPER_H
#define __LIBMESH_HELPER_H

#include "libmesh/enum_elem_type.h" //libMesh::ElemType
#include "libmesh/point.h" //libmesh::point
#include "libmesh/enum_quadrature_type.h" //libMesh::QuadratureType

#include "TypeEnumerations.h"
#include "LinearAlgebra.h"

#include <type_traits> //enable_if

namespace LibmeshBinary
{
	/**
		Renaming libMesh enumerators for the use in this library.
		In libMesh there are multiple types describing the same geometry (i.e. TRI3, TRI6):
		in this way a potential switch is easier, since it must be done only ones here.
		"using" syntax cannot be used to rename enumerators, only for types.
	**/
	//TODO: use a more c++ style way to do it
#define LibmeshInvalidType	libMesh::INVALID_ELEM
#define LibmeshIntervalType libMesh::EDGE2
#define LibmeshTriangleType libMesh::TRI3
#define LibmeshSquareType	libMesh::QUAD4
#define LibmeshCubeType		libMesh::HEX8

	/**
		Conversion from Geometry::ElementType to correspondent libMesh::ElemType.
		It is necessary for the initialization of the libMesh::QBase object.
		Since #Geometry::ElementType is known at compile time,
		the conversion can be done as template type specialization.
	**/
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

	/**
		Conversion from libMesh::ElemType to correspondent Geometry::ElementType.
		libMesh::ElemType is known at runtime (it's the output of the pure virtual method type() of the libMesh::Elem class),
		so the function cannot be defined template as ConvertType().
	**/
	Geometry::ElementType ConvertLibmeshType (libMesh::ElemType type);

	/**
		Conversion from libMesh::point to #Geometry::Point.
		libmesh::point has 3 coordinates, so for dim > 3 it does not make sense.
	**/
	template < size_t dim,
			   typename std::enable_if< (dim < 4), size_t>::type = 0>
	Geometry::Point<dim> ConvertPoint (const libMesh::Point& point)
	{
		Geometry::Point<dim> result;
		size_t i (0);
		for (auto& iter : result)
			iter = point (i++);

		return result;
	};

	/**
		Conversion from string to enum type
	**/
	libMesh::QuadratureType ConvertQuadType (std::string);


} //namespace LibmeshBinary
#endif //__LIBMESH_HELPER_H
