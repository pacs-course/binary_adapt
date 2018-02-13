#include "BinaryNode.h"

#include <limits> //numeric_limits::max()

using namespace std;

namespace BinaryTree
{
	BinaryNode::BinaryNode() :
		_s_element	(this),
		_E			(numeric_limits<double>::max()),
		_E_tilde	(numeric_limits<double>::max()),
		_q			(numeric_limits<double>::max()),
		_tilde_error(numeric_limits<double>::max())
	{}

	BinaryNode::~BinaryNode()
	{}

	BinaryNode::BinaryNode (const BinaryNode& bn)
	{
		*this = bn;
	}

	BinaryNode& BinaryNode::operator = (const BinaryNode& bn)
	{
		if (&bn != this)
		{
			_s_element = bn._s_element;
			_E = bn._E;
			_E_tilde = bn._E_tilde;
			_q = bn._q;
			_tilde_error = bn._tilde_error;
		}
		return *this;
	}

	BinaryNode* BinaryNode::S() const
	{
		return _s_element;
	}

	void BinaryNode::S (BinaryNode* delta)
	{
		_s_element = delta;
	}

	double BinaryNode::E() const
	{
		return _E;
	}

	void BinaryNode::E (double val)
	{
		_E = val;
	}

	double BinaryNode::ETilde() const
	{
		return _E_tilde;
	}

	void BinaryNode::ETilde (double val)
	{
		_E_tilde = val;
	}

	double BinaryNode::Q() const
	{
		return _q;
	}

	void BinaryNode::Q (double val)
	{
		_q = val;
	}

	double BinaryNode::TildeError() const
	{
		return _tilde_error;
	}

	void BinaryNode::TildeError (double val)
	{
		_tilde_error = val;
	}

} //namespace BinaryTree
