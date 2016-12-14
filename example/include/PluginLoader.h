#ifndef __PLUGIN_LOADER_H
#define __PLUGIN_LOADER_H

#include "Plugin.h"

#include <vector> //std::vector
#include <memory> //std::unique_ptr
#include <iostream>
namespace PluginLoading
{
	class PluginLoader
	{
		public:
			PluginLoader(const std::string& loading_path = "");

			~PluginLoader()
			{
#ifdef DESTRUCTOR_ALERT
				std::cerr << "Distruggo PluginLoader" << std::endl;
#endif
			};
			/*
			 	Set path. 
			*/
			void SetPath(const std::string& loading_path);

			/*
			 	Get path. 
			*/
			const std::string& GetPath() const;

			/*
			 	Adds shared objects to the list.
			 	The loading sequence will reflect the order of the add invocation.
			*/
			void Add(const std::string& so_file, int mode = RTLD_LAZY);

			/*
			 	The main function to load all plugins.
			 	It returns true if all plugins are correctly loaded.
				After loading, no other shared objects can be added.
			*/
			bool Load();

			size_t Size() const;

		private:
			PluginLoader(const PluginLoader&) = delete;
			PluginLoader& operator = (const PluginLoader&) = delete;

		private:
			std::string _loading_path;
			std::vector<std::unique_ptr<Plugin>> _plugins;
			bool _loaded;
	};

} //namespace PluginLoading
 
#endif //__PLUGIN_LOADER_H
