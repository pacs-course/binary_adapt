#include "LibMeshFElement.h"

namespace LibmeshBinary
{
	InvalidGeometry::InvalidGeometry()
	{
		throw std::logic_error ("Trying to use an invalid geometry");
	}

	InvalidGeometry::~InvalidGeometry()
	{}

	libMesh::ElemType InvalidGeometry::type() const
	{
		return LibmeshInvalidType;
	}

	const libMesh::Node* const* InvalidGeometry::get_nodes()
	{
		return nullptr;
	}

	void InvalidGeometry::refine (libMesh::MeshRefinement&)
	{
		throw std::logic_error ("Trying to refine an invalid geometry");
	}

	unsigned int InvalidGeometry::dim () const
	{
		return 0;
	}

} //namespace LibmeshBinary
