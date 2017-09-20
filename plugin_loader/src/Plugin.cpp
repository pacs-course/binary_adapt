#include "Plugin.h"

#include <stdexcept> //std::runtime_error
#include <iostream>
using namespace std;

namespace PluginLoading
{
	Plugin::Plugin(const std::string& so_file, int mode) : _so_file(so_file), _handle(nullptr), _mode(mode)
	{	};

	Plugin::~Plugin()
	{
#ifdef DESTRUCTOR_ALERT
		std::cerr << "Sono nel distruttore di Plugin" << std::endl; 
#endif
#ifdef BANANA
		Close();
#endif //BANANA
#ifdef DESTRUCTOR_ALERT
		std::cerr << "Plugin distrutto" << std::endl;
#endif
	};

	void* Plugin::Open()
	{
		Close();
		this->_handle = dlopen(this->_so_file.c_str(), this->_mode);
		if (!this->_handle)
		{
			char* err = dlerror();
			throw runtime_error("Cannot load \"" + this->_so_file + "\": "  + string(err ? err : ""));
		}
		return this->_handle;
		
	};

	void Plugin::Close()
	{
#ifdef MYDEBUG
		cerr << "Closing plugin" << endl;
#endif //MYDEBUG
		if ( this->_handle )
		{
			int err = dlclose(this->_handle);
			if (err)
			{
				char* dlerr = dlerror();
				string err_human = dlerr ? dlerr : "";

				throw runtime_error(err_human + "closing" + this->_so_file + "\n dlclose returned " + to_string(err));
			}
		}
		this->_handle = nullptr;
	};

	void* Plugin::Handle() const
	{
		return this->_handle;
	};

	const std::string& Plugin::File() const
	{
		return this->_so_file;
	};

} //namespace PluginLoading
