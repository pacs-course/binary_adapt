#include "DinamicLoadConfiguration.h"

using namespace std;

bool Initializer::Load(const vector<string>& handles)
{
	for (auto& handle : handles)
		_pl.Add(handle);
	return _pl.Load();
};

LoadTest::LoadTest() : BasicTest(), _initializer() {};

LoadTest::~LoadTest()
{
#ifdef DESTRUCTOR_ALERT
	clog << "Sono nel distruttore di LoadTest" << endl;
#endif //DESTRUCTOR_ALERT
};

void LoadTest::SetUp()
{
	BasicTest::SetUp();
#ifdef MYDEBUG
	clog << "Sono nella SetUp di LoadTest" << endl;
#endif //MYDEBUG

	vector<string> handles;
#ifndef DEBUG
	handles.push_back("libmesh_quadrature.so");
	handles.push_back("libsandia_quadrature.so");
	handles.push_back("libinterpolating_functions.so");
#else //DEBUG
	handles.push_back("libmesh_quadrature_Debug.so");
	handles.push_back("libsandia_quadrature_Debug.so");
	handles.push_back("libinterpolating_functions_Debug.so");
#endif //DEBUG
	if (!_initializer.Load(handles))
		throw runtime_error ("Houston we have a problem: something went wrong loading plugins");
};

void LoadTest::TearDown()
{
	BasicTest::TearDown();
#ifdef MYDEBUG
	clog << "Sono nella TearDown di LoadTest" << endl;
#endif //MYDEBUG
};
