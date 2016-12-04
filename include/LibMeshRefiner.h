#ifndef __LIBMESH_REFINER_H
#define __LIBMESH_REFINER_H

#include "MeshRefiner.h"
#include "LibMeshBinarityMap.h"

//Basic include file needed for the mesh functionality.
#include "libmesh.h"
#include "mesh.h"
#include "mesh_base.h"
#include "mesh_data.h"
#include "boundary_info.h"
#include "elem.h"

#include <string> //std::string

namespace LibmeshBinary
{
	template <size_t dim>
		class BinaryRefiner : public BinaryTree::MeshRefiner<dim>
		{
			//TODO: libMesh::init(argc, argv) is missing, maybe it has to be called when loading the library
			public :
				using Iterator = libMesh::MeshBase::element_iterator;

				BinaryRefiner(BinaryTree::FunctionPtr<dim> f) : BinaryTree::MeshRefiner<dim>(f),
																				_mesh_ptr(nullptr),
																				_mesh_data_ptr(nullptr),
																				_mesh_refinement_ptr(nullptr),
																				_initialized(false)
				{	};

				virtual void Init(int argc, char**argv) override
				{
					libMesh::LibMeshInit init (argc, argv);
					_mesh_ptr = std::make_shared<libMesh::Mesh> (init.comm(), static_cast<unsigned char> (dim));
					_mesh_data_ptr = std::make_shared<libMesh::MeshData> (*_mesh_ptr);
					_mesh_refinement_ptr = std::make_unique<libMesh::MeshRefinement> (*_mesh_ptr);

					this->_initialized = true;
				};

				void SetMesh(std::shared_ptr<libMesh::MeshBase> mesh_ptr, std::shared_ptr<libMesh::MeshData> data_ptr = nullptr)
				{
					this->_mesh_ptr = mesh_ptr;
					this->_mesh_data_ptr = data_ptr;
					this->_mesh_refinement_ptr = std::make_unique<libMesh::MeshRefinement> (*(this->_mesh_ptr));
					BinarityMap::MakeBinary<dim>(*(this->_mesh_refinement_ptr), this->_objective_function);
					InitializeGodfather();
					this->_initialized = true;
				};
				
				virtual void LoadXdaXta(std::string input) override
				{
					CheckInitialization();
#ifdef MYDEBUG
					std::cout << "\n\n========== reading the mesh ========= \n\n";
#endif //MYDEBUG
					this->_mesh_ptr->read(input + ".xda");

					//Replacing the libMesh elements with the BinaryTree ones
					//TODO: maybe it's better to put it at the end of the Init method
					BinarityMap::MakeBinary<dim>(*(this->_mesh_refinement_ptr), this->_objective_function);
					InitializeGodfather();
#ifdef MYDEBUG
					this->_mesh_ptr->print_info();
					this->_mesh_ptr->boundary_info->print_info();

					std::cout << "\n\n========== reading the mesh data ========= \n\n";
#endif //MYDEBUG

					// Create and read the mesh data.
					this->_mesh_data_ptr->enable_compatibility_mode();
					this->_mesh_data_ptr->read(input + ".xta"); 
#ifdef MYDEBUG
					this->_mesh_data_ptr->print_info();
#endif //MYDEBUG
				};
				
				virtual void ExportMesh(std::string output)
				{
					CheckInitialization();

					//TODO:	after the refinement evaluate if it is the case to "unbinarize" the mesh
					//			remember that the binarization guarantees that also the children are binary elements
					//			but for the _elemlinks attribute it cannot be guaranteed the same (binarize a boundary object is meaningless in our
					//			context. Note that, since the pointer to the non binary element is stored inside the binary one, probably the
					//			"unbinarization" does not request much effort
				};

			protected:

				virtual void InitializeGodfather() override
				{
					this->_godfather.template FillElements <Iterator>
																(_mesh_ptr->elements_begin(),
																 _mesh_ptr->elements_end(),
																 [](Iterator iter){return BinarityMap::AsBinary(*iter);}
																);

				};

				void CheckInitialization()
				{
					if(!(this->_initialized))
						throw runtime_error("Trying to use an uninitialized mesh");
				};

			protected:
				std::shared_ptr<libMesh::MeshBase> _mesh_ptr;
				std::shared_ptr<libMesh::MeshData> _mesh_data_ptr;

				std::unique_ptr<libMesh::MeshRefinement> _mesh_refinement_ptr;
				bool _initialized;
		};
} //namespace LibmeshBinary
#endif //__LIBMESH_REFINER_H
