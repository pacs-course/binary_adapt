#include "Examples.h"
#include "PluginLoader.h"
#include "MeshRefiner.h"
#include "Functor.h"

#include <iostream>
#include <memory> //std::unique_ptr
#include <utility> //std::move

using namespace std;

void Example1(int argc, char** argv)
{
	cerr << "Example started" << endl;

	auto& mrf(BinaryTree::MeshRefinerFactory<1>::Instance());
//	cerr << "Indirizzo della factory 1D di refiner : " << &mrf << endl;

	auto& mrf2(BinaryTree::MeshRefinerFactory<2>::Instance());
//	cerr << "Indirizzo della factory 2D di refiner : " << &mrf2 << endl;

	auto& funfac(BinaryTree::FunctionsFactory<1>::Instance());
//	cerr << "Indirizzo della factory 1D di funzioni : " << &funfac << endl;

	PluginLoading::PluginLoader pl;
//	cerr << "Loader creato" << endl;
#ifndef DEBUG
	pl.Add("libmy_functions.so");
	pl.Add("libmesh_bridge.so");
	pl.Add("libmesh_quadrature.so");
#else //DEBUG
	pl.Add("libmy_functions_Debug.so");
	pl.Add("libmesh_bridge_Debug.so");
	pl.Add("libmesh_quadrature_Debug.so");
#endif //DEBUG

	if (!pl.Load())
	{
		cerr << "Houston we have a problem: something went wrong loading plugins";
		return;
	}
//	cerr << "Plugin caricati" << endl;

	unique_ptr<BinaryTree::MeshRefiner<1>> refiner = mrf.create("libmesh");
//	cerr << "Refiner creato" << endl;

	refiner->Init("sqrt_x", argc, argv);
//	cerr << "Refiner inizializzato con sqrt_x" << endl;

//	refiner->LoadMesh("mesh_files/example_1.TODO");
	cerr << "Example ended" << endl;

};
