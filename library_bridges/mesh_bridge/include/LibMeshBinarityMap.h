#ifndef __LIBMESH_BINARITY_MAP_H
#define __LIBMESH_BINARITY_MAP_H

#include "libmesh/mesh.h" //libMesh::MeshBase
#include <elem.h> //libMesh::Elem
#include <mesh_refinement.h> //libMesh::MeshRefinement

#include "BinaryNode.h"
#include "LinearAlgebra.h"
#include "Functor.h"

namespace LibmeshBinary
{
	using namespace std;
/**
	Class to convert a libMesh mesh.
	It contains functions needed to make a generic
	libMesh tree a binary tree, that is a tree made
	of #BinaryTree::AbstractBinaryElement nodes.
	All informations and functions to pass from
	a libMesh point of view to a BinaryTree one 
	are concentrated here, with the purpose of
	minimizing the possibility to get in trouble.
**/
	class BinarityMap
	{
		public:
/**
			Convert the mesh stored by the input libMesh refiner.
			It convert every element of the mesh to its correspondent
			#LibmeshBinary::BinaryTreeElement binary counterpart.
			The resulting mesh will be composed of binary tree elements.
**/
			template <size_t dim>
				static void MakeBinary(libMesh::MeshRefinement&, BinaryTree::FunctionPtr<dim>);

/**
			Cast the mesh element to a binary node element
**/
			template <size_t dim>
				static BinaryTree::DimensionedNode<dim>* AsBinary(libMesh::Elem*);

/**
			Cast the const mesh element to a binary node element
**/
			template <size_t dim>
				static const BinaryTree::DimensionedNode<dim>* AsBinary(const libMesh::Elem*);

/**
			Check if the input mesh is made of binary tree elements.
**/
			static bool CheckBinarity(libMesh::MeshBase& mesh);

/**
			Convert a libMesh element to its binary counterpart
**/
			template <size_t dim>
				static libMesh::Elem* BinarizeNode (libMesh::Elem*,
																BinaryTree::FunctionPtr<dim>, 
																libMesh::MeshRefinement&);

		private:
/**
			Templated conversion. Needed by BinarizeNode()
**/
			template <size_t dim, class BinaryClass, class LibmeshClass>
				static libMesh::Elem* TemplateNodeBinarization
				(
					libMesh::Elem*,
					BinaryTree::FunctionPtr<dim>,
					libMesh::MeshRefinement&
				);

		private:
//			TODO: check if this static variable could make the code not re-entrant
/**
			A map which tells if an object of libMesh::Meshbase has been binarized.
			The compulsory way to binarize an element is by calling BinarityMap::MakeBinary<dim> (mesh_to_be_binarized, f).
**/
			static map<libMesh::MeshBase*, bool> _boolean_map;
	};


	//TODO: introduce support for meshes containing different dimension elements (i.e. both hexes and quads in the same mesh)
	template <size_t dim>
		void BinarityMap::MakeBinary(libMesh::MeshRefinement& mesh_refinement, BinaryTree::FunctionPtr<dim> f_ptr)
		{
			libMesh::MeshBase& mesh(mesh_refinement.get_mesh());
/*
			The mesh can contain elements of different dimension (i.e. volume objects and boundary objects)
			The mesh_dimension() method, if multi-dimensional meshes are supported, returns the largest dimension of mesh elements.
			For the binary tree algorithm I'm interested only in highest dimensional objects
			TODO: test this multi-dimensional case
*/
			if (mesh.mesh_dimension() != dim)
				throw invalid_argument("In MakeBinary dim parameter is different from mesh dimension!");

			for (auto iter(mesh.elements_begin()); iter != mesh.elements_end(); ++iter)
			{
				auto& el_ptr(*iter);
				auto dad = el_ptr->parent();
				int index = -1;
				if (dad)
				{
					for (int i(0); i < static_cast<int> (dad->n_children()); ++i)
						if(dad->child(i) == el_ptr)
							index = i;
				}

				auto bin_ptr = BinarityMap::BinarizeNode<dim> (el_ptr, f_ptr, mesh_refinement);
/*
				since Interval constructor uses the default copy constructor of the Geometry,
				I don't want to destroy the underlying libmesh object **iter;
				because of this I don't use the insert_elem method of the mesh, but I simply reassign the *iter address.
				Note that if the Geometry class implemented the copy constructor, this instruction would cause a memory leakage, 
				since I insert the copy without freeing the memory pointed by *iter.
*/
				if (el_ptr != bin_ptr)
				{
					el_ptr = bin_ptr;

					if (dad)
						dad->replace_child(el_ptr, index);

					if (el_ptr->has_children())
						for (size_t i = 0; i < el_ptr->n_children(); ++i)
							el_ptr->child(i)->set_parent(el_ptr);
/*
					I have to replace the binarized element in neighbors address list
					TODO: the first neighbor should be the father; check if previous replacement could be avoided
*/
					for (size_t i = 0; i < el_ptr->n_neighbors(); ++i)
					{
						auto friend_ptr = el_ptr->neighbor_ptr(i);
						if (friend_ptr)
							for (size_t j = 0; j < friend_ptr->n_neighbors(); ++j)
							{
								auto friend_of_my_friend = friend_ptr->neighbor_ptr(j);
								if (friend_of_my_friend == el_ptr)
									friend_ptr->set_neighbor(j, el_ptr);
							}
					}
				}
			}

			/* After the binarization I update the map */
			_boolean_map[&mesh] = true;
		};

//TODO: optimize the function in order to not check for all the elements if the cast has already been done (maybe give the element you want to binarize as input parameter, setted nullptr by default, and if it is nullptr check all the elements)
	template <size_t dim, class BinaryClass, class LibmeshClass>
		libMesh::Elem* BinarityMap::TemplateNodeBinarization
		(
			libMesh::Elem* el_ptr,
			BinaryTree::FunctionPtr<dim> f_ptr,
			libMesh::MeshRefinement& mesh_refinement
		)
		{
/*
			The mesh can contain elements of different dimension (i.e. volume objects and boundary objects)
			Since for the binary tree algorithm I need only the dim objects, I binarize only these ones
			TODO: verify if returning the element as it is, could cause problems in case it has not the appropriate dimensionality
*/
			if (el_ptr->dim() != dim)
				return el_ptr;

			BinaryClass* result = nullptr;
/*
			I check if the element has been already casted (i.e. in adaptation, we want to transform only the children but not the father);
			WARNING : if I don't add this two lines the functions generates a segmentation fault error when calling the map initialization inside the el->init() instruction; the first line of AffineMap::init() cannot access the nodes_ptr address memory
*/
			result = dynamic_cast<BinaryClass*> (el_ptr);
			if (!result)
			{
				bool type_recognized = false;

				LibmeshClass* casted_ptr = dynamic_cast<LibmeshClass*> (el_ptr);
				unique_ptr<LibmeshClass> smart_ptr(casted_ptr);

				if (smart_ptr)
				{
					type_recognized = true;

					/*WARNING:	when I go out from the for cicle libmesh detect a memory leak due to the destruction of casted_ptr
									without deleting the underlying object; the problem is that this object is also pointed by *iter, since it is
									an element of the mesh, so it cannot be destroyed by calling delete casted_ptr*/
					result = new BinaryClass(move(smart_ptr), f_ptr, mesh_refinement);

					result->Init();
				}

				if(!type_recognized)
					throw invalid_argument("Cannot convert the input libMesh element into requested type!");
			}

			return result;
		};

	template <size_t dim>
		BinaryTree::DimensionedNode<dim>* BinarityMap::AsBinary(libMesh::Elem* el)
		{
			return dynamic_cast<BinaryTree::DimensionedNode<dim>*> (el);
		};

	template <size_t dim>
		const BinaryTree::DimensionedNode<dim>* BinarityMap::AsBinary(const libMesh::Elem* el)
		{
			return dynamic_cast<const BinaryTree::DimensionedNode<dim>*> (el);
		};

	template <size_t dim>
		libMesh::Elem* BinarityMap::BinarizeNode
		(
			libMesh::Elem*,
			BinaryTree::FunctionPtr<dim>, 
			libMesh::MeshRefinement&
		)
		{
			/* In the default case (unexpected dim) it throws an exception; concrete behaviour for able-to-handle dim is defined in the specializations */
			throw invalid_argument("Cannot handle an element of dimension " + to_string(dim));
			return nullptr;
		};

	template <>
		libMesh::Elem* BinarityMap::BinarizeNode<1> (libMesh::Elem* el_ptr,
																	BinaryTree::FunctionPtr<1> f_ptr,
																	libMesh::MeshRefinement& mesh_refinement);

	template <>
		libMesh::Elem* BinarityMap::BinarizeNode<2> (libMesh::Elem* el_ptr,
																	BinaryTree::FunctionPtr<2> f_ptr,
																	libMesh::MeshRefinement& mesh_refinement);

} //namespace LibmeshBinary


#endif //__LIBMESH_BINARITY_MAP_H
