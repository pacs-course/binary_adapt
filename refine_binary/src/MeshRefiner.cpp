#include "MeshRefiner.h"

namespace BinaryTree
{
	ErrorComputer::ErrorComputer(double& error_location) : _error_variable(error_location){};

	void ErrorComputer::operator()(BinaryNode* node)
	{
		(this->_error_variable) += node->ProjectionError();
//		(this->_error_variable) += node->ETilde();
	};

	void ErrorComputer::ResetError()
	{
		this->_error_variable = 0;
	};

	void PlevelsExtractor::operator()(BinaryNode* node)
	{
		this->_p_levels.push_back(node->PLevel());
	};

	std::vector<size_t> PlevelsExtractor::GetPLevels()
	{
		return this->_p_levels;
	};

} //namespace BinaryTree
