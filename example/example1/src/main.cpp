/**
	Example of 2D binary tree adaptation loading everything at runtime.

		 mesh manager library
		 quadrature rule library
		 functors library

	are taken from binary_tree.conf configuration file and loaded at runtime.

	From the same file are taken:

		 the input file storing the mesh to be refined
		 the output file where to store the result
		 the functor to be interpolated
		 the maximum number of iterations of the algorithm
		 the tolerance on the error for the algorithm stopping criterion
**/

#include "MeshRefiner.h"
#include "LibraryInit.h"
#include "BinaryTreeHelper.h" //Cfgfile

#include <iostream>
#include <memory> //std::unique_ptr
#include <utility> //std::move

using namespace std;

void PrintHelp()
{
	cout << "All info are in the README file" << endl;
	cout << "If you don't want to use the default configuration file, "
		 << "manually set it typing:" << endl;
	cout << "	conf_file = path/to/file" << endl;
};

int main (int argc, char** argv)
{
	cerr << "Example started" << endl;
	Helpers::Cfgfile main_input (argc, argv);
	if (main_input.HasHelp())
	{
		PrintHelp();
		exit (0);
	}

	string conf_file = main_input ("conf_file", "../../binary_tree.conf");

	bool failed = BinaryTree::Init (conf_file);
	if (failed)
	{
		cerr << "Library initialization failed" << endl;
		return 1;
	}

	Helpers::Cfgfile cl (conf_file);

	string mesh_refiner = "binary_tree/mesh/mesh_refiner";
	string refiner_key	= cl (mesh_refiner, "");

	auto& mrf (BinaryTree::MeshRefinerFactory<2>::Instance());

	unique_ptr<BinaryTree::MeshRefiner<2>> refiner = nullptr;
	try
	{
		refiner = move (mrf.create (refiner_key));
	}
	catch (exception& ex)
	{
		cerr << "Refiner construction failed" << endl;
		cerr << ex.what() << endl;
		return 1;
	}

	cerr << "Refiner correctly constructed" << endl;

	string func_ID = "binary_tree/functions/functor";
	string func_name = cl (func_ID, "");

	try
	{
		refiner->Init (func_name, argc, argv);
		cerr << "Refiner correctly initialized with " << func_name << endl;
	}
	catch (exception& ex)
	{
		cerr << "Refiner initialization failed" << endl;
		cerr << ex.what() << endl;
		return 1;
	}

	string input_mesh = "binary_tree/algorithm/input_mesh";
	string mesh_filename = cl (input_mesh, "");

	if (mesh_filename == "")
	{
		cerr << "Missing mesh file name in configuration file" << endl;
		return 1;
	}

	try
	{
		refiner->LoadMesh (mesh_filename);
	}
	catch (exception& ex)
	{
		cerr << "Mesh loading from " << mesh_filename << " failed" << endl;
		cerr << ex.what() << endl;
		return 1;
	}

	string iteration_number = "binary_tree/algorithm/iteration_number";
	size_t n_iter = cl (iteration_number, 0);

	string s_tol = "binary_tree/algorithm/tolerance";
	double tol = cl (s_tol, 0.0);

	refiner->Refine (n_iter, tol);

	string output_mesh = "binary_tree/algorithm/output_mesh";
	string output_filename = cl (output_mesh, "");

	if (output_filename == "")
	{
		cerr << "Missing output file name in configuration file" << endl;
		return 1;
	}

	refiner->ExportMesh (output_filename);

	cerr << "Example 1 ended" << endl;

	return 0;
};

