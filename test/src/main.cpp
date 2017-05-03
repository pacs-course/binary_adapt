#include "Test.h"
#include "PluginLoader.h"

#define DEPRECATED
#ifdef DEPRECATED
#include "LibMeshQuadratureRegister.h" //LibmeshBananaFun
#include "SandiaQuadratureRegister.h" //SandiaBananaFun
#endif //DEPRECATED

#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
	cerr << "Sono nel main" << endl;

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

#define LIBMESH_QUADRATURE
#ifdef LIBMESH_QUADRATURE
	Banana::LibmeshBananaFun();
#else
	Banana::SandiaBananaFun();
#endif

#endif //DEPRECATED

//	test1	(argc, argv);

/* useless
	test2	(argc, argv);
	test3	(argc, argv);
*/

//	test4	(argc, argv);
//	test5	(argc, argv);
//	test6	(argc, argv);
//	test7	(argc, argv);
//	test8	(argc, argv);
//	test9	(argc, argv);
//	test10(argc, argv); //perche' il primo polinomio non integrato esattamente ha norma nulla?
//	test11(argc, argv);
//	test12(argc, argv);

//	test13(argc, argv);

//	test14(argc, argv);

	test15(argc, argv);
//	test16(argc, argv);

	cout << "Tests completed" << endl;

	return 0;
}
