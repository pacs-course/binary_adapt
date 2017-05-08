#include "MyFunctors.h"

#include "BinaryTreeHelper.h"

/*
	A function which does the registrations in the factory
*/
namespace MyFunctions
{
	__attribute__((destructor))
	static void ExitFunction()
	{
		clog << "I'm closing the functions library" << endl;
	};

	__attribute__((constructor))
		static void RegisterFunction()
		{
			clog << "Registering in Functor factory" << endl;

			auto& f_one_d_factory(BinaryTree::FunctionsFactory<1>::Instance());
//			auto& f_two_d_factory(BinaryTree::FunctionsFactory<2>::Instance());

#ifdef MYDEBUG
			cerr << "Indirizzo della factory 1D di funzioni : " << &f_one_d_factory << endl;
//			cerr << "Indirizzo della factory 2D di funzioni : " << &f_two_d_factory << endl;
#endif //MYDEBUG

			f_one_d_factory.add ("x_squared",
										&HelperFunctions::Builders <XSquared,
																			 BinaryTree::Functor<1>
																			>::BuildObject);
			f_one_d_factory.add ("sqrt_x",
										&HelperFunctions::Builders <SqrtX,
																			 BinaryTree::Functor<1>
																			>::BuildObject);
		};
}

