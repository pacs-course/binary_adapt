#include "Functors.h"

#include "BinaryTreeHelper.h"

/*
	A function which does the registrations in the factory
*/
namespace Functions
{
	__attribute__ ((destructor))
	static void ExitFunction()
	{}

	/**
		Function doing the functor builders registrations at plugin loading.
		Automatically called when the plugin is loaded,
		it makes the registrations of functors builders in
		functor factories.
	**/
	__attribute__ ((constructor))
	static void RegisterFunction()
	{
		auto& f_one_d_factory (BinaryTree::FunctionsFactory<1>::Instance());
		auto& f_two_d_factory (BinaryTree::FunctionsFactory<2>::Instance());

		f_one_d_factory.add ("mu_parser_expr",
							 &Helpers::Builders <ParserFunctor<1>,
												 BinaryTree::Functor<1>
												>::BuildObject);
		f_one_d_factory.add ("x_squared",
							 &Helpers::Builders <XSquared,
												 BinaryTree::Functor<1>
												  >::BuildObject);
		f_one_d_factory.add ("x_exp_beta<40>",
							 &Helpers::Builders <XExpBeta<40>,
												 BinaryTree::Functor<1>
												>::BuildObject);
		f_one_d_factory.add ("sqrt_x",
							 &Helpers::Builders <SqrtX,
												 BinaryTree::Functor<1>
												>::BuildObject);
		f_one_d_factory.add ("half_step",
							 &Helpers::Builders <HalfStep,
												 BinaryTree::Functor<1>
												>::BuildObject);
		f_one_d_factory.add ("half_sqrt_x",
							 &Helpers::Builders <HalfSqrt,
												 BinaryTree::Functor<1>
												>::BuildObject);
		f_one_d_factory.add ("half_x",
							 &Helpers::Builders <HalfX,
												 BinaryTree::Functor<1>
												>::BuildObject);
		f_one_d_factory.add ("half_square",
							 &Helpers::Builders <HalfSquare,
												 BinaryTree::Functor<1>
												>::BuildObject);
		f_one_d_factory.add ("half_twenty",
							 &Helpers::Builders <HalfTwenty,
												 BinaryTree::Functor<1>
												>::BuildObject);
		f_one_d_factory.add ("advection_diffusion_solution<100>",
							 &Helpers::Builders <AdvectionDiffusionSolution<100>,
												 BinaryTree::Functor<1>
												>::BuildObject);
		f_one_d_factory.add ("advection_diffusion_solution<-100>",
							 &Helpers::Builders <AdvectionDiffusionSolution < -100 >,
												 BinaryTree::Functor<1>
												>::BuildObject);

		f_two_d_factory.add ("mu_parser_expr",
							 &Helpers::Builders <ParserFunctor<2>,
												 BinaryTree::Functor<2>
												>::BuildObject);
		f_two_d_factory.add ("x_squared_plus_y_squared",
							 &Helpers::Builders <X2PlusY2,
												 BinaryTree::Functor<2>
												>::BuildObject);
	}
}

