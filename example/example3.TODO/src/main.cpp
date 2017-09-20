#include <array>
#include <utility> //std::move

#include "PluginLoader.h"
#include "BinaryTreeHelper.h"

#ifdef DEPRECATED
#include "LibMeshQuadratureRegister.h" //LibmeshBananaFun
#include "SandiaQuadratureRegister.h" //SandiaBananaFun
#endif //DEPRECATED

#include "LibMeshRefiner.h"
#include "LibMeshBinaryElements.h"

#include "MyFunctors.h"

#include "libmesh/libmesh.h"
#include "libmesh/mesh_generation.h" //MeshTools
#include "libmesh/getpot.h" //Getpot
//#include <quadrature.h>

using namespace std;
using namespace Geometry;
using namespace BinaryTree;
using namespace FiniteElements;
using namespace LibmeshBinary;

int main(int argc, char** argv)
{
	cout << endl << "Starting 1D refining complete example" << endl;

#ifndef DEPRECATED

	PluginLoading::PluginLoader pl;
	cerr << "PluginLoader costruito" << endl;
#ifndef DEBUG
	pl.Add("libmesh_quadrature.so");
	pl.Add("libsandia_quadrature.so");
#else //DEBUG
	pl.Add("libmesh_quadrature_Debug.so");
	pl.Add("libsandia_quadrature_Debug.so");
#endif //DEBUG
	if (!pl.Load())
	{
		cerr << "Houston we have a problem: something went wrong loading plugins";
		return 1;
	}

#else //DEPRECATED

	Banana::LibmeshBananaFun();
//#define NOT_SANDIA
#ifndef NOT_SANDIA
	Banana::SandiaBananaFun();
#endif //NOT_SANDIA

#endif //DEPRECATED

	libMesh::LibMeshInit init (argc, argv);
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
		auto binary_refiner_ptr = HelperFunctions::MakeUnique<LibmeshBinary::BinaryRefiner<1>>();

//		auto f_ptr = HelperFunctions::MakeUnique<MyFunctions::XSquared>();
//		auto f_ptr = HelperFunctions::MakeUnique<MyFunctions::XExpBeta<40>>();
//		auto f_ptr = HelperFunctions::MakeUnique<MyFunctions::SqrtX>();
//		auto f_ptr = HelperFunctions::MakeUnique<MyFunctions::HalfStep>();
//		auto f_ptr = HelperFunctions::MakeUnique<MyFunctions::HalfSqrt>();
//		auto f_ptr = HelperFunctions::MakeUnique<MyFunctions::HalfX>();
//		auto f_ptr = HelperFunctions::MakeUnique<MyFunctions::HalfSquare>();
//		auto f_ptr = HelperFunctions::MakeUnique<MyFunctions::HalfTwenty>();
		auto f_ptr = HelperFunctions::MakeUnique<MyFunctions::AdvectionDiffusionSolution<100>>();
		string function_name = f_ptr->Name();

		binary_refiner_ptr->Init(move(f_ptr));
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
			string script_name= "results/" + "p_levels_script_" + function_name;
			string error_name	= "results/" + "error_data_" + function_name;
			binary_refiner_ptr->ExportGnuPlot(script_name);
			ofstream error_file(error_name);
			error_file << "#Function: " << function_name << endl;
			size_t cont = 0;
			for (auto e : error)
				error_file << cont++ << "	" << e << endl;
		}
	}
	
	cout << "1D refining complete example ended" << endl;
}
