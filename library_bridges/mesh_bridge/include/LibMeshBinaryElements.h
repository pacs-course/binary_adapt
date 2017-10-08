#ifndef __LIBMESH_BINARY_ELEMENT_H
#define __LIBMESH_BINARY_ELEMENT_H

#include "LibMeshFElement.h"
#include "AbstractBinaryElement.h"
#include "LibMeshBinarityMap.h"

#include <memory> //std::shared_ptr
#include <functional> //std::function

#include <mesh_refinement.h>

namespace LibmeshBinary
{
	using namespace std;


	template <size_t dim, FiniteElements::BasisType FeType = FiniteElements::InvalidFeType, class LibmeshGeometry = InvalidGeometry>
		class BinaryTreeElement : public LibmeshGeometry, public BinaryTree::AbstractBinaryElement<dim, FeType>
		{
			public:
				BinaryTreeElement	(	unique_ptr<LibmeshGeometry> element_ptr,
											BinaryTree::FunctionPtr<dim> f,
											libMesh::MeshRefinement& mesh_refinement
										) :
										LibmeshGeometry(*element_ptr),
										BinaryTree::AbstractBinaryElement<dim, FeType>	(f,
																										 new FElement <dim, FeType,
																															LibmeshGeometry> (move(element_ptr))
																										),
										_mesh_refinement(mesh_refinement)
				{				};
				
				virtual ~BinaryTreeElement()
				{
#ifdef DESTRUCTOR_ALERT
					clog << "Distruggo BinaryTreeElement" << endl;
#endif //DESTRUCTOR_ALERT
				};

				virtual BinaryTree::BinaryNode* Left()
				{
					if (this->has_children())
						return BinarityMap::AsBinary<dim>(this->child(0));

					return nullptr;
				}
				virtual BinaryTree::BinaryNode* Right()
				{
					if (this->has_children())
						return BinarityMap::AsBinary<dim>(this->child(1));

					return nullptr;
				};
				virtual BinaryTree::BinaryNode* Dad()
				{
					return BinarityMap::AsBinary<dim>(this->parent());
				};

				virtual size_t NodeID() override
				{
#ifdef VERBOSE
					cout << "Nodes : ";
					auto nodes = this->get_nodes();
					auto length = this->n_nodes();
					size_t i(0);
					while (i < length-1)
					{
						auto current_ptr = nodes[i];
						size_t j(0);
						while (j < dim - 1)
							cout << (*current_ptr)(j++) << ",";
						cout << (*current_ptr)(j) << "	";
						++i;
					}
					auto current_ptr = nodes[i];
					size_t j(0);
					while (j < dim - 1)
						cout << (*current_ptr)(j++) << ",";
					cout << (*current_ptr)(j) << endl;
#endif //VERBOSE
					return static_cast<size_t>(this->id());
				};

				virtual void Bisect() override
				{
#ifdef MYDEBUG
					clog << "Sono in BinElement::refine" << endl;
#endif //MYDEBUG
					/* First I check that the derived class which implements the refine makes two children */
					if (this->n_children() != 2)
						throw logic_error("This is not a binary element!");

					this->set_refinement_flag(libMesh::Elem::REFINE);

					libMesh::Elem::refine(this->_mesh_refinement);
#ifdef MYDEBUG
					clog << "Effettuata la Elem::refine" << endl;
					clog << "L'indirizzo del primo figlio e' : " << this->child(0) << endl;
					clog << "L'indirizzo del secondo figlio e' : " << this->child(1) << endl;
#endif //MYDEBUG
					//TODO: optimize this step, it recomputes all the mesh elements, not only the two new children
					BinarityMap::MakeBinary<dim>(this->_mesh_refinement, this->_f);
#ifdef MYDEBUG
					clog << "Binarizzati i nuovi elementi" << endl;
					clog << "L'indirizzo del primo figlio e' : " << this->child(0) << endl;
					clog << "L'indirizzo del secondo figlio e' : " << this->child(1) << endl;
					clog << "Visti come BinaryNode*" << endl;
					clog << "L'indirizzo del primo figlio e' : " << Left() << endl;
					clog << "L'indirizzo del secondo figlio e' : " << Right() << endl;
					auto& mesh(_mesh_refinement.get_mesh());
					int cont = mesh.n_elem();
					for (auto iter = mesh.elements_begin(); iter != mesh.elements_end(); ++iter)
						if ((*iter)->refinement_flag() == libMesh::Elem::INACTIVE)
							--cont;
					clog << "After refinement mesh has " << mesh.n_elem() << " elements, " << cont << " of them are ACTIVE" << endl;
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
	class Triangle : public BinaryTreeElement<2, FiniteElements::WarpedType, LibmeshTriangleClass>
	{
		public:
			Triangle(unique_ptr<LibmeshTriangleClass> el_ptr,
						BinaryTree::FunctionPtr<2> f,
						libMesh::MeshRefinement& mesh_refinement);

			virtual ~Triangle(){};

			virtual float	embedding_matrix (const unsigned int child_num,
														const unsigned int child_node_num,
														const unsigned int parent_node_num) const override
			{
				return _binary_embedding_matrices[this->_longest_side][child_num][child_node_num][parent_node_num];
			};

			virtual unsigned int n_children () const override
			{
				return 2;
			};

		protected:
			/*
				this are the embedding matrices to be used for the binary bisection of the triangle
				the one which will be effectively used depends on which is the longest side
			*/
			static const float _binary_embedding_matrices[3][2][3][3];
			// the index of the longest side
			size_t _longest_side;
	};


}; //namespace LibmeshBinary

#endif //__LIBMESH_BINARY_ELEMENT_H
