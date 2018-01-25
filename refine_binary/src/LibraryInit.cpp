#include "LibraryInit.h"
#include "PluginLoader.h"
#include "StdFElementInterface.h"
#include "MeshRefiner.h"
#include "BinaryTreeHelper.h" //Cfgfile

using namespace std;

namespace BinaryTree
{
	//TODO:	maybe add a different behaviour (throw exception, do nothing, do something else...)
	//		if this function is called more than once
	bool Init (string conf_filename)
	{
		Helpers::Cfgfile cl(conf_filename);

		/* TODO:
		if two different libraries try to register with the same key,
		the item available will be the one of the last library loaded;
		because of this, if two libraries both register two items with
		the two same keys (i.e. both sandia and libMesh register their rule
		for intervals and their one for squares), it will be impossible to use
		one library for the first key and the other library for the second one,
		since the first two ones registered by the first library will be overwritten */

		string ipercube_quad_library = "binary_tree/quadrature/ipercube/quad_library";
		string tri_quad_library		 = "binary_tree/quadrature/triangle/quad_library";

		string func_library = "binary_tree/functions/func_library";
		string mesh_library = "binary_tree/mesh/mesh_library";

		string tri_quad_so_file	 = "lib"
								 + string (cl (tri_quad_library.c_str(),
											   "mesh_quadrature"));
		string iper_quad_so_file = "lib"
								 + string (cl (ipercube_quad_library.c_str(),
											   "sandia_quadrature"));
		string func_so_file = "lib"
							+ string (cl (func_library.c_str(),
										  "interpolating_functions"));
		string mesh_so_file = "lib"
							+ string (cl (mesh_library.c_str(),
									  	  "mesh_bridge"));

#ifdef DEBUG
		tri_quad_so_file  += "_Debug";
		iper_quad_so_file += "_Debug";
		func_so_file += "_Debug";
		mesh_so_file += "_Debug";
#endif //DEBUG
		tri_quad_so_file  += ".so";
		iper_quad_so_file += ".so";
		func_so_file += ".so";
		mesh_so_file += ".so";

		//I don't want the plugins to be destroyed at the end of this function
		//but at the exit of the program, so I create a static variable
		static PluginLoading::PluginLoader pl;
#ifdef VERBOSE
		cerr << "Loader constructed" << endl;
#endif //VERBOSE

		//I popolate the factories
		pl.Add (func_so_file);
		pl.Add (mesh_so_file);
		pl.Add (tri_quad_so_file);

		//I don't want to register two times the same builders
		if (tri_quad_so_file != iper_quad_so_file)
			/*	I'm giving priority to ipercube quadratures,
				i.e. if tri_quad_so_file makes some registration for quadratures
				for ipercube objects (interval, square, ...)
				iper_quad_so_file will overwrite them */
			pl.Add (iper_quad_so_file);

		if (!pl.Load())
		{
			cerr << "Houston we have a problem! Something went wrong loading plugins";
			return 1;
		}
#ifdef VERBOSE
		clog << "Plugins loaded" << endl;
#endif //VERBOSE

		using namespace FiniteElements;
		string iteration_number = "binary_tree/algorithm/iteration_number";
		size_t n_iter = cl (iteration_number, 0);

		//TODO: call only the initialization of the norms
		//		effectively needed by the specific run of the program
		StdFElementInterface<1, LegendreType>::InitNorms (n_iter);
		StdFElementInterface<2, LegendreType>::InitNorms (n_iter);
		StdFElementInterface<2, WarpedType>::InitNorms (n_iter);

		return 0;
	};
} //namespace BinaryTree
