#include "MeshRefiner.h"

namespace BinaryTree
{
	ErrorComputer::ErrorComputer(double& error_location) : _error_variable(error_location){};

	void ErrorComputer::operator()(BinaryNode* node)
	{
		(this->_error_variable) += node->ProjectionError();
	};

	void ErrorComputer::ResetError()
	{
		this->_error_variable = 0;
	};

} //namespace BinaryTree
