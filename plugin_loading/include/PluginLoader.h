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
			PluginLoader();
			PluginLoader(const std::string& loading_path);

			~PluginLoader()
			{
#ifdef DESTRUCTOR_ALERT
				std::cerr << "Distruggo PluginLoader" << std::endl;
#endif
			};
			void Add(std::string, const std::string&, int mode = RTLD_LAZY);
			/*
			 	Adds shared objects to the list.
			 	The loading sequence will reflect the order of the add invocation.
			*/
			void Add(const std::string&, int mode = RTLD_LAZY);

			/*
			 	The main function to load all plugins.
			 	It returns true if all plugins are correctly loaded.
				After loading, no other shared objects can be added.
			*/
			bool Load();

			/*
				It loads every plugin contained in the _loading_path
			*/
			void LoadPath(const std::string&, int mode = RTLD_LAZY);

			size_t Size() const;

		private:
			PluginLoader(const PluginLoader&) = delete;
			PluginLoader& operator = (const PluginLoader&) = delete;

		private:
			std::vector<std::unique_ptr<Plugin>> _plugins;
			bool _loaded;
	};

} //namespace PluginLoading
 
#endif //__PLUGIN_LOADER_H
