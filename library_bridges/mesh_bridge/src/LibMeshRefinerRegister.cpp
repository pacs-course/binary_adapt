#include "LibMeshRefiner.h"

#include "BinaryTreeHelper.h"

using namespace std;

/*
	A function which does the registration when the library is loaded
*/
namespace LibmeshBinary
{
	__attribute__((destructor))
	static void ExitFunction()
	{
#ifdef MYDEBUG
		clog << "I'm closing the libmesh refiner library" << endl;
#endif //MYDEBUG
	};

	__attribute__((constructor))
		static void RegisterFunction()
		{
#ifdef MYDEBUG
			clog << "Registering in Refiner factory" << endl;
#endif //MYDEBUG

			auto& r_one_d_factory (BinaryTree::MeshRefinerFactory<1>::Instance());
			auto& r_two_d_factory (BinaryTree::MeshRefinerFactory<2>::Instance());

#ifdef MYDEBUG
			cerr << "Indirizzo della factory 1D di refiner : " << &r_one_d_factory << endl;
			cerr << "Indirizzo della factory 2D di refiner : " << &r_two_d_factory << endl;
#endif //MYDEBUG

			r_one_d_factory.add ("libmesh",
										&HelperFunctions::Builders <BinaryRefiner<1>,
																			 BinaryTree::MeshRefiner<1>
																			>::BuildObject);

			r_two_d_factory.add ("libmesh",
										&HelperFunctions::Builders <BinaryRefiner<2>,
																			 BinaryTree::MeshRefiner<2>
																			>::BuildObject);
		};
}

