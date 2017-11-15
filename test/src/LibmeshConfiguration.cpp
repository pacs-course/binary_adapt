#include "LibmeshConfiguration.h"

#include "BinaryTreeHelper.h"

using namespace std;

LibmeshLogFile::LibmeshLogFile(const string& filename) : _ofs(filename, ios_base::app)
																			,_libmesh_err_buf(libMesh::err.rdbuf(_ofs.rdbuf()))
																			,_libmesh_out_buf(libMesh::out.rdbuf(_ofs.rdbuf()))
{
#ifdef MYDEBUG
	clog << "I buffer di libMesh sono su file" << endl;
#endif //MYDEBUG
};

LibmeshLogFile::~LibmeshLogFile()
{
#ifdef MYDEBUG
	clog << "Redirigo i buffer di libMesh sullo standard" << endl;
#endif //MYDEBUG
	libMesh::err.rdbuf(_libmesh_err_buf);
	libMesh::out.rdbuf(_libmesh_out_buf);
#ifdef MYDEBUG
	cerr << "Buffer di libMesh rediretti" << endl;
#endif //MYDEBUG
};


LibmeshTest::LibmeshTest() : LoadTest(), _libmesh_out("test.log"), _mesh_init_ptr(nullptr) 
{
#ifdef MYDEBUG
	clog << "Sono nel costruttore di LibmeshTest" << endl;
#endif //MYDEBUG

	string exe_name = "test";
	const char* trivialargv[] = {exe_name.c_str()};
	_mesh_init_ptr = Helpers::MakeUnique<libMesh::LibMeshInit>(1, trivialargv);
};

LibmeshTest::~LibmeshTest()
{
#ifdef DESTRUCTOR_ALERT
	clog << "Sono nel distruttore di LibmeshTest" << endl;
#endif //DESTRUCTOR_ALERT
};

void LibmeshTest::SetUp()
{
	LoadTest::SetUp();
#ifdef MYDEBUG
	clog << "Sono nella SetUp di LibmeshTest" << endl;
#endif //MYDEBUG
};

void LibmeshTest::TearDown()
{
	LoadTest::TearDown();
#ifdef MYDEBUG
	clog << "Sono nella TearDown di LibmeshTest" << endl;
#endif //MYDEBUG
};
