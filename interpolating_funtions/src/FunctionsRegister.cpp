#include "FunctionsRegister.h"
#include "MyFunctors.h"

#include "BinaryTreeHelper.h"

namespace MyFunctions
{
	FunctionsRegister::FunctionsRegister()
	{
		auto& f_one_d_factory(BinaryTree::FunctionsFactory<1>::Instance());
		auto& f_two_d_factory(BinaryTree::FunctionsFactory<2>::Instance());

#ifdef MYDEBUG
		cerr << "Indirizzo della factory 1D di funzioni : " << &f_one_d_factory << endl;
		cerr << "Indirizzo della factory 2D di funzioni : " << &f_two_d_factory << endl;
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

	FunctionsRegister& FunctionsRegister::Instance()
	{
		static FunctionsRegister fr;
		return fr;
	};

} //namespace MyFunctions

/*
	I instantiate a global variable which does the registration when the library is loaded
	I use the unnamed namespace to avoid conflicts
	This does not cause any problem since this variable is not needed anywhere else 
*/
namespace
{
	MyFunctions::FunctionsRegister& fr(MyFunctions::FunctionsRegister::Instance());
}

