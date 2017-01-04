#include "PluginLoader.h"
#include "MeshRefiner.h"
#include "Functor.h"

#include <iostream>
#include <memory> //std::unique_ptr
#include <utility> //std::move

using namespace std;

int main(int argc, char ** argv)
{
	cerr << "Example started" << endl;

	auto& mrf(BinaryTree::MeshRefinerFactory<1>::Instance());
	cerr << "Indirizzo della factory 1D di refiner : " << &mrf << endl;

	auto& mrf2(BinaryTree::MeshRefinerFactory<2>::Instance());
	cerr << "Indirizzo della factory 2D di refiner : " << &mrf2 << endl;

	auto& funfac(BinaryTree::FunctionsFactory<1>::Instance());
	cerr << "Indirizzo della factory 1D di funzioni : " << &funfac << endl;

	PluginLoading::PluginLoader pl("../lib");
	cerr << "Loader creato" << endl;
	pl.Add("libmy_functions.so");
	pl.Add("libmesh_bridge.so");
	pl.Load();
	cerr << "Plugin caricati" << endl;

	unique_ptr<BinaryTree::MeshRefiner<1>> refiner = mrf.create("libmesh");
	cerr << "Refiner creato" << endl;

	refiner->Init("sqrt_x", argc, argv);
	cerr << "Refiner inizializzato con sqrt_x" << endl;

//	refiner->LoadMesh("mesh_files/example_1.TODO");
	cerr << "Example ended" << endl;

	return 0;
};

