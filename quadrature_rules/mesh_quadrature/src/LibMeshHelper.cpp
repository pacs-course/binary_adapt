#include "LibMeshHelper.h"

namespace LibmeshBinary
{
	template <>
	libMesh::ElemType ConvertType<Geometry::IntervalType> ()
	{
		return LibmeshIntervalType;
	};

	template <>
	libMesh::ElemType ConvertType<Geometry::SquareType> ()
	{
		return LibmeshSquareType;
	};

	template <>
	libMesh::ElemType ConvertType<Geometry::TriangleType> ()
	{
		return LibmeshTriangleType;
	};

	template <>
	libMesh::ElemType ConvertType<Geometry::CubeType> ()
	{
		return LibmeshCubeType;
	};

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
	};

	libMesh::QuadratureType ConvertQuadType (std::string type)
	{
		if (type == "QGAUSS")			 return libMesh::QGAUSS;
		if (type == "QJACOBI_1_0")		 return libMesh::QJACOBI_1_0;
		if (type == "QJACOBI_2_0")		 return libMesh::QJACOBI_2_0;
		if (type == "QSIMPSON")			 return libMesh::QSIMPSON;
		if (type == "QTRAP")			 return libMesh::QTRAP;
		if (type == "QGRID")			 return libMesh::QGRID;
		if (type == "QGRUNDMANN_MOLLER") return libMesh::QGRUNDMANN_MOLLER;
		if (type == "QMONOMIAL")		 return libMesh::QMONOMIAL;
		if (type == "QCONICAL")			 return libMesh::QCONICAL;
		if (type == "QGAUSS_LOBATTO")	 return libMesh::QGAUSS_LOBATTO;
		if (type == "QCLOUGH")			 return libMesh::QCLOUGH;
		if (type == "QCOMPOSITE")		 return libMesh::QCOMPOSITE;

		return libMesh::INVALID_Q_RULE;
	};

} //namespace LibmeshBinary

