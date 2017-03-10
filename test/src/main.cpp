#include "Test.h"
#include "PluginLoader.h"

#include "LibMeshQuadratureRegister.h" //QuadratureBananaFun
//#include "LibMeshRefinerRegister.h" //RefinerBananaFun

#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
	cerr << "Sono nel main" << endl;
//	PluginLoading::PluginLoader pl;
//	cerr << "PluginLoader costruito" << endl;
//#ifndef DEBUG
//	pl.Add("libmy_functions.so");
//	pl.Add("libmesh_bridge.so");
//	pl.Add("libmesh_quadrature.so");
//#else //DEBUG
//	pl.Add("libmy_functions_Debug.so");
//	pl.Add("libmesh_bridge_Debug.so");
//	pl.Add("libmesh_quadrature_Debug.so");
//#endif //DEBUG
//	if (!pl.Load())
//	{
//		cerr << "Houston we have a problem: something went wrong loading plugins";
//		return 1;
//	}

	Banana::QuadratureBananaFun();

//	test1	(argc, argv);

/* useless
	test2	(argc, argv);
	test3	(argc, argv);
*/

//	test4	(argc, argv);
//	test5	(argc, argv);
//	test6	(argc, argv);
	test7	(argc, argv);
//	test8	(argc, argv);
//	test9	(argc, argv);
//	test10(argc, argv);
//	test11(argc, argv);
//	test12(argc, argv);

//	test13(argc, argv); //fallisce

//	test14(argc, argv);

//	test15(argc, argv); //fallisce

	cout << "Tests completed" << endl;

	return 0;
}
