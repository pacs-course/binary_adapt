#include "DynamicLoadConfiguration.h"

using namespace std;

LoadTest::LoadTest() : BasicTest(), _pl()
{}

LoadTest::~LoadTest()
{}

void LoadTest::SetUp()
{
	BasicTest::SetUp();

	vector<string> handles;
#ifndef DEBUG
	handles.push_back ("libmesh_quadrature.so");
	handles.push_back ("libsandia_quadrature.so");
	handles.push_back ("libinterpolating_functions.so");
#else //DEBUG
	handles.push_back ("libmesh_quadrature_Debug.so");
	handles.push_back ("libsandia_quadrature_Debug.so");
	handles.push_back ("libinterpolating_functions_Debug.so");
#endif //DEBUG
	for (auto& handle : handles)
		_pl.Add (handle);

	if (!_pl.Load())
		throw runtime_error (
			"Houston we have a problem: something went wrong loading plugins");
}

void LoadTest::TearDown()
{
	BasicTest::TearDown();
}
