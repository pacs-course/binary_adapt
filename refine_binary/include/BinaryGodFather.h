#ifndef __BINARY_GOD_FATHER_H
#define __BINARY_GOD_FATHER_H

#include <algorithm> //std::for_each

#include "BinaryNode.h"

namespace BinaryTree
{

/*
	class for the object which has to work as the father of every element of the initial mesh
	it could be meant as a "ghost" node
	the output of the S() method of this class will be the node to be bisected at 
	each iteration of the algorithm
*/
	template <size_t dim>
		class DimensionedGodFather
		{
			public:
				DimensionedGodFather(){};

				template <typename Iterator>
					void FillElements(Iterator begin,
											Iterator end,
											std::function<DimensionedNode<dim>*(Iterator)> extract = [](Iterator iter){return *iter;})
					{
						//TODO : maybe a resize could be fine, push_back can be avoided
						this->_elements.clear();
						for (; begin != end; ++begin)
							this->_elements.push_back(extract(begin));
				
						SortElements();
					};

				BinaryNode* MakeBisection();
				void SelectActiveNodes();

			protected:
				void SortElements();

			protected:
/**
				These are the binary elements of the initial mesh.
				Every element could generate a binary tree during the algorithm execution;
				_elements will always be sorted in such a way that the first element of the vector
				must point to (i.e. the return of the S() method must be) the next element to be divided
**/
				std::list<DimensionedNode<dim>*> _elements;
		};

	class RecursiveSelector
	{
		public:
			RecursiveSelector();
			~RecursiveSelector();
			void operator() (BinaryNode*);
		protected:
			void SelectActiveNodesRecursively(BinaryNode*);
			void DeactivateSubTree(BinaryNode*);
	};

	template <size_t dim>
		void DimensionedGodFather<dim>::SelectActiveNodes()
		{
			RecursiveSelector rs;
			std::for_each(_elements.begin(), _elements.end(), rs);
		};

	template <size_t dim>
		BinaryNode* DimensionedGodFather<dim>::MakeBisection()
		{
			SortElements();
			auto gonna_be_divided = (*(_elements.begin()))->S();
			gonna_be_divided->Bisect();
			return gonna_be_divided;
		};

	template <size_t dim>
		void DimensionedGodFather<dim>::SortElements()
		{
			//TODO: optimize it when I need to sort only the first element (i.e. after a bisection)
			this->_elements.sort( [](BinaryNode* n1, BinaryNode* n2){return n1->S()->Q() > n2->S()->Q();} );
		};

}
#endif //__BINARY_GOD_FATHER_H
