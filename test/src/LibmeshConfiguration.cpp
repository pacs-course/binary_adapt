#include "LibmeshConfiguration.h"

#include "BinaryTreeHelper.h"

using namespace std;

LibmeshLogFile::LibmeshLogFile (const string& filename) :
	_ofs (filename, ios_base::app),
	_libmesh_err_buf (libMesh::err.rdbuf (_ofs.rdbuf())),
	_libmesh_out_buf (libMesh::out.rdbuf (_ofs.rdbuf()))
{}

LibmeshLogFile::~LibmeshLogFile()
{
	libMesh::err.rdbuf (_libmesh_err_buf);
	libMesh::out.rdbuf (_libmesh_out_buf);
}


LibmeshTest::LibmeshTest() :
	LoadTest(),
	_libmesh_out ("test.log"),
	_mesh_init_ptr (nullptr)
{
	string exe_name = "test";
	const char* trivialargv[] = {exe_name.c_str()};
	_mesh_init_ptr = Helpers::MakeUnique<libMesh::LibMeshInit> (1, trivialargv);
}

LibmeshTest::~LibmeshTest()
{}

void LibmeshTest::SetUp()
{
	LoadTest::SetUp();
}

void LibmeshTest::TearDown()
{
	LoadTest::TearDown();
}
