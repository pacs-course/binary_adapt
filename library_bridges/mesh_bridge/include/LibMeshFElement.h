#ifndef __F_ELEMENT_H
#define __F_ELEMENT_H

#include "AbstractFElement.h"
#include "LibMeshHelper.h"

#include <elem.h> //libMesh::Node
#include <mesh_base.h> //libMesh::element_iterator

namespace LibmeshBinary
{
	class InvalidGeometry
	{
		public:
			InvalidGeometry()
			{
				throw logic_error("Trying to use an invalid geometry");
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

			void refine(libMesh::MeshRefinement& mr)
			{
				//to silence warning by the compiler
				(void)mr;
				throw logic_error("Trying to refine an invalid geometry");
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
					cout << "WARNING: I'm calling the LibmeshGeometry constructor without parameters" << endl;
				};

				FElement(libMesh::MeshBase::element_iterator iter) : _geom(nullptr)
				{
					auto& element = *iter;
					LibmeshGeometry* libmesh_interval = dynamic_cast<LibmeshGeometry*> (element);
					if (!libmesh_interval)
						throw logic_error("libMesh iterator type not congruent with FElement one");

					unique_ptr<LibmeshGeometry> int_ptr (libmesh_interval);
					_geom = move(int_ptr);
					element = nullptr;
				};

				//TODO:	maybe it's convenient to have in the input the mesh iterator, so I can ensure that the mesh element is substituted
				FElement(unique_ptr<LibmeshGeometry> element) : _geom(move(element))
				{
#ifdef MYDEBUG
					cout << "Sono nel costruttore di FElement" << endl;
					std::cout << "CONTROLLO NODI" << std::endl;
					auto el_nodes = GetNodes();
					std::cout << "primo nodo :" << el_nodes[0][0] << std::endl;
					std::cout << "secondo nodo :" << el_nodes[1][0] << std::endl;

					cout << "Controllo i nodi di FElement" << endl;
					std::cout << "CONTROLLO NODI" << std::endl;
					el_nodes = GetNodes();
					std::cout << "primo nodo :" << el_nodes[0][0] << std::endl;
					std::cout << "secondo nodo :" << el_nodes[1][0] << std::endl;
#endif //MYDEBUG
				};

				~FElement(){};
				virtual Geometry::ElementType GetType() const override
				{
					return ConvertLibmeshType (_geom->type());
				};

				virtual NodesVector<dim> GetNodes() override
				{
					auto geometry_nodes = _geom->get_nodes();
					auto length = _geom->n_nodes();
					NodesVector<dim> result;

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
				This attribute will not be used, it's presente only to avoid memory leakage.
				Since the LibmeshGeometry class does not implement neither a smart copy constructor, nor a smart assignment operator, I store here the pointer to the element I want my stuff to be added; since I store it as a unique_ptr, I take the ownership of it, so there's no way that the original libmesh object is accessed directly from the outside.
				This trick guarantees me that, when destroying my FElement, also the underlying object will be destroyed.
*/
				unique_ptr<LibmeshGeometry> _geom;
		};

} //namespace LibmeshBinary
#endif //__F_ELEMENT_H
