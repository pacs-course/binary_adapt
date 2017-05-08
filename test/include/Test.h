#ifndef __TEST_H
#define __TEST_H

#include "gtest/gtest.h"

#include "PluginLoader.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

class Logfile
{
	public:
		Logfile(const std::string& filename) : _ofs(filename, std::ios_base::app)
															,_out_buf(std::cout.rdbuf(_ofs.rdbuf()))
															,_log_buf(std::clog.rdbuf(_ofs.rdbuf()))
															,_err_buf(std::cerr.rdbuf(_ofs.rdbuf()))
		{};

		~Logfile()
		{
			std::clog.rdbuf(_log_buf);
			std::cout.rdbuf(_out_buf);
			std::cerr.rdbuf(_err_buf);
		};

	private:
		std::ofstream _ofs;
		std::basic_streambuf<char>* _out_buf;
		std::basic_streambuf<char>* _log_buf;
		std::basic_streambuf<char>* _err_buf;
};


class Initializer
{
	public:
		Initializer(){};
		void Load (const std::vector<std::string>&);

	protected:
		PluginLoading::PluginLoader _pl;
};

class LoadTest : public ::testing::Test
{
	protected:
		LoadTest() : _out("test.log"), _argc(1)
		{
#ifndef DEBUG
			_argv[0] = "test";
#else
			_argv[0] = "test_Debug";
#endif //DEBUG
		};
		virtual void SetUp();

		Logfile _out;
		Initializer _initializer;
		int _argc;
		char* _argv[1];
};

#endif //__TEST_H
