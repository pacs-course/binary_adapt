#include "LibMeshHelper.h"

namespace LibmeshBinary
{
	template <>
	libMesh::ElemType ConvertType<Geometry::IntervalType> ()
	{
		return LibmeshIntervalType;
	}

	template <>
	libMesh::ElemType ConvertType<Geometry::SquareType> ()
	{
		return LibmeshSquareType;
	}

	template <>
	libMesh::ElemType ConvertType<Geometry::TriangleType> ()
	{
		return LibmeshTriangleType;
	}

	template <>
	libMesh::ElemType ConvertType<Geometry::CubeType> ()
	{
		return LibmeshCubeType;
	}

	Geometry::ElementType ConvertLibmeshType (libMesh::ElemType type)
	{
		Geometry::ElementType result = Geometry::InvalidElementType;

		switch (type)
		{
			case libMesh::EDGE2 :
				result = Geometry::IntervalType;
				break;

			case libMesh::TRI3 :
				result = Geometry::TriangleType;
				break;

			case libMesh::QUAD4 :
				result = Geometry::SquareType;
				break;

			case libMesh::HEX8 :
				result = Geometry::CubeType;
				break;

			default :
				break;
		};
		return result;
	}


} //namespace LibmeshBinary

