#ifndef __LIBMESH_REFINER_H
#define __LIBMESH_REFINER_H

#include "MeshRefiner.h"
#include "LibMeshBinarityMap.h"
#include "BinaryTreeHelper.h"

//Basic include file needed for the mesh functionality.
#include "libmesh.h"
#include "mesh.h"
#include "mesh_base.h"
//The MeshData class is deprecated from libMesh and will be removed in future releases 
#ifdef MESH_DATA_VERSION
#include "mesh_data.h"
#endif //MESH_DATA_VERSION

#include "boundary_info.h"
#include "elem.h"

#include <string> //std::string
#include <algorithm> //std::for_each

namespace LibmeshBinary
{
	template <size_t dim>
		class BinaryRefiner : public BinaryTree::MeshRefiner<dim>
		{
			//TODO: libMesh::init(argc, argv) is missing, maybe it has to be called when loading the library
			public :
				using Iterator = libMesh::MeshBase::element_iterator;

				BinaryRefiner() : BinaryTree::MeshRefiner<dim>(),
										_mesh_ptr(nullptr),
#ifdef MESH_DATA_VERSION
										_mesh_data_ptr(nullptr),
#endif //MESH_DATA_VERSION
										_mesh_refinement_ptr(nullptr),
										_initialized(false),
										_libmesh_init_ptr (nullptr)
				{	
#ifdef DESTRUCTOR_ALERT
					cerr << "Sono nel costruttore di LibmeshBinary::BinaryRefiner" << endl;
#endif //DESTRUCTOR_ALERT
				};

				~BinaryRefiner()
				{	
					CheckInitialization();
				}

/*
				Proper methods of the concrete class LibmeshBinary::BinaryRefiner;
				Used to directly set the mesh with an object constructed outside the class and to get the result of the refining
				This methods are usable if I want to bypass the MeshRefinerFactory pattern of the static library refine_binary.a
				i.e. if I'm writing libMesh code, I can just use directly this dynamic class objects
				If I want to stay general (i.e. library independent) this methods are not accesible, since the mesh is passed
				through file input and it is accessed only through base class BinaryTree::MeshRefiner methods
				With this simpler Init I can avoid to pass the main parameters
*/
				void Init(std::unique_ptr<BinaryTree::Functor<dim>> f)
				{
					BinaryTree::MeshRefiner<dim>::Init(std::move(f));
				};

				void Init(std::string functor_id)
				{
					auto& f_factory(BinaryTree::FunctionsFactory<dim>::Instance());
					BinaryTree::MeshRefiner<dim>::Init(std::move(f_factory.create(functor_id)));
				};

				void SetMesh(std::shared_ptr<libMesh::MeshBase> mesh_ptr
#ifdef MESH_DATA_VERSION
								, std::shared_ptr<libMesh::MeshData> data_ptr = nullptr
#endif //MESH_DATA_VERSION
								)
				{
					this->_mesh_ptr = mesh_ptr;
#ifdef MESH_DATA_VERSION
					this->_mesh_data_ptr = data_ptr;
#endif //MESH_DATA_VERSION
					this->_mesh_refinement_ptr = HelperFunctions::MakeUnique<libMesh::MeshRefinement> (*(this->_mesh_ptr));
					BinarityMap::MakeBinary<dim>(*(this->_mesh_refinement_ptr), this->_objective_function);
					InitializeGodfather();
					this->_initialized = true;
				};
				
				std::shared_ptr<libMesh::MeshBase> GetMesh(
#ifdef MESH_DATA_VERSION
																		std::shared_ptr<libMesh::MeshData> data_ptr = nullptr
#endif //MESH_DATA_VERSION
																		) const
				{
					CheckInitialization();
					/* If the class */
					if (this->_libmesh_init_ptr)
						throw logic_error("The mesh can't be exported outside the class that owns the LibMeshInit communicator");
#ifdef MESH_DATA_VERSION
					if (data_ptr)
						data_ptr = this->_mesh_data_ptr;
#endif //MESH_DATA_VERSION
					return this->_mesh_ptr;
				};

				virtual void LoadMesh(std::string input) override
				{
					CheckInitialization();

					std::cout << "\n\n========== reading the mesh ========= \n\n";

					this->_mesh_ptr->read(input
#ifdef MESH_DATA_VERSION
														, this->_mesh_data_ptr
#endif //MESH_DATA_VERSION
														);

					//Replacing the libMesh elements with the BinaryTree ones
					//TODO: maybe it's better to put it at the end of the Init method
					BinarityMap::MakeBinary<dim>(*(this->_mesh_refinement_ptr), this->_objective_function);
					InitializeGodfather();
#ifdef MYDEBUG
					this->_mesh_ptr->print_info();
					this->_mesh_ptr->boundary_info->print_info();

#endif //MYDEBUG
#ifdef MESH_DATA_VERSION
#ifdef MYDEBUG
					this->_mesh_data_ptr->print_info();
#endif //MYDEBUG
#endif //MESH_DATA_VERSION
				};
				
				virtual void ExportMesh(std::string output) override
				{
					CheckInitialization();

					this->_mesh_ptr->write	(output
#ifdef MESH_DATA_VERSION
													,this->_mesh_data_ptr.get()
#endif //MESH_DATA_VERSION
													);
					//TODO:	evaluate the other way given by libMesh to write a mesh
					//TODO:	after the refinement evaluate if it is the case to "unbinarize" the mesh
					//			remember that the binarization guarantees that also the children are binary elements
					//			but for the _elemlinks attribute it cannot be guaranteed the same (binarize a boundary object is meaningless in our
					//			context. Note that, since the pointer to the non binary element is stored inside the binary one, probably the
					//			"unbinarization" does not request much effort
				};

			protected:

				virtual void DerivedInitialization(int argc, char** argv)override
				{
					this->_libmesh_init_ptr = HelperFunctions::MakeUnique<libMesh::LibMeshInit> (argc, argv);

					_mesh_ptr = std::make_shared<libMesh::Mesh> (this->_libmesh_init_ptr->comm(), static_cast<unsigned char> (dim));
#ifdef MESH_DATA_VERSION
					_mesh_data_ptr = std::make_shared<libMesh::MeshData> (*_mesh_ptr);
#endif //MESH_DATA_VERSION
					_mesh_refinement_ptr = HelperFunctions::MakeUnique<libMesh::MeshRefinement> (*_mesh_ptr);

					this->_initialized = true;

				};

				virtual void IterateActiveNodes(BinaryTree::NodeOperator& func) override
				{
					std::for_each( this->_mesh_ptr->active_elements_begin(),
										this->_mesh_ptr->active_elements_end(),
										[&func](libMesh::Elem* el_ptr){return func(BinarityMap::AsBinary(el_ptr));});
				};

				virtual void InitializeGodfather() override
				{
					this->_godfather.template FillElements <Iterator>
																	(this->_mesh_ptr->elements_begin(),
																	 this->_mesh_ptr->elements_end(),
																	 [](Iterator iter){return BinarityMap::AsBinary(*iter);}
																	);

				};

				void CheckInitialization() const
				{
					if(!(this->_initialized))
						throw runtime_error("Trying to use an uninitialized mesh");
				};

			protected:
				std::shared_ptr<libMesh::MeshBase> _mesh_ptr;
#ifdef MESH_DATA_VERSION
				std::shared_ptr<libMesh::MeshData> _mesh_data_ptr;
#endif //MESH_DATA_VERSION
				std::unique_ptr<libMesh::MeshRefinement> _mesh_refinement_ptr;
/*
				libMesh need a LibMeshInit object which has to be constructed before the mesh and destructed after mesh destruction
				To make _initialized true I have to ways:
					- The DerivedInitialization method is called (this means that the Init(f, argc, argv) has been called)
					  In this case the LibMeshInit obj is an attribute of this class
					- The SetMesh method is called
					  In this case the LibMeshInit obj has been constructed outside this class (where the mesh has been constructed)
					  and so I can leave _libmesh_init as nullptr, since I don't need another communicator
*/
				bool _initialized;
/*
				The LibMeshInit object has to be initialized with the main arguments
				Since I don't want to pass any argument to the constructor
				(otherwise I should change the AbstractFactory in order to allow input arguments to the constructor)
				I store it as a pointer, which will be initialized as nullptr
				The real initialization of the LibMeshInit object will be done by the Init method
*/
				unique_ptr<libMesh::LibMeshInit> _libmesh_init_ptr;

		};
} //namespace LibmeshBinary
#endif //__LIBMESH_REFINER_H
