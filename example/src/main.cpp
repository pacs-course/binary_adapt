#include "PluginLoader.h"
#include "MeshRefiner.h"
#include "Functor.h"
#include <iostream>

using namespace std;

int main(int argc, char ** argv)
{
	cerr << "SONO NEL MAIN" << endl;

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

//	cerr << "la factory di refiner contiene : " << endl;
//	vector<string> reg1 = mrf.registered();
//	for (auto i : reg1)
//		cout << i << endl;

	auto refiner = mrf.create("libmesh");
	cerr << "Refiner creato" << endl;
	refiner->Init(nullptr, argc, argv);

//	cerr << "la factory di funzioni contiene : " << endl;
//	vector<string> reg2 = funfac.registered();
//	for (auto i : reg2)
//		cerr << i << endl;
	auto interpolating_function = funfac.create("sqrt_x");
	cerr << "Funzione da interpolare creata" << endl;

	cout << "Example ended" << endl;

	return 0;
};

