#include "MeshRefiner.h"

using namespace std;

namespace BinaryTree
{
	void Counter::operator()(BinaryNode* node)
	{
#ifdef VERBOSE
		cerr << "Elemento attivo di:" << endl;
		auto id = node->NodeID();
		cerr << "#id : " << id << endl;
//#ifdef MYDEBUG
		cerr << "p level : " << node->PLevel() << endl;
		cerr << "e = " << node->ProjectionError() << endl;
		cerr << "E = " << node->E() << endl;
		cerr << "e~ = " << node->TildeError() << endl;
		cerr << "q = " << node->Q() << endl;
		cerr << "E~ = " << node->ETilde() << endl << endl;
//#endif //MYDEBUG
#endif //VERBOSE
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
