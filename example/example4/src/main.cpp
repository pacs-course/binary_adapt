#include <array>
#include <utility> //std::move

#ifdef DEPRECATED
#include <cstdlib> //system
#else //DEPRECATED
#include <sys/stat.h> //mkdir, stat
#include <stdio.h> //remove
#endif //DEPRECATED

#include "LibraryInit.h"
#include "BinaryTreeHelper.h"

#include "LibMeshRefiner.h"
#include "LibMeshBinaryElements.h"

#include "libmesh/libmesh.h"
#include "libmesh/mesh_generation.h" //MeshTools


using namespace std;
using namespace Geometry;
using namespace BinaryTree;
using namespace FiniteElements;
using namespace LibmeshBinary;

/**
	This is the result expected from the algorithm:
	p_0(x) = x > 0.500000 && x <= 1.000000? 5 : 1/0
	p_1(x) = x > 0.250000 && x <= 0.500000? 4 : 1/0
	p_2(x) = x > 0.125000 && x <= 0.250000? 4 : 1/0
	p_3(x) = x > 0.062500 && x <= 0.125000? 3 : 1/0
	p_4(x) = x > 0.031250 && x <= 0.062500? 3 : 1/0
	p_5(x) = x > 0.000000 && x <= 0.015625? 13 : 1/0
	p_6(x) = x > 0.015625 && x <= 0.031250? 3 : 1/0

	I want to pass from these p levels:
		5 4 4 3 3 13 3
	to
		8 7 7 4 4 1  4
		
**/
//TODO:	make the example more general, if anything changes in the algo result
//			this example becomes meaningless
class ManualAdapter : public NodeOperator
{
	public:
		/** constructor **/
		ManualAdapter() : p_levels({8, 7, 7, 4, 4, 1, 4}), index(0){};
		/** destructor **/
		~ManualAdapter(){};

		virtual void operator()(BinaryNode* node) override
		{
			auto p = p_levels[index++];
			node->PLevel(p);
		};
	protected:
		vector<double> p_levels;
		size_t index;
};

/**
	Comparison between algorithm and manually adapted mesh.
	This example compares the projection errors for sqrt(x) function computed over:
		- the resulting mesh of the binary tree algorithm (see example2)
		- a manually adapted mesh (with the same complexity) which,
		  from the theory ["p- and hp- Finite Element Methods", Ch. Schwab]
		  is supposed to be nearer to the optimal option
**/
int main(int argc, char** argv)
{
	cout << endl << "Starting sqrt comparison example" << endl;

	libMesh::LibMeshInit init (argc, argv);

	BinaryTree::Init();

	size_t max_iter = 40;

	cout << "Starting the algorithm with " << max_iter << " iterations" << endl;


	unique_ptr<LibmeshBinary::LibmeshRefiner<1>> binary_refiner_ptr(nullptr);
	try
	{
		auto ptr = Helpers::MakeUnique<LibmeshBinary::LibmeshRefiner<1>>();
		binary_refiner_ptr = move(ptr);
	}
	catch(exception& ex)
	{
		cerr << "Refiner construction failed" << endl;
		cerr << ex.what() << endl;
		return 1;
	}

	string func_name = "sqrt_x";

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

	string results_dir = "results";
#ifdef DEPRECATED
	string instruction = "mkdir -p " + results_dir;
	if ( system(instruction.c_str()) )
		return 1;
#else //DEPRECATED
	struct stat st;

	if (stat(results_dir.c_str(), &st) == -1 && mkdir(results_dir.c_str(), 0777) != 0)
	{
		cerr << "Failed while constructing " << results_dir << " directory" << endl;
		return 1;
	}
#endif //DEPRECATED

	string log_filename = results_dir + "/example.log";

#ifdef DEPRECATED
	string remove_instruction = "rm -f " + log_filename;
	if ( system(remove_instruction.c_str()) )
		return 1;
#else //DEPRECATED
	if (stat(log_filename.c_str(), &st) != -1 && remove(log_filename.c_str()) != 0)
	{
		cerr << "Error removing " << log_filename << endl;
		return 1;
	}
#endif //DEPRECATED

	//I redirect the standard buffers to file
	Helpers::Logfile logfile(log_filename);

	auto mesh_ptr = make_shared<libMesh::Mesh> (init.comm());

	int n = 1;
	double x_min = 0;
	double x_max = 1;
	libMesh::MeshTools::Generation::build_line(*mesh_ptr, n, x_min, x_max, LibmeshIntervalType);

	binary_refiner_ptr->SetMesh(mesh_ptr);

	binary_refiner_ptr->Refine(max_iter);

	double algo_error = binary_refiner_ptr->GlobalError();

	string p_levels_script_name = results_dir + "/p_levels_script_algo";
	binary_refiner_ptr->ExportGnuPlot(p_levels_script_name);

	size_t n_points = 1000;
	double step = (x_max - x_min) / n_points;
	string projected_fun_name = results_dir + "/projection_data_algo";
	binary_refiner_ptr->ExportProjection(projected_fun_name, step);

	clog << "Binary tree algorithm successfully executed" << endl;

	/* now I manually construct the mesh to be compared with the previously computed one
		I keep the h adaptation unchanged, while I modify the p levels:
		I want the minimum p level near the singularity in 0,
		so I redistribute those degrees of freedom among the other elements, increasing their p level */

	ManualAdapter adapter;
	clog << "ManualAdapter constructed" << endl;
	binary_refiner_ptr->IterateActiveNodes(adapter);

	double manual_error = binary_refiner_ptr->GlobalError();

	string manual_p_levels_script_name = results_dir + "/p_levels_script_manual";
	binary_refiner_ptr->ExportGnuPlot(manual_p_levels_script_name);

	string manual_projected_fun_name = results_dir + "/projection_data_manual";
	binary_refiner_ptr->ExportProjection(manual_projected_fun_name, step);

	logfile.~Logfile();
	cout << "Projection error on the resulting mesh" << endl;
	cout << "E = " << algo_error << endl;

	cout << "Projection error on the manual adapted mesh" << endl;
	cout << "e = " << manual_error << endl;

	cout << "The difference is: " << endl;
	cout << "E - e = " << algo_error - manual_error << endl;

	//I make the refined mesh usable in libmesh
	mesh_ptr->prepare_for_use(/*skip_renumber =*/ false);


	cout << endl << "sqrt comparison example ended" << endl;
}
