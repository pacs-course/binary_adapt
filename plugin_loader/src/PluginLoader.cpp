#include "PluginLoader.h"

//TODO:	this is the only include from refine_binary library
//			if MakeUnique is re-written in this file, the plugin_loader library
//			can be made independent on refine_binary
#include "BinaryTreeHelper.h"

#include <dirent.h>
#include <iostream>

using namespace std;

namespace PluginLoading
{
	PluginLoader::PluginLoader() : _plugins(), _loaded (false) {}

	PluginLoader::~PluginLoader()
	{}

	void PluginLoader::Add (std::string full_path,
							const std::string& so_file,
							int mode)
	{
		full_path += so_file;
		this->Add (full_path, mode);
	}

	void PluginLoader::Add (const std::string& full_path_so_file, int mode)
	{
		if (this->_loaded)
			cerr << "Warning: "
				 << "adding a plugin, but the Load() has already been called"
				 << endl;

		//TODO: add the check that this plugin has not already been added;

		_plugins.push_back (
			Helpers::MakeUnique<Plugin> (full_path_so_file, mode));

#ifdef VERBOSE
		clog << "Added plugin : " << full_path_so_file << endl;
#endif //VERBOSE
	}

	bool PluginLoader::Load()
	{
		bool ok = true;

		for (auto& p_ptr : this->_plugins)
		{
			Plugin& p = *p_ptr;

			// TODO: Test if the entry is a broken symlink

			if ( p.Handle() == nullptr )
			{
				if (!p.Open())
					ok = false;
			}
		}
		this->_loaded = true;

		return ok;
	}

#ifdef TRY_IT
	/*	Plugin loading from a directory */
	PluginLoader::PluginLoader (const string& loading_path) : PluginLoader()
	{
		this->LoadPath (loading_path);
	}

	//TODO: test it
	void PluginLoader::LoadPath (const string& loading_path, int mode)
	{
		DIR* plugin_folder = opendir (loading_path.c_str());
		if (plugin_folder)
		{
			while (auto dir_content = readdir (plugin_folder))
			{
				string name = string (dir_content->d_name);

				//it navigates recursively subdirectories
				if (dir_content->d_type == DT_DIR)
					LoadPath (name, mode);
				else
				{
					if (name.substr (name.length() - 3) == ".so")
						Add (loading_path, name, mode);
				}
			}
		}
		else
		{
			cerr << "Warning: unable to open the directory: " << loading_path << endl;
		}
	}
#endif //TRY_IT
	size_t PluginLoader::Size() const
	{
		return this->_plugins.size();
	}

} //namespace PluginLoading
