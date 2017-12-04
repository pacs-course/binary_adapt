#ifndef __F_ELEMENT_H
#define __F_ELEMENT_H

#include "AbstractFElement.h"
#include "LibMeshHelper.h"

#include "libmesh/elem.h" //libMesh::Node
#include "libmesh/mesh_base.h" //libMesh::element_iterator
#include "libmesh/edge_edge2.h" //libMesh::Edge2
#include "libmesh/face_tri3.h" //libMesh::Tri3
#include "libmesh/face_quad4.h" //libMesh::Quad4

namespace LibmeshBinary
{
	using LibmeshIntervalClass = libMesh::Edge2;
	using LibmeshTriangleClass = libMesh::Tri3;
	using LibmeshSquareClass	= libMesh::Quad4;

/**
	Class representing a geometry of InvalidType.
	If an object of this class is constructed, it means
	that something have been wrong.
**/
	class InvalidGeometry
	{
		public:
/**
			The constructor raises an exception
**/
			InvalidGeometry();
/**
			default destructor
**/
			virtual ~InvalidGeometry();

			libMesh::ElemType type() const;

			const libMesh::Node *const * get_nodes();

			void refine(libMesh::MeshRefinement&);

			unsigned int dim () const;
	};

	template <size_t dim, FiniteElements::BasisType FeType = FiniteElements::InvalidFeType, class LibmeshGeometry = InvalidGeometry>
		class FElement : public FiniteElements::AbstractFElement<dim, FeType>
		{
			public:
/**
				default constructor
**/
				FElement();
/**
				constructor from libMesh mesh iterator.
				It takes the ownership of the element obtained dereferenciating the iterator.
**/
				FElement(libMesh::MeshBase::element_iterator);
/**
				constructor from libMesh element.
**/
				FElement(std::unique_ptr<LibmeshGeometry>);
/**
				default destructor
**/
				virtual ~FElement();

/**
				The geometrical type identifier
**/
				virtual Geometry::ElementType GetType() const override;

/**
				Get the vertices of the geometry
**/
				virtual Geometry::NodesVector<dim> GetNodes() override;

/**
				A reference to the underlying libMesh geometry
**/
				const LibmeshGeometry& GetGeometry() const;

/**
				Release the ownership of the libMesh element
**/
				LibmeshGeometry* ReleaseGeometry();

			private:
/**
				This attribute will not be used, it's present only to avoid memory leakage.
				Since the LibmeshGeometry class does not implement neither a smart copy constructor, nor a smart assignment operator, 
				here is stored the pointer to the element we want to endow with #FiniteElements::AbstractFElement;
				since it is stored as a unique_ptr, FElement object takes its ownership, 
				so there's no way that the original libmesh object is accessed directly from the outside.
				This trick guarantees that, when destroying the FElement object, also the underlying libMesh object will be destroyed,
				and all the memory will be freed.
**/
				std::unique_ptr<LibmeshGeometry> _geom;
		};

	//TODO:	the default constructor makes damages when LibmeshGeometry() is called
	//			add the parameters needed by the libMesh::Elem constructor
	template <size_t dim, FiniteElements::BasisType FeType, class LibmeshGeometry>
		FElement<dim, FeType, LibmeshGeometry>::
		FElement()
		{
			std::cerr << "Warning: I'm calling the LibmeshGeometry constructor without parameters" << std::endl;
		};

	template <size_t dim, FiniteElements::BasisType FeType, class LibmeshGeometry>
		FElement<dim, FeType, LibmeshGeometry>::
		FElement(libMesh::MeshBase::element_iterator iter) : _geom(nullptr)
		{
			auto& element = *iter;
			LibmeshGeometry* libmesh_interval = dynamic_cast<LibmeshGeometry*> (element);
			if (!libmesh_interval)
				throw std::logic_error("libMesh iterator type not congruent with FElement one");

			std::unique_ptr<LibmeshGeometry> int_ptr (libmesh_interval);
			_geom = std::move(int_ptr);
			element = nullptr;
		};

	template <size_t dim, FiniteElements::BasisType FeType, class LibmeshGeometry>
		FElement<dim, FeType, LibmeshGeometry>::
		FElement(std::unique_ptr<LibmeshGeometry> element) : _geom(std::move(element))
		{};

	template <size_t dim, FiniteElements::BasisType FeType, class LibmeshGeometry>
		FElement<dim, FeType, LibmeshGeometry>::
		~FElement()
		{};

	template <size_t dim, FiniteElements::BasisType FeType, class LibmeshGeometry>
		Geometry::ElementType FElement<dim, FeType, LibmeshGeometry>::
		GetType() const
		{
			return ConvertLibmeshType (_geom->type());
		};

	template <size_t dim, FiniteElements::BasisType FeType, class LibmeshGeometry>
		Geometry::NodesVector<dim> FElement<dim, FeType, LibmeshGeometry>::
		GetNodes()
		{
			auto geometry_nodes = _geom->get_nodes();
			auto length = _geom->n_nodes();
			Geometry::NodesVector<dim> result(length);

			for (size_t i = 0; i < length; ++i)
			{
				auto current_ptr = geometry_nodes[i];
				Geometry::Point<dim> p = ConvertPoint<dim>(*current_ptr);
				result.Insert(i, p);
			}
			return result; 
		};

	template <size_t dim, FiniteElements::BasisType FeType, class LibmeshGeometry>
		const LibmeshGeometry& FElement<dim, FeType, LibmeshGeometry>::
		GetGeometry() const
		{
			return *_geom;
		};

	template <size_t dim, FiniteElements::BasisType FeType, class LibmeshGeometry>
		LibmeshGeometry* FElement<dim, FeType, LibmeshGeometry>::
		ReleaseGeometry()
		{
			return _geom.release();
		};

} //namespace LibmeshBinary
#endif //__F_ELEMENT_H
