#ifndef __LIBMESH_BINARY_ELEMENT_H
#define __LIBMESH_BINARY_ELEMENT_H

#include "LibMeshFElement.h"
#include "AbstractBinaryElement.h"
#include "LibMeshBinarityMap.h"

#include <memory> //std::shared_ptr
#include <functional> //std::function

#include <mesh_refinement.h>
#include <face_tri3.h>
#include <edge_edge2.h>

using namespace std;

namespace LibmeshBinary
{
	using LibmeshIntervalClass = libMesh::Edge2;
	using LibmeshTriangleClass = libMesh::Tri3;

	template <size_t dim, FiniteElements::BasisType FeType = FiniteElements::InvalidFeType, class LibmeshGeometry = InvalidGeometry>
		class BinaryTreeElement : public LibmeshGeometry, public BinaryTree::AbstractBinaryElement<dim, FeType>
		{
			public:
				BinaryTreeElement (	shared_ptr<FElement<dim, FeType, LibmeshGeometry> > fe_ptr, 
											BinaryTree::FunctionPtr<dim> f,
											libMesh::MeshRefinement& mesh_refinement
										):
										LibmeshGeometry(fe_ptr->GetGeometry()),
										BinaryTree::AbstractBinaryElement<dim, FeType>(f, fe_ptr, mesh_refinement)
				{};

				BinaryTreeElement	(	unique_ptr<LibmeshGeometry> element_ptr,
											BinaryTree::FunctionPtr<dim> f,
											libMesh::MeshRefinement& mesh_refinement
										) :
										LibmeshGeometry(*element_ptr),
										BinaryTree::AbstractBinaryElement<dim, FeType>	(f,
																										 make_shared <FElement <dim,
																																		FeType,
																																		LibmeshGeometry>
																														 >(move(element_ptr))
																										),
										_mesh_refinement(mesh_refinement)
				{				};
				
				virtual ~BinaryTreeElement()
				{
#ifdef DESTRUCTOR_ALERT
					cout << "Distruggo BinaryTreeElement" << endl;
#endif //DESTRUCTOR_ALERT
				};

				virtual BinaryTree::BinaryNode* Left()
				{
					if (this->has_children())
						return BinarityMap::AsBinary(this->child(0));

					return nullptr;
				}
				virtual BinaryTree::BinaryNode* Right()
				{
					if (this->has_children())
						return BinarityMap::AsBinary(this->child(1));

					return nullptr;
				};
				virtual BinaryTree::BinaryNode* Dad()
				{
					return BinarityMap::AsBinary(this->parent());
				};

				virtual void Bisect() override
				{
#ifdef MYDEBUG
					cout << "Sono in BinElement::refine" << endl;
#endif //MYDEBUG
					/* First I check that the derived class which implements the refine makes two children */
					if (this->n_children() != 2)
						throw logic_error("This is not a binary element!");

					this->set_refinement_flag(libMesh::Elem::REFINE);

					libMesh::Elem::refine(this->_mesh_refinement);
#ifdef MYDEBUG
					cout << "Effettuata la Elem::refine" << endl;
					cout << "L'indirizzo del primo figlio e' : " << this->child(0) << endl;
					cout << "L'indirizzo del secondo figlio e' : " << this->child(1) << endl;
#endif //MYDEBUG
					//TODO: optimize this step, it recomputes all the mesh elements, not only the two new children
					BinarityMap::MakeBinary<dim>(this->_mesh_refinement, this->_f);
#ifdef MYDEBUG
					cout << "Binarizzati i nuovi elementi" << endl;
					cout << "L'indirizzo del primo figlio e' : " << this->child(0) << endl;
					cout << "L'indirizzo del secondo figlio e' : " << this->child(1) << endl;
					cout << "Visti come BinaryNode*" << endl;
					cout << "L'indirizzo del primo figlio e' : " << Left() << endl;
					cout << "L'indirizzo del secondo figlio e' : " << Right() << endl;
					auto& mesh(_mesh_refinement.get_mesh());
					int cont = mesh.n_elem();
					for (auto iter = mesh.elements_begin(); iter != mesh.elements_end(); ++iter)
						if ((*iter)->refinement_flag() == libMesh::Elem::INACTIVE)
							--cont;
					std::cout << "After refinement mesh has " << mesh.n_elem() << " elements, " << cont << " of them are ACTIVE" << endl;
#endif //MYDEBUG
				};

				virtual void Activate() override
				{
					/* when I activate my binary node, I also change the libMesh::Elem::_p_level,
						so that the p-refinement can be see also in a libMesh framework */
					this->set_p_level(this->PLevel());

					if (!(this->active()))
						this->set_refinement_flag(libMesh::Elem::JUST_REFINED);
				};

				virtual void Deactivate() override
				{
					if (this->active())
						this->set_refinement_flag(libMesh::Elem::INACTIVE);
				};

			private:
				libMesh::MeshRefinement& _mesh_refinement;
		};

	//TODO: use GetPot to select FElement BasisType at runtime	
	using Interval = BinaryTreeElement<1, FiniteElements::LegendreType, LibmeshIntervalClass>;

	//It must be modified the way libMesh elements divides theirselves
	//The info about the procreation should be stored in the elem.embedding_matrix() method
	class Triangle : public BinaryTreeElement<2, FiniteElements::LegendreType, LibmeshTriangleClass>
	{
		public:
			Triangle(unique_ptr<LibmeshTriangleClass> el_ptr,
						BinaryTree::FunctionPtr<2> f,
						libMesh::MeshRefinement& mesh_refinement) :
						BinaryTreeElement<2, LegendreType, LibmeshTriangleClass>(move(el_ptr), f, mesh_refinement)
			{};
			virtual ~Triangle(){};

			virtual float	embedding_matrix (const unsigned int child_num,
														const unsigned int child_node_num,
														const unsigned int parent_node_num) const override
			{
				//TODO: override the method to make it bisective

				return 0.0;
			};

			virtual unsigned int n_children () const override
			{
				return 2;
			};
	};

}; //namespace LibmeshBinary

#endif //__LIBMESH_BINARY_ELEMENT_H
