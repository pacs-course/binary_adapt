#ifndef __MESH_REFINER_H
#define __MESH_REFINER_H

#include "BinaryNode.h"
#include "BinaryTreeHelper.h"

#include <algorithm> //std::min
#include <string> //std::string
#include <stdexcept> //std::runtime_error

namespace BinaryTree
{
	template <std::size_t dim>
		class MeshRefiner
		{
			public:
				//TODO: make f loadable dinamically
				MeshRefiner(FunctionPtr<dim> f) : _objective_function(f), _godfather() {};
				virtual void Init(int argc, char** argv) = 0;
				virtual void LoadXdaXta(std::string input) = 0;
				virtual void ExportMesh(std::string output) = 0;

				/*	n_iter is the number of degrees of freedom,
					which corresponds to the number of iterations of the algorithm */
				virtual void Refine(std::size_t n_iter);
			protected:
				virtual void InitializeGodfather() = 0;

			protected:
/*
	The objective function based on which the interpolation error will be computed during the refinement algorithm
*/
				FunctionPtr<dim> _objective_function;
				BinaryGodfather _godfather;
		};

	template <std::size_t dim>
		void MeshRefiner<dim>::Refine(std::size_t n_iter)
		{
			BinaryNode* daddy(nullptr);
			BinaryNode* previous_daddy(nullptr);

			while (n_iter)
			{
#ifdef MYDEBUG
				std::cout << "Iterazione #" << n_iter << std::endl;
#endif //MYDEBUG
				daddy = this->_godfather.MakeBisection();
				
				while(daddy)
				{
					auto val = daddy->PLevel();
					daddy->PLevel(val + 1);

					auto hansel = daddy->Left ();
					auto gretel = daddy->Right();

					auto new_E = std::min(	hansel->E() + gretel->E(),
													daddy->ProjectionError()
												);

					auto old_E_tilde = daddy->ETilde();
					auto new_E_tilde = new_E * old_E_tilde
											 /
											 (new_E + old_E_tilde);

					daddy->E(new_E);
					daddy->ETilde(new_E_tilde);

					BinaryNode* alfa_bro(nullptr);
					hansel->Q() > gretel->Q() ? alfa_bro = hansel : alfa_bro = gretel;

					auto new_q = std::min (alfa_bro->Q(), new_E_tilde);
					daddy->Q(new_q);

					daddy->S(alfa_bro->S());

					previous_daddy = daddy;									 
					daddy = previous_daddy->Dad();
				} //while(daddy)

				--n_iter;
			} //while(n_iter)

			(this->_godfather).SelectActiveNodes();
		}; //Refine()

} //namespace BinaryTree
#endif //__MESH_REFINER_H
