#ifndef __MESH_REFINER_H
#define __MESH_REFINER_H

#include "BinaryNode.h"
#include "BinaryTreeHelper.h"
#include "AbstractFactory.h"
#include "Functor.h"

#include <algorithm> //std::min
#include <string> //std::string
#include <stdexcept> //std::runtime_error
#include <fstream> //std::ofstream

namespace BinaryTree
{
	class NodeOperator
	{
		public:
			NodeOperator(){};
			virtual void operator()(BinaryNode*) = 0;
	};

	class Counter : public NodeOperator
	{
		public:
			Counter():_counter(0){};
			virtual void operator()(BinaryNode*)override;
			size_t GetCount()const;
		protected:
			size_t _counter;
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

	class PlevelsExtractor : public NodeOperator
	{
		public:
			PlevelsExtractor() : _p_levels(){};
			virtual void operator()(BinaryNode*)override;
			
			std::vector<size_t> GetPLevels();
		protected:
			std::vector<size_t> _p_levels;
	};

	template <std::size_t dim>
		class MeshRefiner
		{
			public:
				MeshRefiner() : _objective_function(nullptr),
									 _godfather(),
									 _global_error(numeric_limits<double>::max()),
									 _error_updated(false)
				{};

/*
				functor is the identifier to be passed to FunctionsFactory
				from the resultant object will be initialized the _objective_function attribute
*/
				void Init(std::string functor, int argc, char** argv)
				{
					auto& f_factory(FunctionsFactory<dim>::Instance());
					std::unique_ptr<Functor<dim>> f_ptr = f_factory.create(functor);

					Init(move(f_ptr));

					DerivedInitialization(argc, argv);
				};

				virtual void LoadMesh(std::string input) = 0;
				virtual void ExportMesh(std::string output) = 0;

/*
				It produces a gnuplot script which plots the mesh with the plevel associated to each element
				It is available only for 1D meshes, other dim values has to be implemented
*/
				template <std::size_t dummy = dim, typename std::enable_if< (dummy == 1), size_t>::type = 0>
				void ExportGnuPlot(const std::string&);

				/*	n_iter is the number of degrees of freedom,
					which corresponds to the number of iterations of the algorithm */
				virtual void Refine(std::size_t n_iter);

				double GlobalError()
				{
					if (!(this->_error_updated))
						UpdateGlobalError();

					return this->_global_error;
				};

				std::vector<size_t> ExtractPLevels()
				{
					PlevelsExtractor p_ex;
					this->IterateActiveNodes(p_ex);
					return p_ex.GetPLevels();
				};

				size_t ActiveNodesNumber()
				{
					Counter cont;
					this->IterateActiveNodes(cont);
					return cont.GetCount();
				};

				const Functor<dim>& GetFunctor() const
				{
					return *(this->_objective_function);
				};

			protected:
				virtual void DerivedGnuPlotExport(std::ofstream&) = 0;
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
				void Init(unique_ptr<Functor<dim>> f_ptr)
				{
					/* I convert the unique_ptr in a shared_ptr,
					since I need to share it with the BinaryNode elements */
					this->_objective_function = FunctionPtr<dim>(f_ptr.release());
				};

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
#ifdef MYDEBUG
					cerr << "Modifico i valori dell'elemento di id # " << daddy->NodeID() << endl;
#endif //MYDEBUG

					auto val = daddy->PLevel();
					daddy->PLevel(val + 1);

					auto hansel = daddy->Left ();
					auto gretel = daddy->Right();

#ifdef MYDEBUG
					cerr << "Daddy ha q = " << daddy->Q() << endl;
					cerr << "Daddy ha e tilde = " << daddy->TildeError() << endl;
					cerr << "Daddy ha e = " << daddy->ProjectionError() << endl;
					cerr << "Hansel ha q = " << hansel->Q() << endl;
					cerr << "Hansel ha e tilde = " << hansel->TildeError() << endl;
					cerr << "Hansel ha e = " << hansel->ProjectionError() << endl;
					cerr << "Gretel ha q = " << gretel->Q() << endl;
					cerr << "Gretel ha e tilde = " << gretel->TildeError() << endl;
					cerr << "Gretel ha e = " << gretel->ProjectionError() << endl;
#endif //MYDEBUG

					auto new_E = std::min(	hansel->E() + gretel->E(),
													daddy->ProjectionError()
												);

					auto old_E_tilde = daddy->ETilde();
					auto new_E_tilde = new_E * old_E_tilde
											 /
											 (new_E + old_E_tilde);
#ifdef MYDEBUG
					cerr << "E" << endl;
					cerr << "Old value: " << daddy->E() << endl;
#endif //MYDEBUG
					daddy->E(new_E);
#ifdef MYDEBUG
					cerr << "New value: " << new_E << endl;
#endif //MYDEBUG
					daddy->ETilde(new_E_tilde);
#ifdef MYDEBUG
					cerr << "E tilde" << endl;
					cerr << "Old value: " << old_E_tilde << endl;
					cerr << "New value: " << new_E_tilde << endl;
#endif //MYDEBUG

					BinaryNode* alfa_bro(nullptr);

					hansel->Q() > gretel->Q() ? alfa_bro = hansel : alfa_bro = gretel;

					auto new_q = std::min (alfa_bro->Q(), new_E_tilde);
#ifdef MYDEBUG
					cerr << "Il figlio alfa ha q = " << alfa_bro->Q() << endl;
					cerr << "Il nuovo q vale: " << new_q << endl;
#endif //MYDEBUG
					daddy->Q(new_q);

					daddy->S(alfa_bro->S());

					previous_daddy = daddy;									 
					daddy = previous_daddy->Dad();
				} //while(daddy)

				--n_iter;
			} //while(n_iter)

			(this->_godfather).SelectActiveNodes();
			this->_error_updated = false;
		}; //Refine()

	template <std::size_t dim>
	template <std::size_t dummy, typename std::enable_if< (dummy == 1), size_t>::type>
	void MeshRefiner<dim>::ExportGnuPlot(const std::string& script_name)
	{
		std::ofstream output_file(script_name);
		output_file	<< "#It produces the plot of the plevels of the binary refined mesh" << endl
				<< "#call it by typing in gnuplot the instruction:" << endl
				<< "#	call \""
				<< script_name
				<< "\"" << endl;

		this->DerivedGnuPlotExport(output_file);
	};

} //namespace BinaryTree
#endif //__MESH_REFINER_H
