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
/**
				constructor.
				Default constructor, the initialization of the attribute must be done through the Init method
**/
				MeshRefiner() : _objective_function(nullptr),
									 _godfather(),
									 _global_error(std::numeric_limits<double>::max()),
									 _error_updated(false)
				{};

/**
				destructor.
**/
				virtual ~MeshRefiner(){};

/**
				Initialize the element.
				To be called after the construction.
				Init method performs the search of the identifier in the FunctionsFactory,
				whose output is passed to the protected overload of the Init method;
				then the pure virtual DerivedInitialization method is called.
				First string parameter is the identifier of the functor to be projected;
				the main parameters are passed as input, in case the DerivedInitialization method needs them.

**/
				void Init(std::string, int, char**);

/**
				Load mesh from file.
				It depends on the underlying mesh implementation, defined in derived classes.
**/
				virtual void LoadMesh(std::string input) = 0;
/**
				Export refined mesh to file.
				It depends on the underlying mesh implementation, defined in derived classes.
**/
				virtual void ExportMesh(std::string output) = 0;

/**
				It produces a gnuplot script which plots the mesh with the p level associated to each element.
				The string parameter is the name of the output file.
				It is available only for 1D meshes, other dim values have to be implemented.
**/
				template <std::size_t dummy = dim, typename std::enable_if< (dummy == 1), size_t>::type = 0>
				void ExportGnuPlot(const std::string&);

/**
				It produces a file with the values of the interpolated function.
				Method needed to export the function projected over the underlying mesh.
				The first parameter is the name of the output file, the second one the x-sampling length.
				It is available only for 1D meshes, other dim values have to be implemented
**/
				template <std::size_t dummy = dim, typename std::enable_if< (dummy == 1), size_t>::type = 0>
				void ExportProjection(const std::string&, double);

/*
				As for the one parameter version, but now x values to be computed are passed to the function
				in a file named points_file
*/
				//TODO
//				template <std::size_t dummy = dim, typename std::enable_if< (dummy == 1), size_t>::type = 0>
//				void ExportProjection(const std::string& output_filename, const std::string& points_file);

/**
				Method implementing the refining algorithm.
				n_iter is the called complexity of the algorithm,
				which corresponds to the number of iterations of the algorithm;
				toll is the tolerance on the error;
				algorithm stopping criterion:  n > n_iter or err < toll;
				the variadic template parameters are functors which are called
				at every iteration of the algorithm;
				Purpose for them being created is to store partial info of the algorithm execution
**/
				template <typename... Args>
				void Refine(std::size_t n_iter, double toll, Args... funcs);

/**
				As in the template version, but it simply executes the algorithm as described in Binev paper.
				the only stopping criterion is on the iteration number;
				it is more efficient than the templated version, since it does not store partial info,
				so some computation can be avoided during the execution of the algorithm,
				i.e. trim of the tree at every iteration is not needed in this case
**/
				void Refine(std::size_t n_iter);

/**
				Extract the projection error on refined mesh.
				It returns the projection error in L2 norm of the _objective_function attribute over the underlying mesh.
				The error is computed as the sum of projection errors over any active element.
**/
				double GlobalError();

/**
				Extract the values of the p levels of active elements.
**/
				std::vector<size_t> ExtractPLevels();

/**
				The number of active elements.
**/
				size_t ActiveNodesNumber();

/**
				Reference to _objective_function attribute.
**/
				const Functor<dim>& GetFunctor() const;

/**
				Iterate on the activated nodes of the mesh.
				For each node n it calls the functor NodeOperator(n);
				using this virtual method to navigate the mesh I'm taking advantage
				from the fact that the underlying mesh in concrete refiner classes
				implements an iterator structure to iterate only active elements.
				This pattern could be used also to implement other kinds of iterator.
				The iteration is defined in protected pure virtual IterateActive method,
				which will be implemented in concrete derived classes.
**/
				void IterateActiveNodes(NodeOperator& func)
				{
					IterateActive(func);
					this->_error_updated = false;
				};
/**
				Overloaded method for DimOperator<dim> input.
				It has the same behaviour of the NodeOperator input method.
**/
				void IterateActiveNodes(DimOperator<dim>& func)
				{
					IterateActive(func);
					this->_error_updated = false;
				};

/**
				const version of IterateActiveNodes.
				In the const version I don't need to update the error since I'm sure
				that the functor has not modified the mesh.
**/
				virtual void IterateActiveNodes(NodeOperator&) const = 0;
/**
				Overloaded method for DimOperator<dim> input.
				It has the same behaviour of the NodeOperator input const method.
**/
				virtual void IterateActiveNodes(DimOperator<dim>&) const = 0;

			protected:
				virtual void IterateActive(NodeOperator&) = 0;
				virtual void IterateActive(DimOperator<dim>&) = 0;

/**
				Needed by Refine method.
				It performs the p refinement along the parents of input parameter
				until an element without father is reached
**/
				void ClimbUp(BinaryNode* dad);

			protected:
/**
				Called by Init method.
				It implements the initialization needed by classes derived from this one. 
**/
				virtual void DerivedInitialization(int argc, char** argv) = 0;

/**
				Method to update the _global_error attribute.
				It uses the IterateActiveNodes pure virtual method to iterate the mesh.
				An ErrorComputer functor is used to sum the local errors on active elements.
**/
				virtual void UpdateGlobalError();

/**
				Initialize the _godfather attribute.
				Implemented in derived classes, it sets the _godfather attribute as the father of every element
				of the underlying mesh to be refined. 
**/
				virtual void InitializeGodfather() = 0;

			protected:
/**
				It initialize the _objective_function attribute
**/
				void Init(std::unique_ptr<Functor<dim>> f_ptr);

			protected:
/**
				The objective function based on which the interpolation error will be computed during the refinement algorithm.
**/
				FunctionPtr<dim> _objective_function;
/**
				
**/
				DimensionedGodFather<dim> _godfather;

				double _global_error;
				bool _error_updated;
		};

/**
	Factory for MeshRefiner elements.
	The key is a string which identifies the underlying library of the concrete object.
	The registration is done at loading time by the dynamic library implementing concrete object.
**/
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
