#include "BinaryElements.h"

namespace BinaryTree
{
/*
	Initializing the static attribute
*/
	map<libMesh::MeshBase*, bool> BinarityMap::_boolean_map;

	BinaryNode::BinaryNode() : _s_element	(this),
										_E				(numeric_limits<double>::max()),
										_E_tilde		(numeric_limits<double>::max()),
										_q				(numeric_limits<double>::max())
	{};

	bool BinarityMap::check_binarity(libMesh::MeshBase& mesh)
	{
		/* If the mesh is not stored it insert the new key in the container calling the default constructor for the bool type mapped value, which is false */
		return _boolean_map[&mesh];
	};

	BinaryNode* BinarityMap::as_binary(libMesh::Elem* el)
	{
#ifdef MYDEBUG
		cout << "Interpreto come binario l'indirizzo : " << el << endl;
#endif //MYDEBUG

#ifdef REINTERPRET_VERSION
		return reinterpret_cast<BinaryTree::BinaryNode*> (el);
#else //REINTERPRET_VERSION
		return dynamic_cast<BinaryTree::BinaryNode*> (el);
#endif //REINTERPRET_VERSION
	};

	template <>
		libMesh::Elem* BinarityMap::binarize_node<1>(libMesh::Elem* el_ptr, function_ptr<1> f_ptr)
		{
			libMesh::Elem* result (nullptr);
			switch (el_ptr->type())
			{
				case libMesh::EDGE2 :
					result = BinarityMap::template_node_binarization<1, Interval, libMesh::Edge2> (el_ptr, f_ptr);
					break;

				default :
					throw logic_error("I don't know how to binarize " + to_string(el_ptr->type()) + " type node");
			}
			return result;
		};

	template <>
		libMesh::Elem* BinarityMap::binarize_node<2>(libMesh::Elem* el_ptr, function_ptr<2> f_ptr)
		{
			(void) f_ptr;
			libMesh::Elem* result (nullptr);
			switch (el_ptr->type())
			{
				//TODO : remember to eliminate the warning silencer
				default :
					throw logic_error("I don't know how to binarize " + to_string(el_ptr->type()) + " type node");
			}
			return result;
		};

	template <>
		libMesh::Elem* BinarityMap::binarize_node<3>(libMesh::Elem* el_ptr, function_ptr<3> f_ptr)
		{
			(void) f_ptr;
			libMesh::Elem* result (nullptr);
			switch (el_ptr->type())
			{
				//TODO : remember to eliminate the warning silencer
				default :
					throw logic_error("I don't know how to binarize " + to_string(el_ptr->type()) + " type node");
			}
			return result;
		};

} //namespace BinaryTree


