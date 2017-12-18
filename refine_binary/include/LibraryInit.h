#ifndef __LIBRARY_INIT_HH
#define __LIBRARY_INIT_HH

#include <string>

namespace BinaryTree
{
	/**
		Library initialization from input configuration file.
		It returns 0 if everything goes right.
		If something goes wrong it returns 1.
	**/
	bool Init (std::string conf_filename = "../../binary_tree.conf");
} //namespace BinaryTree

#endif //__LIBRARY_INIT_HH
