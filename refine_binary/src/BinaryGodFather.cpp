#include "BinaryGodFather.h"

using namespace std;

namespace BinaryTree
{
	RecursiveSelector::RecursiveSelector() {}
	RecursiveSelector::~RecursiveSelector() {}

	void RecursiveSelector::operator() (BinaryNode* node)
	{
		SelectActiveNodesRecursively (node);
	}

	void RecursiveSelector::SelectActiveNodesRecursively (BinaryNode* node)
	{
		if (node)
		{
			auto hansel = node->Left();
			auto gretel = node->Right();

			if (node->E() == node->ProjectionError())
			{
				node->Activate();

				DeactivateSubTree (hansel);
				DeactivateSubTree (gretel);
			}
			else
			{
				SelectActiveNodesRecursively (hansel);
				SelectActiveNodesRecursively (gretel);

				node->Deactivate();
			}
		}
	}

	void RecursiveSelector::DeactivateSubTree (BinaryNode* node)
	{
		if (node)
		{
			node->Deactivate();
			DeactivateSubTree (node->Left());
			DeactivateSubTree (node->Right());
		}
	}

} //namespace BinaryTree
