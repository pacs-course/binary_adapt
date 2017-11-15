#include "PluginLoader.h"
#include "MeshRefiner.h"

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

	GetPot cl(conf_file.c_str());
	string quad_library = "binary_tree/quadrature/triangle/quad_library";
	string func_library = "binary_tree/functions/func_library";
	string mesh_library = "binary_tree/mesh/mesh_library";

	string quad_so_file = "lib" + string(cl(quad_library.c_str(), "mesh_quadrature"));
	string func_so_file = "lib" + string(cl(func_library.c_str(), "interpolating_functions"));
	string mesh_so_file = "lib" + string(cl(mesh_library.c_str(), "mesh_library"));

#ifdef DEBUG
	quad_so_file += "_Debug";
	func_so_file += "_Debug";
	mesh_so_file += "_Debug";
#endif //DEBUG
	quad_so_file += ".so";
	func_so_file += ".so";
	mesh_so_file += ".so";


	PluginLoading::PluginLoader pl;
	cerr << "Loader constructed" << endl;

	pl.Add(func_so_file);
	pl.Add(mesh_so_file);
	pl.Add(quad_so_file);

	if (!pl.Load())
	{
		cerr << "Houston we have a problem! Something went wrong loading plugins";
		return 1;
	}
	cerr << "Plugins loaded" << endl;

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

