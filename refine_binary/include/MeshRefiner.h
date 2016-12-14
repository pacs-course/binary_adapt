#ifndef __MESH_REFINER_H
#define __MESH_REFINER_H

#include "BinaryNode.h"
#include "BinaryTreeHelper.h"
#include "AbstractFactory.h"

#include <algorithm> //std::min
#include <string> //std::string
#include <stdexcept> //std::runtime_error

namespace BinaryTree
{
	class NodeOperator
	{
		public:
			NodeOperator(){};
			virtual void operator()(BinaryNode*) = 0;
	};

	class ErrorComputer : public NodeOperator
	{
		public:
			ErrorComputer(double&);
			virtual void operator()(BinaryNode*)override;

			void ResetError();
		protected:
			double& _error_variable;
	};

	template <std::size_t dim>
		class MeshRefiner
		{
			public:
				//TODO: make f loadable dinamically
				MeshRefiner() : _objective_function(nullptr),
									 _godfather(),
									 _global_error(numeric_limits<double>::max()),
									 _error_updated(false)
				{};

				void Init(FunctionPtr<dim> f, int argc, char** argv)
				{
					this->_objective_function = f;
					DerivedInitialization(argc, argv);
				};

				virtual void LoadXdaXta(std::string input) = 0;
				virtual void ExportMesh(std::string output) = 0;

				/*	n_iter is the number of degrees of freedom,
					which corresponds to the number of iterations of the algorithm */
				virtual void Refine(std::size_t n_iter);

				double GlobalError()
				{
					if (!(this->_error_updated))
						UpdateGlobalError();

					return this->_global_error;
				};

			protected:
				virtual void DerivedInitialization(int argc, char** argv) = 0;

				/*
					Method to update the _global_error attribute
					It uses the IterateActiveNodes pure virtual method to iterate the mesh
					An ErrorComputer functor is used to sum the local errors on active elements
				*/
				virtual void UpdateGlobalError();

				/* Iterate the activated nodes of the mesh
					for each node n it calls the functor NodeOperator(n)
					using this virtual method to navigate the mesh I'm taking advantage
					from tha fact that the underlying mesh in concrete refiner classes
					implements an iterator structure to iterate only active elements
					This pattern could be used also to implement other kinds of iterator
				*/
				virtual void IterateActiveNodes(NodeOperator&) = 0;
				virtual void InitializeGodfather() = 0;

			protected:
/*
	The objective function based on which the interpolation error will be computed during the refinement algorithm
*/
				FunctionPtr<dim> _objective_function;
				BinaryGodfather _godfather;
				double _global_error;
				bool _error_updated;
		};

/*
	Factory for MeshRefiner elements
	The key is a string which identifies the underlying library of the concrete object
	The registration is done at loading time by the dynamic library implementing concrete object
*/
	template <size_t dim>
		using MeshRefinerFactory = GenericFactory::ObjectFactory <MeshRefiner<dim>, std::string>;

	template <std::size_t dim>
		void MeshRefiner<dim>::UpdateGlobalError()
		{
			ErrorComputer err_cp(this->_global_error);
			err_cp.ResetError();
			IterateActiveNodes(err_cp);
			this->_error_updated = true;
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
