#ifndef __DYNAMIC_LOAD_CONFIGURATION_H
#define __DYNAMIC_LOAD_CONFIGURATION_H

#include "BasicConfiguration.h"

#include "PluginLoader.h"

/**
	Configuration class loading plugins.
	This configuration class add to the redirecting of the buffers given by #BasicTest
	the loading quadratures and functors.
**/
class LoadTest : public BasicTest
{
	protected:
/**
		default constructor
**/
		LoadTest();
/**
		default destructor
**/
		virtual ~LoadTest();
/**
		do the loading
**/
		virtual void SetUp()override;
		virtual void TearDown()override;

		PluginLoading::PluginLoader _pl;
};

#endif //__DYNAMIC_LOAD_CONFIGURATION_H
