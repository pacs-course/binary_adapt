#ifndef __MESH_REFINER_H
#define __MESH_REFINER_H

#include "BinaryGodFather.h"
#include "MeshRefinerFunctors.h"
#include "BinaryTreeHelper.h"
#include "AbstractFactory.h"
#include "Functor.h"

#include <algorithm> //std::min
#include <string> //std::string
#include <stdexcept> //std::runtime_error
#include <fstream> //std::ofstream

namespace BinaryTree
{

	template <std::size_t dim>
		class MeshRefiner
		{
			public:
				/* constructor */
				MeshRefiner() : _objective_function(nullptr),
									 _godfather(),
									 _global_error(std::numeric_limits<double>::max()),
									 _error_updated(false)
				{};

/*
				functor is the identifier to be passed to FunctionsFactory
				from the resultant object will be initialized the _objective_function attribute
*/
				void Init(std::string, int, char**);

				virtual void LoadMesh(std::string input) = 0;
				virtual void ExportMesh(std::string output) = 0;

/*
				It produces a gnuplot script which plots the mesh with the plevel associated to each element
				The string parameter is the name of the output file
				It is available only for 1D meshes, other dim values have to be implemented
*/
				template <std::size_t dummy = dim, typename std::enable_if< (dummy == 1), size_t>::type = 0>
				void ExportGnuPlot(const std::string&);

/*
				It produces a file with the values of the interpolated function produced by the algorithm
				The first parameter is the name of the output file, the second one the x-sampling length
				It is available only for 1D meshes, other dim values have to be implemented
*/
				template <std::size_t dummy = dim, typename std::enable_if< (dummy == 1), size_t>::type = 0>
				void ExportProjection(const std::string&, double);

/*
				As for the one parameter version, but now x values to be computed are passed to the function
				in a file named points_file
*/
				//TODO
//				template <std::size_t dummy = dim, typename std::enable_if< (dummy == 1), size_t>::type = 0>
//				void ExportProjection(const std::string& output_filename, const std::string& points_file);

				/*	n_iter is the called complexity of the algorithm,
					which corresponds to the number of iterations of the algorithm
					toll is the tolerance on the error
					algorithm stopping criterion:  n > n_iter or err < toll
					the variadic template parameter are functor which are called
					at every iteration of the algorithm;
					Purpose for them being created is to store partial info of the algorithm execution
				*/
				template <typename... Args>
				void Refine(std::size_t n_iter, double toll, Args... funcs);

				/* simply executes the algorithm as described in Binev paper
					just iteration number stopping criterion */
				void Refine(std::size_t n_iter);

				double GlobalError();

				std::vector<size_t> ExtractPLevels();

				size_t ActiveNodesNumber();

				const Functor<dim>& GetFunctor() const;

			protected:
				/*	Called by refine method
					It performs the p refinement along the parents of input parameter
					until an element without father is reached */
				void ClimbUp(BinaryNode* dad);
			protected:
				virtual void DerivedInitialization(int argc, char** argv) = 0;

				/*
					Method to update the _global_error attribute
					It uses the IterateActiveNodes pure virtual method to iterate the mesh
					An ErrorComputer functor is used to sum the local errors on active elements
				*/
				virtual void UpdateGlobalError();

				/* Iterate the activated nodes of the mesh
					for each node n it calls the functor NodeOperator(n);
					using this virtual method to navigate the mesh I'm taking advantage
					from tha fact that the underlying mesh in concrete refiner classes
					implements an iterator structure to iterate only active elements
					This pattern could be used also to implement other kinds of iterator
				*/
				virtual void IterateActiveNodes(NodeOperator&) = 0;
				virtual void IterateActiveNodes(DimOperator<dim>&) = 0;

				virtual void InitializeGodfather() = 0;

			protected:
				void Init(std::unique_ptr<Functor<dim>> f_ptr);

			protected:
/*
	The objective function based on which the interpolation error will be computed during the refinement algorithm
*/
				FunctionPtr<dim> _objective_function;
				DimensionedGodFather<dim> _godfather;
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
		void MeshRefiner<dim>::Init(std::string functor, int argc, char** argv)
		{
			auto& f_factory(FunctionsFactory<dim>::Instance());
			std::unique_ptr<Functor<dim>> f_ptr = f_factory.create(functor);

			Init(move(f_ptr));

			DerivedInitialization(argc, argv);
		};

	template <std::size_t dim>
		void MeshRefiner<dim>::Init(std::unique_ptr<Functor<dim>> f_ptr)
		{
			/* I convert the unique_ptr in a shared_ptr,
			since I need to share it with the BinaryNode elements */
			this->_objective_function = FunctionPtr<dim>(f_ptr.release());
		};

	template <std::size_t dim>
		void MeshRefiner<dim>::UpdateGlobalError()
		{
			ErrorComputer err_cp(this->_global_error);
			err_cp.ResetError();
			IterateActiveNodes(err_cp);
			this->_error_updated = true;
		};


	/* If no variadic argument is passed */
	template <typename... Args>
		void Execute(){};

	/* End of the recursion */
	template <typename T>
		void Execute(T f)
		{
			f();
		};

	/* Function which simply calls in sequence the functors passed by arguments
		It will be called by MeshRefiner<dim>::Refine at each step of the algo */
	template <typename... Args>
		void Execute (std::function<void()> f, Args... funcs)
		{
			f();
			Execute(funcs...);
		};


	template <std::size_t dim>
		template <typename... Args>
		void MeshRefiner<dim>::Refine(std::size_t max_iter, double toll, Args... funcs)
		{
			double total_error = this->GlobalError();
			Execute(funcs...);
			size_t n_iter = 0;
			while (n_iter <= max_iter && total_error > toll)
			{
#ifdef VERBOSE
				std::cout << std::endl << "Complexity : " << n_iter << std::endl;
#endif //VERBOSE
				BinaryNode* daddy = this->_godfather.MakeBisection();
				
				ClimbUp(daddy);

				(this->_godfather).SelectActiveNodes();
				this->_error_updated = false;

				total_error = this->GlobalError();
#ifdef VERBOSE
				std::vector<size_t> p_levels = ExtractPLevels();
				std::cout << ActiveNodesNumber() << " active elements with p refinement levels: " << std::endl;
				for (auto l : p_levels)
					std::cout << l << " ";
				std::cout << std::endl;
				std::cout << "Interpolation error: " << GlobalError() << std::endl;
#endif //VERBOSE

				Execute(funcs...);
				++n_iter;
			}
		};

	template <size_t dim>
		void MeshRefiner<dim>::Refine(std::size_t max_iter)
		{
			std::size_t n_iter = 0;
			while(n_iter <= max_iter)
			{
				BinaryNode* leaf_dad = this->_godfather.MakeBisection();
				
				ClimbUp(leaf_dad);

				++n_iter;
			}

			(this->_godfather).SelectActiveNodes();
			this->_error_updated = false;
		};

	template <size_t dim>
		void MeshRefiner<dim>::ClimbUp(BinaryNode* leaf_dad)
		{
			BinaryNode* daddy = leaf_dad;
			BinaryNode* previous_daddy = nullptr;

			while(daddy)
			{
#ifdef MYDEBUG
				std::clog << "Modifico i valori dell'elemento di id # " << daddy->NodeID() << std::endl;
#endif //MYDEBUG

				auto val = daddy->PLevel();
				daddy->PLevel(val + 1);

				auto hansel = daddy->Left ();
				auto gretel = daddy->Right();

#ifdef MYDEBUG
				std::clog << "Daddy ha q = " << daddy->Q() << std::endl;
				std::clog << "Daddy ha e tilde = " << daddy->TildeError() << std::endl;
				std::clog << "Daddy ha e = " << daddy->ProjectionError() << std::endl;
				std::clog << "Hansel ha q = " << hansel->Q() << std::endl;
				std::clog << "Hansel ha e tilde = " << hansel->TildeError() << std::endl;
				std::clog << "Hansel ha e = " << hansel->ProjectionError() << std::endl;
				std::clog << "Gretel ha q = " << gretel->Q() << std::endl;
				std::clog << "Gretel ha e tilde = " << gretel->TildeError() << std::endl;
				std::clog << "Gretel ha e = " << gretel->ProjectionError() << std::endl;
#endif //MYDEBUG

				auto new_E = std::min(	hansel->E() + gretel->E(),
												daddy->ProjectionError()
											);

				auto old_E_tilde = daddy->ETilde();
				auto new_E_tilde = new_E * old_E_tilde
										 /
										 (new_E + old_E_tilde);
	#ifdef MYDEBUG
				std::clog << "E" << std::endl;
				std::clog << "Old value: " << daddy->E() << std::endl;
	#endif //MYDEBUG
				daddy->E(new_E);
	#ifdef MYDEBUG
				std::clog << "New value: " << new_E << std::endl;
	#endif //MYDEBUG
				daddy->ETilde(new_E_tilde);
	#ifdef MYDEBUG
				std::clog << "E tilde" << std::endl;
				std::clog << "Old value: " << old_E_tilde << std::endl;
				std::clog << "New value: " << new_E_tilde << std::endl;
	#endif //MYDEBUG

				BinaryNode* alfa_bro(nullptr);

				hansel->Q() > gretel->Q() ? alfa_bro = hansel : alfa_bro = gretel;

				auto new_q = std::min (alfa_bro->Q(), new_E_tilde);
	#ifdef MYDEBUG
				std::clog << "Il figlio alfa ha q = " << alfa_bro->Q() << std::endl;
				std::clog << "Il nuovo q vale: " << new_q << std::endl;
	#endif //MYDEBUG
				daddy->Q(new_q);

				daddy->S(alfa_bro->S());

				previous_daddy = daddy;									 
				daddy = previous_daddy->Dad();
			} //while(daddy)
		};

	template <std::size_t dim>
		double MeshRefiner<dim>::GlobalError()
		{
			if (!(this->_error_updated))
				UpdateGlobalError();

			return this->_global_error;
		};

	template <std::size_t dim>
		std::vector<size_t> MeshRefiner<dim>::ExtractPLevels()
		{
			PlevelsExtractor p_ex;
			this->IterateActiveNodes(p_ex);
			return p_ex.GetPLevels();
		};

	template <std::size_t dim>
		size_t MeshRefiner<dim>::ActiveNodesNumber()
		{
			Counter cont;
			this->IterateActiveNodes(cont);
			return cont.GetCount();
		};

	template <std::size_t dim>
		const Functor<dim>& MeshRefiner<dim>::GetFunctor() const
		{
			return *(this->_objective_function);
		};


	template <std::size_t dim>
	template <std::size_t dummy, typename std::enable_if< (dummy == 1), size_t>::type>
		void MeshRefiner<dim>::ExportGnuPlot(const std::string& script_name)
		{
			std::ofstream output_file(script_name);
			output_file	<< "#It produces the plot of the plevels of the binary refined mesh" << std::endl
					<< "#call it by typing in gnuplot the instruction:" << std::endl
					<< "#	call \""
					<< script_name
					<< "\"" << std::endl;

			GnuPlotPrinter printer(output_file);
			IterateActiveNodes(printer);

			double x_min = printer.XMin();
			double x_max = printer.XMax();
			size_t p_max = printer.PMax();

			output_file << "set xrange [" << x_min << ":" << x_max << "]" << std::endl
							<< "set yrange [-1:" << p_max + 1 << "]" << std::endl
							<< "set xlabel \"x\" font \"Helvetica, 22\"" << std::endl
							<< "set ylabel \"p\" font \"Helvetica, 22\"" << std::endl
							<< "set samples 1000" << std::endl
							<< "set xtics nomirror" << std::endl
							<< "set x2tics (";

			auto x_left_vec = printer.XLeftVec();
			for (auto& val : x_left_vec)
				output_file << "\"\" " << val << ", \\" << std::endl;

			output_file << "\"\" " << x_max << ")" << std::endl
							<< "set grid noxtics noytics x2tics" << std::endl
							<< "set tics font \"Helvetica,18\"" << std::endl
							<< "set key font \",22\"" << std::endl
							<< "plot ";

			size_t i = 0;
			Counter c;
			IterateActiveNodes(c);
			for (; i < c.GetCount() - 1; ++i)
				output_file << "p_" << i << "(x) lw 6, \\" << std::endl;
			output_file << "p_" << i << "(x)" << "lw 6" << std::endl;
		};

	template <std::size_t dim>
	template <std::size_t dummy, typename std::enable_if< (dummy == 1), size_t>::type>
		void MeshRefiner<dim>::ExportProjection(const std::string& output_filename, double x_step)
		{
			std::ofstream output_file(output_filename);
			output_file	<< "# Interpolation data for function: " << this->_objective_function->Formula() << std::endl;
			ProjectionPrinter printer(output_file, x_step);
			IterateActiveNodes(printer);
		};

} //namespace BinaryTree
#endif //__MESH_REFINER_H
