/**
	Example of 2D binary tree adaptation of libMesh mesh.
	From binary_tree.conf configuration file are taken:

		 quadrature rule library
		 functors library
		 the functor to be interpolated
		 the maximum number of iterations of the algorithm
		 the tolerance on the error for the algorithm stopping criterion

	After the creation of libMesh::Mesh object the algorithm is applied,
	then the refining mesh can be used inside libMesh.
	Information about the algorithm flow are extracted during the execution
	and stored togheter with the results in ./results directory.
**/

#include <array>
#include <utility> //std::move
#include <iomanip> //std::setfill, std::setw
#include <sys/stat.h> //mkdir, stat
#include <stdio.h> //remove

#include "LibraryInit.h"
#include "BinaryTreeHelper.h"

#include "LibMeshRefiner.h"
#include "LibMeshBinaryElements.h"

#include "libmesh/libmesh.h"
#include "libmesh/mesh_generation.h" //MeshTools
#include "libmesh/gmsh_io.h"
#include "libmesh/getpot.h"

using namespace std;
using namespace Geometry;
using namespace BinaryTree;
using namespace FiniteElements;
using namespace LibmeshBinary;


void PrintHelp()
{
	cout << "All info are in the README file" << endl;
	cout << "Manually set the configuration file with" << endl;
	cout << "	conf_file = path/to/file" << endl;
};


int main (int argc, char** argv)
{
	cout << endl << "Starting 2D refining complete example" << endl;

	libMesh::LibMeshInit init (argc, argv);

	GetPot main_input (argc, argv);
	if (main_input.search (2, "--help", "-h"))
	{
		PrintHelp();
		exit (0);
	}

	string conf_file = main_input ("conf_file", "../../binary_tree.conf");

	BinaryTree::Init (conf_file);

	unique_ptr<LibmeshBinary::LibmeshRefiner<2>> binary_refiner_ptr (nullptr);
	try
	{
		auto ptr = Helpers::MakeUnique<LibmeshBinary::LibmeshRefiner<2>>();
		binary_refiner_ptr = move (ptr);
	}
	catch (exception& ex)
	{
		cerr << "Refiner construction failed" << endl;
		cerr << ex.what() << endl;
		return 1;
	}

	GetPot cl (conf_file.c_str());

	string func_ID = "binary_tree/functions/functor";
	string func_name = cl (func_ID.c_str(), "");

	try
	{
		binary_refiner_ptr->Init (func_name);
		cerr << "Refiner correctly initialized with " << func_name << endl;
	}
	catch (exception& ex)
	{
		cerr << ex.what() << endl;
		cerr << "Refiner initialization failed" << endl;
		return 1;
	}

	string identifier = binary_refiner_ptr->GetFunctor().ID();
	string function_name = binary_refiner_ptr->GetFunctor().Formula();

	string results_dir = "results/" + identifier;

	struct stat st;

	if (stat (results_dir.c_str(), &st) == -1 &&
		mkdir (results_dir.c_str(), 0777) != 0)
	{
		cerr << "Failed while constructing "
			 << results_dir
			 << " directory"
			 << endl;
		return 1;
	}

	string log_filename = results_dir + "/example.log";

	if (stat (log_filename.c_str(), &st) != -1 &&
		remove (log_filename.c_str()) != 0)
	{
		cerr << "Error removing " << log_filename << endl;
		return 1;
	}

	//I redirect the standard buffers to file
	Helpers::Logfile logfile (log_filename);

	//constructing libmesh mesh
	int nx = 1, ny = 1;
	auto mesh_ptr = make_shared<libMesh::Mesh> (init.comm());
	clog << "Building a 2D mesh with "
		 << 2 * nx* ny
		 << " triangular elements in (0,1)x(0,1)"
		 << endl;

	libMesh::MeshTools::Generation::build_square (*mesh_ptr, nx, ny, 0., 1., 0., 1.,
												  LibmeshTriangleType);

	binary_refiner_ptr->SetMesh (mesh_ptr);

	//I take from getpot configuration file the maximum number of iteration
	string iteration_number = "binary_tree/algorithm/iteration_number";
	size_t max_iter = cl (iteration_number.c_str(), 0);

	//To have 01 instead of 1 and so on, needed to list files in alphabetical order in the results directory
	int digits = max_iter > 0 ? (int) log10 ((double) max_iter) + 1 : 1;

//*INDENT-OFF*
	//captured by reference, it's needed to have different filenames for every iteration of the algorithm
	size_t n_iter = 0;
	function<void()> gmsh_printer =
		[&n_iter, digits, mesh_ptr, results_dir]
		()
		{
			stringstream ss;
			ss << setfill ('0') << setw (digits) << to_string (n_iter);

			string mesh_name = results_dir + "/mesh_" + ss.str() + ".msh";
			libMesh::GmshIO gmsh_io (*mesh_ptr);
			gmsh_io.write (mesh_name);
			++n_iter;
		};

	//captured by reference, it stores the global interpolation error on the refined mesh at every step of the algorithm
	vector<double> error;
	function<void()> error_extractor =
		[&error, &binary_refiner_ptr]
		(void)
		{error.push_back (binary_refiner_ptr->GlobalError());};
//*INDENT-ON*

	//I take from getpot configuration file the tolerance on the error
	string s_tol = "binary_tree/algorithm/tolerance";
	double tol = cl (s_tol.c_str(), 0.0);

	cout << "Starting the algorithm with "
		 << max_iter
		 << " iterations and tolerance of "
		 << tol
		 << endl;

	binary_refiner_ptr->Refine (max_iter,
								tol,
								error_extractor,
								gmsh_printer);

	//I print to file the error data
	string error_name	 = results_dir + "/error_data";
	ofstream error_file (error_name);

	if (!error_file.is_open())
		cerr << "Houston we have a problem! I'm not writing on error file!"
			 << endl;

	error_file << "#Function: " << function_name << endl;
	size_t cont = 0;

	for (auto e : error)
		error_file << cont++ << "	" << e << endl;

	//I make the refined mesh usable in libmesh
	mesh_ptr->prepare_for_use (/*skip_renumber =*/ false);


	//I want last line to be printed to std::cout, so I manually destroy logfile
	logfile.~Logfile();
	cout << endl << "2D refining complete example ended" << endl;
}
