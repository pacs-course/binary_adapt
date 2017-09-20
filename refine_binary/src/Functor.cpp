#include "Functor.h"

namespace Banana
{
	void TestFun()
	{
		auto& funfac(BinaryTree::FunctionsFactory<1>::Instance());
		cerr << "Sono in testfun" << endl;
		cerr << "Indirizzo della factory 1D di funzioni : " << &funfac << endl;
	};
}

