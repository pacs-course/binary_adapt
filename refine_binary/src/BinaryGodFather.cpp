#include "BinaryGodFather.h"

using namespace std;

namespace BinaryTree
{
	RecursiveSelector::RecursiveSelector(){};
	RecursiveSelector::~RecursiveSelector(){};
	void RecursiveSelector::operator() (BinaryNode* node)
	{
		SelectActiveNodesRecursively(node);
	};

	void RecursiveSelector::SelectActiveNodesRecursively(BinaryNode* node)
	{
		if (node)
		{
			auto hansel = node->Left();
			auto gretel = node->Right();

			if (node->E() == node->ProjectionError())
			{
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
				SelectActiveNodesRecursively(hansel);
				SelectActiveNodesRecursively(gretel);

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


	void RecursiveSelector::DeactivateSubTree(BinaryNode* node)
	{
		if(node)
		{
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
			DeactivateSubTree(node->Left());
			DeactivateSubTree(node->Right());
		}
	};

} //namespace BinaryTree
