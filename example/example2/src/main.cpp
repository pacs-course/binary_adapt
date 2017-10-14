#include <array>
#include <utility> //std::move
#include <cstdlib> //system
#include <iomanip> //std::setfill, std::setw

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

class Logfile
{
	public:
		Logfile(const std::string& filename): _ofs(filename, ios_base::app)
														,_out_buf(cout.rdbuf(_ofs.rdbuf()))
														,_log_buf(clog.rdbuf(_ofs.rdbuf()))
														,_err_buf(cerr.rdbuf(_ofs.rdbuf()))
		{
		#ifdef MYDEBUG
			clog << "I buffer sono su file" << endl;
		#endif //MYDEBUG
		};
		~Logfile()
		{
#ifdef MYDEBUG
			clog << "Redirigo i buffer sullo standard" << endl;
#endif //MYDEBUG
			clog.rdbuf(_log_buf);
			cout.rdbuf(_out_buf);
			cerr.rdbuf(_err_buf);
#ifdef MYDEBUG
			cerr << "Buffer rediretti" << endl;
#endif //MYDEBUG
		};

	private:
		std::ofstream _ofs;
		std::basic_streambuf<char>* _out_buf;
		std::basic_streambuf<char>* _log_buf;
		std::basic_streambuf<char>* _err_buf;
};


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
	cout << "PluginLoader costruito" << endl;

	GetPot cl(conf_file.c_str());
	string quad_library = "binary_tree/quadrature/interval/quad_library";
	string func_library = "binary_tree/functions/func_library";

	string quad_so_file = "lib" + string(cl(quad_library.c_str(), "sandia_quadrature"));
	string func_so_file = "lib" + string(cl(func_library.c_str(), "interpolating_functions"));

#ifdef DEBUG
	quad_so_file += "_Debug";
	func_so_file += "_Debug";
#endif //DEBUG

	quad_so_file += ".so";
	func_so_file += ".so";

	pl.Add(quad_so_file);
	pl.Add(func_so_file);

	if (!pl.Load())
	{
		cerr << "Houston we have a problem: something went wrong loading plugins";
		return 1;
	}
	cout << "Plugins loaded" << endl;

	string iteration_number = "binary_tree/algorithm/iteration_number";
	size_t max_iter = cl(iteration_number.c_str(), 0);
	string s_toll = "binary_tree/algorithm/tolerance";
	double toll = cl(s_toll.c_str(), 0.0);
	cout << "Starting the algorithm with " << max_iter << " iterations and tolerance of " << toll << endl;


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

	string func_ID = "binary_tree/functions/functor";
	string func_name = cl(func_ID.c_str(), "");

	try
	{
		binary_refiner_ptr->Init(func_name);
		cout << "Refiner correctly initialized with "<< func_name << endl;
	}
	catch(exception& ex)
	{
		cerr << ex.what() << endl;
		cerr << "Refiner initialization failed" << endl;
		return 1;
	}

	string identifier = binary_refiner_ptr->GetFunctor().ID();
	string function_name = binary_refiner_ptr->GetFunctor().Formula();

	string results_dir = "results/" + identifier;
	string instruction = "mkdir -p " + results_dir;
	if (system (instruction.c_str()))
		return 1;

	string log_filename = results_dir + "/example.log";
	string remove_instruction = "rm -f " + log_filename;
	if (system (remove_instruction.c_str()))
		return 1;

	Logfile logfile(log_filename);

	auto mesh_ptr = make_shared<libMesh::Mesh> (init.comm());

	int n = 1;
	double x_min = 0;
	double x_max = 1;
	libMesh::MeshTools::Generation::build_line(*mesh_ptr, n, x_min, x_max, LibmeshIntervalType);

	binary_refiner_ptr->SetMesh(mesh_ptr);

	function<void()> p_levels_printer = [&binary_refiner_ptr]()
	{
#ifdef VERBOSE
		vector<size_t> p_levels = binary_refiner_ptr->ExtractPLevels();
		cout << "Livelli di p refinement : " << endl;
		for (auto l : p_levels)
			cout << l << " ";
		cout << endl;
		cout << "The mesh has " << binary_refiner_ptr->ActiveNodesNumber() << " ACTIVE elements" << endl;

		cout << endl << "Interpolation error after refinement: " << binary_refiner_ptr->GlobalError() << endl;
#endif //VERBOSE
	};

	int digits = max_iter > 0 ? (int) log10 ((double) max_iter) + 1 : 1;
	size_t n_iter = 0;
	function<void()> gnuplot_printer = [&n_iter, digits, &binary_refiner_ptr, results_dir]()
	{
		stringstream ss;
		ss << setfill('0') << setw(digits) << to_string(n_iter);
		string p_levels_script_name = results_dir + "/p_levels_script_" + ss.str();
		binary_refiner_ptr->ExportGnuPlot(p_levels_script_name);
		++n_iter;
	};

	size_t m_iter = 0;
	function<void()> projection_printer = [x_max, x_min, results_dir, digits, &m_iter, &binary_refiner_ptr]()
	{
		stringstream ss;
		ss << setfill('0') << setw(digits) << to_string(m_iter);
		size_t n_points = 1000;
		double step = (x_max - x_min) / n_points;
		string projected_fun_name = results_dir + "/projection_data_" + ss.str();
		binary_refiner_ptr->ExportProjection(projected_fun_name, step);
		++m_iter;
	};

	vector<double> error;
	function<void()> error_extractor =	[&error, &binary_refiner_ptr]
													(void)
													{error.push_back(binary_refiner_ptr->GlobalError());};
	binary_refiner_ptr->Refine(max_iter,
										toll,
										error_extractor,
										gnuplot_printer,
										projection_printer,
										p_levels_printer);

	string error_name	 = results_dir + "/error_data";
	ofstream error_file(error_name);
	if (!error_file.is_open())
		cerr << "Houston we have a problem! I'm not writing on error file!" << endl;
	error_file << "#Function: " << function_name << endl;
	size_t cont = 0;
	for (auto e : error)
		error_file << cont++ << "	" << e << endl;

	logfile.~Logfile();
	cout << endl << "1D refining complete example ended" << endl;
}
