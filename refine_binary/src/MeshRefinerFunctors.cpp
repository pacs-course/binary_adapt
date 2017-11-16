#include "MeshRefinerFunctors.h"

using namespace std;

namespace BinaryTree
{
	void Counter::operator()(BinaryNode*)
	{
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
	};

	void ErrorComputer::ResetError()
	{
		this->_error_variable = 0;
	};

	void PlevelsExtractor::operator()(BinaryNode* node)
	{
		this->_p_levels.push_back(node->PLevel());
	};

	vector<size_t> PlevelsExtractor::GetPLevels()
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
		clog << "Active element of id " << node->NodeID() << endl;
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

	ProjectionPrinter::ProjectionPrinter(ofstream& output_file, double x_step) :	_output_file(output_file),
																											_x_step(x_step)
	{}

	void ProjectionPrinter::operator() (DimensionedNode<1>* node)
	{
		auto nodes = node->Nodes();

		double x_left = nodes[0];
		double x_right = nodes[1];
		for (; x_left < x_right; x_left+=this->_x_step)
			this->_output_file << x_left << "	" << node->Projection(x_left) << endl;
	};
} //namespace BinaryTree
