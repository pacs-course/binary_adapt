#include "LibMeshRefiner.h"

#include "BinaryTreeHelper.h"

using namespace std;

namespace LibmeshBinary
{
	__attribute__((destructor))
	static void ExitFunction()
	{};

/**
	Function doing LibMeshRefiner builder registration at plugin loading.
	Automatically called when the plugin is loaded,
	it makes the registration of LibMeshRefiner builders in
	refiner factories.
**/
	__attribute__((constructor))
		static void RegisterFunction()
		{
			auto& r_one_d_factory (BinaryTree::MeshRefinerFactory<1>::Instance());
			auto& r_two_d_factory (BinaryTree::MeshRefinerFactory<2>::Instance());

			r_one_d_factory.add ("libmesh",
										&Helpers::Builders <LibmeshRefiner<1>,
																			 BinaryTree::MeshRefiner<1>
																			>::BuildObject);

			r_two_d_factory.add ("libmesh",
										&Helpers::Builders <LibmeshRefiner<2>,
																			 BinaryTree::MeshRefiner<2>
																			>::BuildObject);
		};
} //namespace LibmeshBinary
