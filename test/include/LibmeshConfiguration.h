#ifndef __LIBMESH_CONFIGURATION_H
#define __LIBMESH_CONFIGURATION_H

#include "DynamicLoadConfiguration.h"

#include "libmesh/libmesh.h" //libMesh::LibMeshInit

/**
	Class redirecting libMesh buffers.
	Class with the same behaviour of #LogFile class,
	but acting on libMesh proper buffers.
**/
class LibmeshLogFile
{
	public:
/**
		constructor
**/
		LibmeshLogFile(const std::string&);
/**
		destructor
**/
		virtual ~LibmeshLogFile();

	private:
/**
		output file
**/
		std::ofstream _ofs;
/**
		libMesh error buffer
**/
		std::basic_streambuf<char>* _libmesh_err_buf;
/**
		libMesh standar buffer
**/
		std::basic_streambuf<char>* _libmesh_out_buf;
};

/**
	Configuration class for tests that need libMesh framework.
	This configuration class add to #LoadTest framework
	a the set up needed by tests which use libMesh and/or libMesh dependent objects.
**/
class LibmeshTest : public LoadTest
{
	protected:
/**
		constructor
**/
		LibmeshTest();
/**
		destructor
**/
		virtual ~LibmeshTest();
		virtual void SetUp()override;
		virtual void TearDown()override;

/**
		LogFile to redirect libMesh buffers
**/
		LibmeshLogFile _libmesh_out;
/**
		libMesh initializer object
**/
		std::unique_ptr<libMesh::LibMeshInit> _mesh_init_ptr;		
};

#endif //__LIBMESH_CONFIGURATION_H
