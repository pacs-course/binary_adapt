#ifndef __LIBMESH_REFINER_H
#define __LIBMESH_REFINER_H

#include "MeshRefiner.h"
#include "LibMeshBinarityMap.h"
#include "BinaryTreeHelper.h"

//Basic include file needed for the mesh functionality.
#include "libmesh/libmesh.h"
#include "libmesh/mesh.h"
#include "libmesh/mesh_base.h"
#include "libmesh/boundary_info.h"
#include "libmesh/elem.h"

#include <string> //std::string
#include <algorithm> //std::for_each

/**
	Implementation of BinaryTree abstract structures based on libMesh library.
**/
namespace LibmeshBinary
{
/**
	Class implementing concrete #BinaryTree::MeshRefiner object based on 
	<a href="https://libmesh.github.io/doxygen/index.html">link text</a> library.
	#BinaryTree::MeshRefiner defines methods to implement the refinement algorithm, but has no info about the underlying mesh.
	LibmeshRefiner uses libMesh library to manage the tree structure and the resulting mesh,
	and to define operations like node bisection and iteration over mesh elements,
	which are declared at an abstract level in #BinaryTree::MeshRefiner class.

	This class could be also meant as an extension of libMesh,
	i.e. if libMesh is directly linked in the program, additional methods for mesh inport/export and initialization
	can be used, which do not belong to MeshRefiner base class since they cannot be defined at that level.
**/
	template <size_t dim>
		class LibmeshRefiner : public BinaryTree::MeshRefiner<dim>
		{
			public :
				using Iterator = libMesh::MeshBase::element_iterator;
/**
				default constructor.
				The DerivedInitialization() method will initialize attributes
**/
				LibmeshRefiner();

/**
				default destructor.
**/
				virtual ~LibmeshRefiner();

/**
				Directly set the mesh with an object constructed outside the class.
				Proper method of the concrete class LibmeshBinary::LibmeshRefiner;
				used to directly set the mesh with an object constructed outside the class and to get the result of the refining.
				This method is meant to be used in order to bypass the MeshRefinerFactory pattern of the library refine_binary,
				i.e. if I'm writing libMesh code, I can just use directly these dynamic class objects.
				If I want to stay general (i.e. library independent) this method is not accessible, since the mesh is passed
				through file input and it is accessed only through base class BinaryTree::MeshRefiner methods.
**/
				void SetMesh(std::shared_ptr<libMesh::MeshBase>);

/**
				Directly extract the mesh as a libMesh object.
				Proper method of the concrete class LibmeshBinary::LibmeshRefiner;
				used to directly get the result of the refining.
				As for SetMesh method, this method is meant to be used in order to bypass the
				MeshRefinerFactory pattern of the library refine_binary.
				If I want to stay general (i.e. library independent) this method is not accessible,
				since the mesh is exported on output file and it is accessed only through base class BinaryTree::MeshRefiner methods.
**/				
				std::shared_ptr<libMesh::MeshBase> GetMesh()	const;

/**
				Simpler Init to avoid to pass the main parameters.
				Proper method of the concrete class LibmeshBinary::LibmeshRefiner;
				if I'm here I'm writing libMesh code, this is not an override of a MeshRefiner method.
**/
				void Init(std::unique_ptr<BinaryTree::Functor<dim>>);

/**
				Overloaded version of previous Init method;
				It searches for the id passed as input in the functors factory, then it passes the factory output to the previous Init method.
**/
				void Init(std::string);

/**
				Implementation of mesh import from file.
				Declared pure virtual in base class.
**/
				virtual void LoadMesh(std::string) override;

/**
				Implementation of mesh export on file.
				Declared pure virtual in base class.
**/				
				virtual void ExportMesh(std::string) const override;

/**
				The overloads defined in base class
**/
				using BinaryTree::MeshRefiner<dim>::IterateActiveNodes;

/**
				Implementation of const active node iteration.
				Declared pure virtual in base class.
**/
				virtual void IterateActiveNodes(BinaryTree::ConstOperator&) const override;

/**
				Implementation of const active node iteration.
				Declared pure virtual in base class.
**/
				virtual void IterateActiveNodes(BinaryTree::ConstDimOperator<dim>&) const override;

			protected:
/**
				Implementation of non-const active node iteration.
				Declared pure virtual in base class.
**/
				virtual void IterateActive(BinaryTree::NodeOperator&) override;
/**
				Implementation of non-const active node iteration.
				Declared pure virtual in base class.
**/
				virtual void IterateActive(BinaryTree::DimOperator<dim>&) override;


/**
				Initialization of libMesh objects.
				The LibMeshInit communicator is constructed with main arguments passed as input;
				the Mesh attribute is initialized, even if the mesh has to be built yet;
				the MeshRefinement attribute is initialized with the just constructed Mesh attribute
**/
				virtual void DerivedInitialization(int, char**) override;

/**
				Method that initialize _godfather attribute.
				It fills _godfather attribute elements storage with active elements of the mesh.
**/
				virtual void InitializeGodfather() override;

/**
				Method raising an exception if the object has not been initialized.
**/
				void CheckInitialization() const;

			protected:
/**
				libMesh communicator.
				The LibMeshInit object has to be initialized with the main arguments.
				Since I don't want to pass any argument to the constructor
				(otherwise I should change the AbstractFactory in order to allow input arguments to the constructor)
				I store it as a pointer, which will be initialized as nullptr
				The real initialization of the LibMeshInit object will be done by the Init method
**/
				unique_ptr<libMesh::LibMeshInit> _libmesh_init_ptr;

/**
				pointer to the mesh object.
**/
				std::shared_ptr<libMesh::MeshBase> _mesh_ptr;
/**
				pointer to libMesh MeshRefinement object.
				It's needed to make any refinement on any element of the mesh pointed by _mesh_ptr.
**/
				std::unique_ptr<libMesh::MeshRefinement> _mesh_refinement_ptr;
/**
				Flag telling if libMesh has been initialized.
				libMesh needs a LibMeshInit object which has to be constructed before the mesh and destructed after mesh destruction.
				To make _initialized true there are two ways:
					-	the DerivedInitialization() method is called (this means that the Init(f, argc, argv) has been called);
						in this case the LibMeshInit object is an attribute of this class;
					-	the SetMesh() method is called;
						in this case the LibMeshInit object has been constructed outside this class (where the mesh has been constructed)
						and so _libmesh_init if left as nullptr, since another communicator is not needed.
**/
				bool _initialized;

		};

	template <size_t dim>
		LibmeshRefiner<dim>::LibmeshRefiner(): BinaryTree::MeshRefiner<dim>(),
															_libmesh_init_ptr (nullptr),
															_mesh_ptr(nullptr),
															_mesh_refinement_ptr(nullptr),
															_initialized(false)
		{};

	template <size_t dim>
		LibmeshRefiner<dim>::~LibmeshRefiner()
		{};

	template <size_t dim>
		void LibmeshRefiner<dim>::Init(std::unique_ptr<BinaryTree::Functor<dim>> f)
		{
			BinaryTree::MeshRefiner<dim>::Init(std::move(f));
		};

	template <size_t dim>
		void LibmeshRefiner<dim>::Init(std::string functor_id)
		{
			auto& f_factory(BinaryTree::FunctionsFactory<dim>::Instance());
			BinaryTree::MeshRefiner<dim>::Init(std::move(f_factory.create(functor_id)));
		};

	template <size_t dim>
		void LibmeshRefiner<dim>::SetMesh( std::shared_ptr<libMesh::MeshBase> mesh_ptr)
		{
			this->_mesh_ptr = mesh_ptr;

			this->_mesh_refinement_ptr = Helpers::MakeUnique<libMesh::MeshRefinement> (*(this->_mesh_ptr));
			BinarityMap::MakeBinary<dim>(*(this->_mesh_refinement_ptr), this->_objective_function);
			InitializeGodfather();
			this->_initialized = true;
		};

	template <size_t dim>
		std::shared_ptr<libMesh::MeshBase> LibmeshRefiner<dim>::GetMesh()	const
		{
			CheckInitialization();
			/* If the libMesh communicator pointer is not null, this means that the mesh has been constructed
				inside this class, and not outside; since libMesh requires that libMesh objects are created and
				destructed where LibMeshInit communicator is created and destructed, if _libmesh_init_ptr is not
				null I can't export the mesh as a libMesh object */
			if (this->_libmesh_init_ptr)
				throw logic_error("The mesh can't be exported outside the class that owns the LibMeshInit communicator");

			return this->_mesh_ptr;
		};

	template <size_t dim>
		void LibmeshRefiner<dim>::LoadMesh(std::string input)
		{
			CheckInitialization();

			clog << "\n\n========== reading the mesh ========= \n\n" << endl;

			this->_mesh_ptr->read(input);

			//Replacing the libMesh elements with the BinaryTree ones
			//TODO: verify if it could be better to put it at the end of the Init method
			BinarityMap::MakeBinary<dim>(*(this->_mesh_refinement_ptr), this->_objective_function);
			InitializeGodfather();
		};

	template <size_t dim>
		void LibmeshRefiner<dim>::ExportMesh(std::string output) const
		{
			CheckInitialization();

			this->_mesh_ptr->write	(output);
			//TODO:	evaluate the other way given by libMesh to write a mesh
			//TODO:	after the refinement evaluate if it is the case to "unbinarize" the mesh
			//			remember that the binarization guarantees that also the children are binary elements
			//			but for the _elemlinks attribute it cannot be guaranteed the same (binarize a boundary object is meaningless in our
			//			context. Note that, since the pointer to the non binary element is stored inside the binary one, probably the
			//			"unbinarization" does not request much effort
		};

	template <size_t dim>
		void LibmeshRefiner<dim>::IterateActiveNodes(BinaryTree::ConstOperator& func) const
		{
			std::for_each( this->_mesh_ptr->active_elements_begin(),
								this->_mesh_ptr->active_elements_end(),
								[&func](const libMesh::Elem* el_ptr){return func(BinarityMap::AsBinary<dim>(el_ptr));});
		};

	template <size_t dim>
		void LibmeshRefiner<dim>::IterateActiveNodes(BinaryTree::ConstDimOperator<dim>& func) const
		{
			std::for_each( this->_mesh_ptr->active_elements_begin(),
								this->_mesh_ptr->active_elements_end(),
								[&func](const libMesh::Elem* el_ptr){return func(BinarityMap::AsBinary<dim>(el_ptr));});
		};


	template <size_t dim>
		void LibmeshRefiner<dim>::IterateActive(BinaryTree::NodeOperator& func)
		{
			std::for_each( this->_mesh_ptr->active_elements_begin(),
								this->_mesh_ptr->active_elements_end(),
								[&func](libMesh::Elem* el_ptr){return func(BinarityMap::AsBinary<dim>(el_ptr));});
		};

	template <size_t dim>
		void LibmeshRefiner<dim>::IterateActive(BinaryTree::DimOperator<dim>& func)
		{
			std::for_each( this->_mesh_ptr->active_elements_begin(),
								this->_mesh_ptr->active_elements_end(),
								[&func](libMesh::Elem* el_ptr){return func(BinarityMap::AsBinary<dim>(el_ptr));});
		};


	template <size_t dim>
		void LibmeshRefiner<dim>::DerivedInitialization(int argc, char** argv)
		{
			this->_libmesh_init_ptr = Helpers::MakeUnique<libMesh::LibMeshInit> (argc, argv);

			_mesh_ptr = std::make_shared<libMesh::Mesh> (this->_libmesh_init_ptr->comm(), static_cast<unsigned char> (dim));

			_mesh_refinement_ptr = Helpers::MakeUnique<libMesh::MeshRefinement> (*_mesh_ptr);

			this->_initialized = true;

		};

	template <size_t dim>
		void LibmeshRefiner<dim>::InitializeGodfather()
		{
			this->_godfather.template FillElements <Iterator>
															(this->_mesh_ptr->active_elements_begin(),
															 this->_mesh_ptr->active_elements_end(),
															 [](Iterator iter){return BinarityMap::AsBinary<dim>(*iter);}
															);

		};

	template <size_t dim>
		void LibmeshRefiner<dim>::CheckInitialization() const
		{
			if(!(this->_initialized))
				throw runtime_error("Trying to use an uninitialized mesh");
		};

} //namespace LibmeshBinary
#endif //__LIBMESH_REFINER_H
