#ifndef __BASIC_CONFIGURATION_H
#define __BASIC_CONFIGURATION_H

#include "gtest/gtest.h"

#include "BinaryTreeHelper.h" //Logfile

/**
	Configuration class redirecting buffers.
	This configuration class just redirects the buffers to "test.log",
	in order to have clear output when running the testing framework.

	It is the basic configuration class, tests which do not need anything else
	will be written as TEST_F(BasicTest, TestName){...};

	Tests which need more configuration instructions
	will use configuration classes derived from BasicTest
**/
class BasicTest : public ::testing::Test
{
  protected:
	/**
		constructor.
		It simply initialize the _out attribute with "test.log" filename
	**/
	BasicTest();
	/**
		destructor.
		Destroying _out attribute buffers are redirected to std ones
	**/
	virtual ~BasicTest();

	virtual void SetUp()override;
	virtual void TearDown()override;

	Helpers::Logfile _out;
};

//TODO The first test run, whatever it is, print always part of the output to std::cout. Fix it

#endif //__TEST_CONFIGURATION_H
