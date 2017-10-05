#ifndef __F_ELEMENT_H
#define __F_ELEMENT_H

#include "AbstractFElement.h"
#include "LibMeshHelper.h"

#include <elem.h> //libMesh::Node
#include <mesh_base.h> //libMesh::element_iterator
#include <edge_edge2.h> //libMesh::Edge2
#include <face_tri3.h> //libMesh::Tri3
#include <face_quad4.h> //libMesh::Quad4

namespace LibmeshBinary
{
	using LibmeshIntervalClass = libMesh::Edge2;
	using LibmeshTriangleClass = libMesh::Tri3;
	using LibmeshSquareClass	= libMesh::Quad4;

	class InvalidGeometry
	{
		public:
			InvalidGeometry()
			{
				throw std::logic_error("Trying to use an invalid geometry");
			};
			~InvalidGeometry(){};

			libMesh::ElemType type() const
			{
				return LibmeshInvalidType;
			};

			const libMesh::Node *const * get_nodes()
			{
				return nullptr;
			};

			void refine(libMesh::MeshRefinement&)
			{
				throw std::logic_error("Trying to refine an invalid geometry");
			};

			unsigned int dim () const
			{
				return 0;
			};
	};

	//TODO: check if the virtual inheritance could be dangerous in this case
	template <size_t dim, FiniteElements::BasisType FeType = FiniteElements::InvalidFeType, class LibmeshGeometry = InvalidGeometry>
		class FElement : public FiniteElements::AbstractFElement<dim, FeType>
		{
			public:
				//TODO:	the default constructor makes damages when LibmeshGeometry() is called
				//			add the parameters needed by the libMesh::Elem constructor
				FElement()
				{
					std::cout << "WARNING: I'm calling the LibmeshGeometry constructor without parameters" << std::endl;
				};

				FElement(libMesh::MeshBase::element_iterator iter) : _geom(nullptr)
				{
					auto& element = *iter;
					LibmeshGeometry* libmesh_interval = dynamic_cast<LibmeshGeometry*> (element);
					if (!libmesh_interval)
						throw std::logic_error("libMesh iterator type not congruent with FElement one");

					std::unique_ptr<LibmeshGeometry> int_ptr (libmesh_interval);
					_geom = move(int_ptr);
					element = nullptr;
				};

				FElement(std::unique_ptr<LibmeshGeometry> element) : _geom(move(element)){};

				~FElement(){};

				virtual Geometry::ElementType GetType() const override
				{
					return ConvertLibmeshType (_geom->type());
				};

				virtual Geometry::NodesVector<dim> GetNodes() override
				{
					auto geometry_nodes = _geom->get_nodes();
					auto length = _geom->n_nodes();
					Geometry::NodesVector<dim> result;

					for (size_t i(0); i < length; ++i)
					{
						auto current_ptr = geometry_nodes[i];
						Geometry::Point<dim> p = ConvertPoint<dim>(*current_ptr);
						result.push_back(p);
					}
					return result; 
				};

				LibmeshGeometry& GetGeometry()
				{
					return *_geom;
				};

			private:
/*
				This attribute will not be used, it's present only to avoid memory leakage.
				Since the LibmeshGeometry class does not implement neither a smart copy constructor, nor a smart assignment operator, I store here the pointer to the element I want my stuff to be added; since I store it as a unique_ptr, I take the ownership of it, so there's no way that the original libmesh object is accessed directly from the outside.
				This trick guarantees me that, when destroying my FElement, also the underlying object will be destroyed.
*/
				std::unique_ptr<LibmeshGeometry> _geom;
		};

} //namespace LibmeshBinary
#endif //__F_ELEMENT_H
