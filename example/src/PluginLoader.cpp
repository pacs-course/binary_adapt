#include "PluginLoader.h"

#include <iostream>

using namespace std;

namespace PluginLoading
{
	PluginLoader::PluginLoader(const std::string& loading_path) :	_loading_path(loading_path),
    																					_plugins(),
																						_loaded(false)
	{	};

	void PluginLoader::SetPath(const std::string& loading_path)
	{
		this->_loading_path = loading_path;
	};

	const std::string& PluginLoader::GetPath() const
	{
		return this->_loading_path;
	};

	void PluginLoader::Add(const std::string& so_file, int mode)
	{
		string full_path;
		if( this->_loading_path != "")
		{
			full_path = this->_loading_path + "/";
		}

		full_path += so_file;
	
		//TODO: add the check that this plugin has not already been added;
	
		_plugins.push_back(make_unique<Plugin>(full_path, mode));
		cerr << "Aggiunto plugin : " << so_file << endl;
	};

	bool PluginLoader::Load()
	{
		bool ok = true;

		for(size_t i = 0; i < Size(); ++i)
		{
			Plugin& p = *(this->_plugins[i]);

			// TODO: Test if the entry is a broken symlink

			if( p.Handle() == nullptr )
			{
				if (!p.Open())
					ok = false;
			}
		}
		this->_loaded = true;

		return ok;
	};

	size_t PluginLoader::Size() const
	{
		return this->_plugins.size();
	};
	
} //namespace PluginLoading
