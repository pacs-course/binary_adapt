#include "MeshRefiner.h"
#include "LibraryInit.h"

#include <iostream>
#include <memory> //std::unique_ptr
#include <utility> //std::move

#ifdef LIBMESH_BUG_FIXED
#include <GetPot>
#else
#include "getpot.h"
#endif

using namespace std;

void PrintHelp()
{
	//TODO
	cout << "All info are in the README file" << endl;
	cout << "If you don't want to use the default configuration file, " <<
			  "manually set it typing:" << endl;
	cout << "	conf_file = path/to/file" << endl;
};

int main(int argc, char ** argv)
{
	cerr << "Example started" << endl;
	GetPot main_input(argc, argv);
	if (main_input.search(2, "--help", "-h"))
	{
		PrintHelp();
		exit(0);
	}

	string conf_file = main_input("conf_file", "../../binary_tree.conf");

	bool failed = BinaryTree::Init(conf_file);
	if (failed)
	{
		cerr << "Library initialization failed" << endl;
		return 1;
	}

	auto& mrf(BinaryTree::MeshRefinerFactory<2>::Instance());

	unique_ptr<BinaryTree::MeshRefiner<2>> refiner = nullptr;
	try
	{
		refiner = move(mrf.create("libmesh"));
	}
	catch(exception& ex)
	{
		cerr << "Refiner construction failed" << endl;
		cerr << ex.what() << endl;
		return 1;
	}

	cerr << "Refiner correctly constructed" << endl;

	GetPot cl(conf_file.c_str());

	string func_ID = "binary_tree/functions/functor";
	string func_name = cl(func_ID.c_str(), "");

	try
	{
		refiner->Init(func_name, argc, argv);
		cerr << "Refiner correctly initialized with "<< func_name << endl;
	}
	catch(exception& ex)
	{
		cerr << ex.what() << endl;
		cerr << "Refiner initialization failed" << endl;
		return 1;
	}


	string input_mesh = "binary_tree/algorithm/input_mesh";
	string mesh_filename = cl(input_mesh.c_str(), "");
	if (mesh_filename == "")
	{
		cerr << "Missing mesh file name in configuration file" << endl;
		return 1;
	}
	refiner->LoadMesh(mesh_filename);

	string iteration_number = "binary_tree/algorithm/iteration_number";
	size_t n_iter = cl(iteration_number.c_str(), 0);

	string s_toll = "binary_tree/algorithm/tolerance";
	double toll = cl(s_toll.c_str(), 0.0);

	refiner->Refine(n_iter, toll);

	string output_mesh = "binary_tree/algorithm/output_mesh";
	string output_filename = cl(output_mesh.c_str(), "");

	refiner->ExportMesh(output_filename);

	cerr << "Example 1 ended" << endl;

	return 0;
};

