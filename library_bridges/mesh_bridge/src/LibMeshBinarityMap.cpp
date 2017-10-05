#include "LibMeshBinarityMap.h"
#include "LibMeshBinaryElements.h"

#include <face_tri3.h>
#include <edge_edge2.h>

namespace LibmeshBinary
{
/*
	Initializing the static attribute
*/
	map<libMesh::MeshBase*, bool> BinarityMap::_boolean_map;

	bool BinarityMap::CheckBinarity(libMesh::MeshBase& mesh)
	{
		/* If the mesh is not stored it insert the new key in the container calling the default constructor for the bool type mapped value, which is false */
		return _boolean_map[&mesh];
	};

	BinaryTree::BinaryNode* BinarityMap::AsBinary(libMesh::Elem* el)
	{
#ifdef MYDEBUG
		cout << "Interpreto come binario l'indirizzo : " << el << endl;
#endif //MYDEBUG

		return dynamic_cast<BinaryTree::BinaryNode*> (el);
	};

	template <>
		libMesh::Elem* BinarityMap::BinarizeNode<1> (libMesh::Elem* el_ptr,
																	BinaryTree::FunctionPtr<1> f_ptr,
																	libMesh::MeshRefinement& mesh_refinement)
		{
			libMesh::Elem* result (nullptr);
			switch (el_ptr->type())
			{
				case libMesh::EDGE2 :
					result = BinarityMap::TemplateNodeBinarization<1, Interval, LibmeshIntervalClass> (el_ptr, f_ptr, mesh_refinement);
					break;

				default :
					throw logic_error("I don't know how to binarize " + to_string(el_ptr->type()) + " type node");
			}
			return result;
		};

	template <>
		libMesh::Elem* BinarityMap::BinarizeNode<2> (libMesh::Elem* el_ptr,
																	BinaryTree::FunctionPtr<2> f_ptr,
																	libMesh::MeshRefinement& mesh_refinement)
		{

			libMesh::Elem* result (nullptr);
			switch (el_ptr->type())
			{
				case libMesh::TRI3 :
					result = BinarityMap::TemplateNodeBinarization<2, Triangle, LibmeshTriangleClass> (el_ptr, f_ptr, mesh_refinement);
					break;

				default :
					throw logic_error("I don't know how to binarize " + to_string(el_ptr->type()) + " type node");
			}
			return result;
		};

} //namespace LibmeshBinary

