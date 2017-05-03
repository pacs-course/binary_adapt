#include "MeshRefiner.h"

namespace BinaryTree
{
	void Counter::operator()(BinaryNode* node)
	{
		//this is to avoid warning by the compiler
//		(void) node;
		cerr << "Elemento attivo di id # " << node->NodeID() << endl;
		++(this->_counter);
	};

	size_t Counter::GetCount()const
	{
		return this->_counter;
	};

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
