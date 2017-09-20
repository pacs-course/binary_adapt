#ifndef __TEST_H
#define __TEST_H

#include "gtest/gtest.h"

#include "PluginLoader.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "libmesh/libmesh.h" //libMesh::LibMeshInit
#include "ostream_proxy.h" //libMesh::err

class Logfile
{
	public:
		Logfile(const std::string&);

		~Logfile();

	private:
		std::ofstream _ofs;
		std::basic_streambuf<char>* _out_buf;
		std::basic_streambuf<char>* _log_buf;
		std::basic_streambuf<char>* _err_buf;
		std::basic_streambuf<char>* _libmesh_err_buf;
		std::basic_streambuf<char>* _libmesh_out_buf;
};


class Initializer
{
	public:
		Initializer(){};
		bool Load (const std::vector<std::string>&);

	protected:
		PluginLoading::PluginLoader _pl;
};

class LoadTest : public ::testing::Test
{
	protected:
		LoadTest();
		virtual void SetUp();
		virtual ~LoadTest();
		virtual void TearDown()override;

		std::unique_ptr<libMesh::LibMeshInit> _mesh_init_ptr;
		Logfile _out;
		Initializer _initializer;
};


#endif //__TEST_H
