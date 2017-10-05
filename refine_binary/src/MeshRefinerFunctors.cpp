#include "MeshRefinerFunctors.h"

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

	GnuPlotPrinter::GnuPlotPrinter(ofstream& output_file) :	DimOperator<1>(),
																				_output_file(output_file),
																				_x_min(0),
																				_x_max(0),
																				_p_max(0),
																				_cont(0)
	{};

	void GnuPlotPrinter::operator() (DimensionedNode<1>* node)
	{
#ifdef MYDEBUG
		cerr << "Active element of id " << node->NodeID() << endl;
#endif //MYDEBUG
		auto nodes = node->Nodes();
		auto p_val = node->PLevel();

		double x_left = nodes[0];
		double x_right = nodes[1];
		this->_x_left_vec.push_back (x_left);
		this->_x_right_vec.push_back (x_right);
		this->_p_level_vec.push_back (p_val);

		if (x_left < this->_x_min)
			this->_x_min = x_left;
		if (x_right > this->_x_max)
			this->_x_max = x_right;
		if (p_val > this->_p_max)
			this->_p_max = p_val;

		string f_s	= "p_"
						+ to_string(this->_cont++)
						+ "(x) = x > "
						+ to_string(x_left)
						+ " && x <= "
						+ to_string(x_right)
						+ "? "
						+ to_string(p_val)
						+ " : 1/0";

		(this->_output_file) << f_s << endl;
	};

	double GnuPlotPrinter::XMin()const
	{
		return this->_x_min;
	};

	double GnuPlotPrinter::XMax()const
	{
		return this->_x_max;
	};

	size_t GnuPlotPrinter::PMax()const
	{
		return this->_p_max;
	};

	vector<double> GnuPlotPrinter::XLeftVec()const
	{
		return this->_x_left_vec;
	};

} //namespace BinaryTree
