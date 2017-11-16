#ifndef __LIBMESH_BINARITY_MAP_H
#define __LIBMESH_BINARITY_MAP_H

#include "libmesh/mesh.h" //libMesh::MeshBase
#include "libmesh/libmesh.h"
#include <elem.h> //libMesh::Elem
#include <mesh_refinement.h> //libMesh::MeshRefinement

#include "BinaryNode.h"
#include "LinearAlgebra.h"
#include "Functor.h"


namespace LibmeshBinary
{
	using namespace std;

	struct BinarityMap
	{
		static bool CheckBinarity(libMesh::MeshBase& mesh);

		//TODO: introduce support for meshes containing different dimension elements (i.e. both hexes and quads in the same mesh)
		template <size_t dim>
			static void MakeBinary(libMesh::MeshRefinement& mesh_refinement, BinaryTree::FunctionPtr<dim> f_ptr)
			{
#ifdef MYDEBUG
				cout << "Sono in MakeBinary" << endl;
#endif //MYDEBUG
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
					since Interval constructor uses the default copy constructor of the Geometry, I don't want to destroy the underlying libmesh object **iter; because of this I don't use the insert_elem method of the mesh, but I simply reassign the *iter address. Note that if the Geometry class implemented the copy constructor, this instruction would cause a memory leak, since I insert the copy without freeing the memory pointed by *iter
*/
					if (el_ptr != bin_ptr)
					{
						el_ptr = bin_ptr;

						if (dad)
							dad->replace_child(el_ptr, index);

						if (el_ptr->has_children())
							for (size_t i = 0; i < el_ptr->n_children(); ++i)
								el_ptr->child(i)->set_parent(el_ptr);

					//I have to replace the binarized element in neighbors address list
					//TODO: the first neighbor should be the father; maybe previous replacement could be avoided
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

				//TODO (evaluate the possibility to "unbinarize" the mesh after the refinement)

				mesh.prepare_for_use(/*skip_renumber =*/ false);

#ifdef MYDEBUG
				cout << "La mesh ha " << mesh.n_elem() << " miei elementi; li conto" << endl;
				for (auto iter = mesh.elements_begin(); iter != mesh.elements_end(); ++iter)
					cout << "Elemento numero " << (*iter)->id() << endl;
#endif //MYDEBUG

				/* After the binarization I update the map */
				_boolean_map[&mesh] = true;
			};

		template <size_t dim>
			static libMesh::Elem* BinarizeNode (libMesh::Elem*,
															BinaryTree::FunctionPtr<dim>, 
															libMesh::MeshRefinement&)
			{
				/* In the default case (unexpected dim) it throws an exception; concrete behaviour for able-to-handle dim is defined in the specializations */
				throw invalid_argument("Cannot handle an element of dimension " + to_string(dim));
				return nullptr;
			};

//TODO: optimize the function in order to not check for all the elements if the cast has already been done (maybe give the element you want to binarize as input parameter, setted nullptr by default, and if it is nullptr check all the elements)
		template <size_t dim, class BinaryClass, class LibmeshClass>
			static libMesh::Elem* TemplateNodeBinarization (libMesh::Elem* el_ptr,
																			BinaryTree::FunctionPtr<dim> f_ptr,
																			libMesh::MeshRefinement& mesh_refinement)
			{
#ifdef THROW_EXCEPTION
				Binaryclass test1;
				LibmeshClass test2;
				if (test1.Type() != test2.Type())
					throw logic_error("Invalid combination of template parameter inside binarize_node");
#endif //THROW_EXCEPTION
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
				TODO: verify why this happens
*/
				result = dynamic_cast<BinaryClass*> (el_ptr);
				if (result)
				{
#ifdef MYDEBUG
					cout << "Already binary the address : " << el_ptr << endl;
#endif //MYDEBUG
				}
				else
				{
#ifdef MYDEBUG
					cout << "I make binary the address : " << el_ptr << endl;
#endif //MYDEBUG

					bool type_recognized = false;

					LibmeshClass* casted_ptr = dynamic_cast<LibmeshClass*> (el_ptr);
					unique_ptr<LibmeshClass> smart_ptr(casted_ptr);

#ifdef MYDEBUG
					cout << "Made smart the address : " << smart_ptr.get() << endl;
#endif //MYDEBUG
					if (smart_ptr)
					{
						type_recognized = true;

						/*WARNING:	when I go out from the for cicle libmesh detect a memory leak due to the destruction of casted_ptr
										without deleting the underlying object; the problem is that this object is also pointed by *iter, since it is
										an element of the mesh, so it cannot be destroyed by calling delete casted_ptr*/
						result = new BinaryClass(move(smart_ptr), f_ptr, mesh_refinement);

						result->Init();
#ifdef MYDEBUG
						cout << "Vediamo se il mio elemento ha il mio metodo:" << endl;
						cout << result->dim() << endl;
						cout << "Sostituisco nella mesh l'elemento di ID=" << result->id() << endl;
#endif //MYDEBUG
#ifdef MYDEBUG
						cout << "Vediamo se nella mesh ho il mio elemento:" << endl;
						auto t = el_ptr->dim();
						cout << t << endl;
						cout << "Il mio elemento e' ancora lo stesso?" << endl;
						cout << result->dim() << endl;
#endif //MYDEBUG
#ifdef MYDEBUG
						cout << "I have a binary object at the address : " << el_ptr << endl;
#endif //MYDEBUG

					}
					if(!type_recognized)
						throw invalid_argument("Cannot convert the input libMesh element into requested type!");
				}

				return result;
			};

		template <size_t dim>
			static BinaryTree::DimensionedNode<dim>* AsBinary(libMesh::Elem* el)
			{
#ifdef MYDEBUG
				cout << "Interpreto come binario l'indirizzo : " << el << endl;
#endif //MYDEBUG

				return dynamic_cast<BinaryTree::DimensionedNode<dim>*> (el);
			};

/*
		A map which tells me if an object of libMesh::Meshbase has been binarized
		The compulsory way to binarize an element is by the call of BinarityMap::MakeBinary<dim> (mesh_to_be_binarized, f)
*/
		//TODO: since this file has to be in a dynamic loadable library, check if this static variable could make the library "non rientrante"
		static map<libMesh::MeshBase*, bool> _boolean_map;

	}; //struct BinarityMap

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
