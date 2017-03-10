#ifndef __PLUGIN_H
#define __PLUGIN_H

#include <string> //std::string
#include <dlfcn.h> //RTLD_LAZY

namespace PluginLoading
{
	class Plugin
	{
		public:
			Plugin(const std::string& so_file, int mode = RTLD_LAZY);
			~Plugin();
			/*
			 	returns the handle
			*/
			void* Open();

			/*
			 	closes the shared object handle if it was previously opened.
			*/ 
			void Close();

			/*
			 	returns the shared object handle. Useful to use dlsym function.
			*/
			void* Handle() const;
	
			/*
				returns the shared object passed to dlopen 
			*/
			const std::string& File() const;

		private:
			Plugin(const Plugin&) = delete;
			Plugin& operator = (const Plugin&) = delete;
	
		private:
			std::string _so_file;
			void * _handle;
			int _mode;
	};

} //namespace PluginLoading

#endif //__PLUGIN_H
