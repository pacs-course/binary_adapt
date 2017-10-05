#include "BinaryNode.h"

#include <limits> //numeric_limits::max()

using namespace std;

namespace BinaryTree
{
	BinaryNode::BinaryNode() : _s_element	(this),
										_E				(numeric_limits<double>::max()),
										_E_tilde		(numeric_limits<double>::max()),
										_q				(numeric_limits<double>::max()),
										_tilde_error(numeric_limits<double>::max())
	{};

	BinaryNode::BinaryNode (const BinaryNode& bn)
	{
		*this = bn;
	};

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
	};

	BinaryNode* BinaryNode::S() const
	{
		return _s_element;
	};

	void BinaryNode::S(BinaryNode* delta)
	{
		_s_element = delta;
	};

	double BinaryNode::E() const
	{
		return _E;
	};

	void BinaryNode::E(double val)
	{
		_E = val;
	};

	double BinaryNode::ETilde() const
	{
		return _E_tilde;
	};

	void BinaryNode::ETilde (double val)
	{
#ifdef TRY_IT
		if (val < 1E-15)
			_E_tilde = 0;
		else
#endif //TRY_IT
			_E_tilde = val;
	};

	double BinaryNode::Q() const
	{
		return _q;
	};

	void BinaryNode::Q(double val)
	{
		_q = val;
	};

	double BinaryNode::TildeError() const
	{
		return _tilde_error;
	};

	void BinaryNode::TildeError(double val)
	{
		_tilde_error = val;
	};

	void BinaryGodfather::SelectActiveNodes()
	{
		for (auto& el : this->_elements)
#ifdef CUT_FROM_THE_BOTTOM
			SelectActiveNodesRecursively(el, el->E());
#else
			SelectActiveNodesRecursively(el);
#endif //CUT_FROM_THE_BOTTOM
	};

#ifdef CUT_FROM_THE_BOTTOM
	void BinaryGodfather::SelectActiveNodesRecursively(BinaryNode* node, const double E_value)
#else
	void BinaryGodfather::SelectActiveNodesRecursively(BinaryNode* node)
#endif //CUT_FROM_THE_BOTTOM
	{
		if (node)
		{
			auto hansel = node->Left();
			auto gretel = node->Right();

#ifdef CUT_FROM_THE_BOTTOM
			bool left_found = FindERecursively(hansel, E_value);
			bool right_found = FindERecursively(gretel, E_value);

			if (!left_found && !right_found)
			{
				if (node->E() != E_value)
					cerr << "Houston we have a problem: q value not found in the tree!" << endl;
#else //CUT_FROM_THE_BOTTOM
			if (node->E() == node->ProjectionError())
			{
#endif //CUT_FROM_THE_BOTTOM
#ifdef MYDEBUG
				cerr << "Activating element with " << endl;
				auto id = node->NodeID();
				cerr << "#id : " << id << endl;
				cerr << "p level : " << node->PLevel() << endl;
				cerr << "e = " << node->ProjectionError() << endl;
				cerr << "E = " << node->E() << endl;
				cerr << "e~ = " << node->TildeError() << endl;
				cerr << "q = " << node->Q() << endl;
				cerr << "E~ = " << node->ETilde() << endl << endl;
#endif //MYDEBUG

				node->Activate();
				DeactivateSubTree(hansel);
				DeactivateSubTree(gretel);
			}
			else
			{
#ifdef CUT_FROM_THE_BOTTOM
				SelectActiveNodesRecursively(hansel, hansel->E());
				SelectActiveNodesRecursively(gretel, gretel->E());
#else //CUT_FROM_THE_BOTTOM
				SelectActiveNodesRecursively(hansel);
				SelectActiveNodesRecursively(gretel);
#endif //CUT_FROM_THE_BOTTOM

#ifdef MYDEBUG
				cerr << "Deactivating element with " << endl;
				auto id = node->NodeID();
				cerr << "#id : " << id << endl;
				cerr << "p level : " << node->PLevel() << endl;
				cerr << "e = " << node->ProjectionError() << endl;
				cerr << "E = " << node->E() << endl;
				cerr << "e~ " << node->TildeError() << endl;
				cerr << "q = " << node->Q() << endl;
				cerr << "E~ = " << node->ETilde() << endl << endl;
#endif //MYDEBUG

				node->Deactivate();
			}
		}
	};

#ifdef CUT_FROM_THE_BOTTOM
	bool BinaryGodfather::FindERecursively(BinaryNode* node, const double E_value)
	{
		if (node)
		{
			if (E_value == node->E())
				return true;
			else
			{
				bool left_found = FindERecursively(node->Left(), E_value);
				bool right_found = FindERecursively(node->Right(), E_value);
				return left_found | right_found;
			}
		}
		return false;
	};
#endif //CUT_FROM_THE_BOTTOM

	void BinaryGodfather::DeactivateSubTree(BinaryNode* node)
	{
		if(node)
		{
#ifdef MYDEBUG
			cerr << "Disattivo l'elemento di " << endl;
			auto id = node->NodeID();
			cerr << "#id : " << id << endl;
			cerr << "p level : " << node->PLevel() << endl;
			cerr << "e = " << node->ProjectionError() << endl;
			cerr << "E = " << node->E() << endl;
			cerr << "e~ " << node->TildeError() << endl;
			cerr << "q = " << node->Q() << endl;
			cerr << "E~ = " << node->ETilde() << endl << endl;
#endif //MYDEBUG
			node->Deactivate();
			DeactivateSubTree(node->Left());
			DeactivateSubTree(node->Right());
		}
	};

	BinaryNode* BinaryGodfather::MakeBisection()
	{
		SortElements();
		auto gonna_be_divided = (*(_elements.begin()))->S();
		gonna_be_divided->Bisect();
		return gonna_be_divided;
	};

	void BinaryGodfather::SortElements()
	{
		//TODO: optimize it when I need to sort only the first element (i.e. after a bisection)
		_elements.sort( [](BinaryNode* n1, BinaryNode* n2){return n1->S()->Q() > n2->S()->Q();} );
	};

} //namespace BinaryTree
