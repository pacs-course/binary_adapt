#ifndef __BASIC_CONFIGURATION_H
#define __BASIC_CONFIGURATION_H

#include "gtest/gtest.h"

#include <fstream>
#include <iostream>
#include <string>

//class which redirects buffers on file
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
};

//This configuration just redirects the buffers to "test.log"
class BasicTest : public ::testing::Test
{
	protected:
		BasicTest();
		virtual void SetUp()override;
		virtual ~BasicTest();
		virtual void TearDown()override;

		Logfile _out;
};


#endif //__TEST_CONFIGURATION_H
