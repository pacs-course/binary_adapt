#ifndef __PLUGIN_LOADER_H
#define __PLUGIN_LOADER_H

#include "Plugin.h"

#include <vector> //std::vector
#include <memory> //std::unique_ptr
#include <iostream>

namespace PluginLoading
{
/**
	Class to semplify the load process of plugins.
	If a series of plugins have to be loaded,
	they can be added to the PluginLoader through Add() method,
	and then load through the Load() method.
**/
	class PluginLoader
	{
		public:
/**
			constructor
**/
			PluginLoader();
/**
			default destructor
**/
			virtual ~PluginLoader();

/**
			Add shared object to plugin list.
			Input parameters:
				- the directory where the file is stored
				- the file name
				- the loading mode
**/
			void Add(std::string, const std::string&, int mode = RTLD_LAZY);
/**
			 	Add shared object to plugin list.
				File name passed as absolute path.
			 	The loading sequence will reflect the order of the add invocation.
**/
			void Add(const std::string&, int mode = RTLD_LAZY);

/**
		 	Load all plugins.
		 	It returns true if all plugins are correctly loaded.
			After loading, no other shared objects shall be added.
**/
			bool Load();
#ifdef TRY_IT
/**
			construct from a directory where plugin files are stored
**/
			PluginLoader(const std::string& loading_path);
/**
			Loads every plugin contained in a directory
**/
			void LoadPath(const std::string&, int mode = RTLD_LAZY);
#endif //TRY_IT
/**
			Number of plugins
**/
			size_t Size() const;

		private:
/**
			deleted copy constructor
**/
			PluginLoader(const PluginLoader&) = delete;
/**
			deleted assignment operator
**/
			PluginLoader& operator = (const PluginLoader&) = delete;

		private:
/**
			The vector storage of #Plugin objects
**/
			std::vector<std::unique_ptr<Plugin>> _plugins;
/**
			Flag telling if plugins have been loaded
**/
			bool _loaded;
	};

} //namespace PluginLoading
 
#endif //__PLUGIN_LOADER_H
