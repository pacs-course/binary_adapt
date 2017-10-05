#ifndef __DINAMIC_LOAD_CONFIGURATION_H
#define __DINAMIC_LOAD_CONFIGURATION_H

#include "BasicConfiguration.h"

#include "PluginLoader.h"

class Initializer
{
	public:
		Initializer(){};
		bool Load (const std::vector<std::string>&);

	protected:
		PluginLoading::PluginLoader _pl;
};

//Configuration used by tests which need the loading of the dynamic plugins 
class LoadTest : public BasicTest
{
	protected:
		LoadTest();
		virtual ~LoadTest();
		virtual void SetUp()override;
		virtual void TearDown()override;

		Initializer _initializer;
};

#endif //__DINAMIC_LOAD_CONFIGURATION_H
