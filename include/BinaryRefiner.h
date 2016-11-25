#ifndef __BINARY_REFINER_H
#define __BINARY_REFINER_H

#include "BinaryElements.h"
#include <algorithm> //std::min

using namespace std;

namespace BinaryTree
{
	class BinaryRefiner : public libMesh::MeshRefinement
	{
		public:
			BinaryRefiner(libMesh::MeshBase&);
/*
	f is the objective function based on which the interpolation error will be computed during the refinement algorithm
	N is the number of degrees of freedom, which correspond to the number of iterations of the algorithm
*/
			template <size_t DIM>
			void refine_binary(function_ptr<DIM> f, size_t N)
			{
				libMesh::MeshBase& mesh(this->get_mesh());

				if (DIM != mesh.mesh_dimension())
					throw logic_error("Mesh dimension and objective function dimension disagree");

				BinarityMap::make_binary<DIM>(mesh, f);

				auto master = mesh.elem_ptr(0);

#ifdef MYDEBUG
				if (BinarityMap::as_binary(master))
					cout << "Element 0 is effectively binary" << endl;
#endif //MYDEBUG

////////////MasterGhost godfather;
				BinaryNode* daddy = BinarityMap::as_binary(master);
				BinaryNode* previous_daddy = daddy;
				while (N)
				{
					daddy->bisect(*this);
					while(daddy)
					{
						auto val = daddy->pLevel();
						daddy->pLevel(val + 1);

						auto hansel = daddy->left ();
						auto gretel = daddy->right();

						auto new_E = min	(	hansel->E() + gretel->E(),
													daddy->projectionError()
												);

						auto old_E_tilde = daddy->E_tilde();
						auto new_E_tilde =	new_E * old_E_tilde
													/
													(new_E + old_E_tilde);

						daddy->E(new_E);
						daddy->E_tilde(new_E_tilde);

						BinaryNode* alfa_bro(nullptr);
						hansel->q() > gretel->q() ? alfa_bro = hansel : alfa_bro = gretel;

						auto new_q = min	(	alfa_bro->q(),
													new_E_tilde
												);
						daddy->q(new_q);

						daddy->s(alfa_bro->s()); 

						previous_daddy = daddy;										 
						daddy = daddy->dad();
					}

					//TODO: this statement must be changed, the element to divide must be choosed by the godfather
					daddy = previous_daddy->s();
					--N;
				}
				//TODO:	after the refinement evaluate if it is the case to "unbinarize" the mesh
				//			remember that the binarization guarantees that also the children are binary elements
				//			but for the _elemlinks attribute it cannot be guaranteed the same (binarize a boundary object is meaningless in our
				//			context. Note that, since the pointer to the non binary element is stored inside the binary one, probably the
				//			"unbinarization" does not request much effort
			};
	};
}

#endif //__BINARY_REFINER_H
