#include "LibMeshRefinerRegister.h"
#include "LibMeshRefiner.h"

#include "BinaryTreeHelper.h"

using namespace std;

namespace LibmeshBinary
{
	RefinerRegister::RefinerRegister()
	{
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

	RefinerRegister& RefinerRegister::Instance()
	{
		static RefinerRegister rr;
		return rr;
	};

} //namespace LibmeshBinary

/*
	I instantiate a global variable which does the registration
	when the library is loaded
	I use the unnamed namespace to avoid conflicts
	This does not cause any problem since this variable is not needed anywhere else 
*/
namespace
{
	LibmeshBinary::RefinerRegister& rr(LibmeshBinary::RefinerRegister::Instance());
}
