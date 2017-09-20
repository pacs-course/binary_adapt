#include <array>
#include <utility> //std::move

#include "PluginLoader.h"
#include "BinaryTreeHelper.h"

#include "LibMeshRefiner.h"
#include "LibMeshBinaryElements.h"

#include "libmesh/libmesh.h"
#include "libmesh/mesh_generation.h" //MeshTools

#include "getpot.h"

using namespace std;
using namespace Geometry;
using namespace BinaryTree;
using namespace FiniteElements;
using namespace LibmeshBinary;

void PrintHelp()
{
	//TODO
	cout << "All info are in the README file" << endl;
	cout << "Manually set the configuration file with" << endl;
	cout << "	conf_file = path/to/file" << endl;
};


int main(int argc, char** argv)
{
	cout << endl << "Starting 1D refining complete example" << endl;

	libMesh::LibMeshInit init (argc, argv);

	GetPot main_input(argc, argv);
	if (main_input.search(2, "--help", "-h"))
	{
		PrintHelp();
		exit(0);
	}

	string conf_file = main_input("conf_file", "../../binary_tree.conf");

	PluginLoading::PluginLoader pl;
	cerr << "PluginLoader costruito" << endl;

	GetPot cl(conf_file.c_str());
	string quad_library = "binary_tree/quadrature/1D/quad_library";
	string func_library = "binary_tree/functions/func_library";

	string quad_so_file = "lib" + string(cl(quad_library.c_str(), "mesh_quadrature"));
	string func_so_file = "lib" + string(cl(func_library.c_str(), "interpolating_functions"));

#ifdef DEBUG
	quad_so_file += "_Debug";
	func_so_file += "_Debug";
#endif //DEBUG

	quad_so_file += ".so";
	func_so_file += ".so";

	cout << "Il file da caricare e': " << func_so_file << endl;

	pl.Add(quad_so_file);
	pl.Add(func_so_file);

	if (!pl.Load())
	{
		cerr << "Houston we have a problem: something went wrong loading plugins";
		return 1;
	}
	cerr << "Plugins loaded" << endl;

	vector<double> error;
	size_t max_iter = 40;
	for (size_t n_iter = 0; n_iter <= max_iter; ++n_iter)
	{
#ifdef VERBOSE
		cerr << endl << "#DOF = " << n_iter << endl;
#endif //VERBOSE
		auto mesh_ptr = make_shared<libMesh::Mesh> (init.comm());

		int n = 1;
		libMesh::MeshTools::Generation::build_line(*mesh_ptr, n, 0, 1, LibmeshIntervalType);

		unique_ptr<LibmeshBinary::BinaryRefiner<1>> binary_refiner_ptr(nullptr);
		try
		{
			auto ptr = HelperFunctions::MakeUnique<LibmeshBinary::BinaryRefiner<1>>();
			binary_refiner_ptr = move(ptr);
		}
		catch(exception& ex)
		{
			cerr << "Refiner construction failed" << endl;
			cerr << ex.what() << endl;
			return 1;
		}
		cout << "Sono prima della DEFINE" << endl;

		string func_ID = "binary_tree/functions/functor";
		string func_name = cl(func_ID.c_str(), "");

		try
		{
			binary_refiner_ptr->Init(func_name);
			cerr << "Refiner correctly initialized with "<< func_name << endl;
		}
		catch(exception& ex)
		{
			cerr << ex.what() << endl;
			cerr << "Refiner initialization failed" << endl;
			return 1;
		}

		cout << "Cerco l'ID" << endl;
		string identifier = binary_refiner_ptr->GetFunctor().ID();
		cout << "Questo e' il mio id: " << identifier << endl;
		string function_name = binary_refiner_ptr->GetFunctor().Formula();


		binary_refiner_ptr->SetMesh(mesh_ptr);

		binary_refiner_ptr->Refine(n_iter);
	
		auto current_error = binary_refiner_ptr->GlobalError();
		error.push_back(current_error);

#ifdef VERBOSE
		cout << endl << "Interpolation error after refinement >>>>>> " << current_error << endl << endl;;

		vector<size_t> p_levels = binary_refiner_ptr->ExtractPLevels();
		cout << "Livelli di p refinement : " << endl;
		for (auto l : p_levels)
			cout << l << " ";
		cout << endl;
		cout << "The mesh has " << binary_refiner_ptr->ActiveNodesNumber() << " ACTIVE elements" << endl;
#endif //VERBOSE

		if(n_iter == max_iter)
		{
			string dir = "results/";
			string script_name = dir + "p_levels_script_" + identifier;
			string error_name	 = dir + "error_data_" + identifier;
			binary_refiner_ptr->ExportGnuPlot(script_name);
			ofstream error_file(error_name);
			if (!error_file.is_open())
				cerr << "Houston we have a problem! I'm not writing on error file!" << endl;
			error_file << "#Function: " << function_name << endl;
			size_t cont = 0;
			for (auto e : error)
				error_file << cont++ << "	" << e << endl;
		}
	}
	
	cout << "1D refining complete example ended" << endl;
}
