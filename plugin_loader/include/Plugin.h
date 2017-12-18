#ifndef __PLUGIN_H
#define __PLUGIN_H

#include <string> //std::string
#include <dlfcn.h> //RTLD_LAZY

namespace PluginLoading
{
	/**
		Class to manage shared library.
		It gives an interface to wrap functions dlopen(), dlclose()...
	**/
	class Plugin
	{
	  public:
		/**
			constructor with shared object file name
		**/
		Plugin (const std::string&, int mode = RTLD_LAZY);
		/**
			destructor
		**/
		virtual ~Plugin();

		/**
			Load the shared object, returning the handle
		**/
		void* Open();

		/**
			Close the shared object handle if it was previously opened.
		**/
		void Close();

		/**
			Get the shared object handle. Useful to use dlsym function.
		**/
		void* Handle() const;

		/**
			Get the shared object passed to dlopen
		**/
		const std::string& File() const;

	  private:
		/**
			deleted copy constructor
		**/
		Plugin (const Plugin&) = delete;
		/**
			deleted assign operator
		**/
		Plugin& operator = (const Plugin&) = delete;

	  private:
		/**
			shared object
		**/
		std::string _so_file;
		/**
			handle returned by dlopen()
		**/
		void* _handle;
		/**
			mode to be passed to dlopen()
		**/
		int _mode;
	};

} //namespace PluginLoading

#endif //__PLUGIN_H
