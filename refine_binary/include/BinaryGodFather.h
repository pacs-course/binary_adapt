#ifndef __BINARY_GOD_FATHER_H
#define __BINARY_GOD_FATHER_H

#include <algorithm> //std::for_each
#include <list> //std::list

#include "BinaryNode.h"

namespace BinaryTree
{
	/**
		class for the object which has to work as the father of every element of the initial mesh
		it could be meant as a "ghost" node
		the output of the S() method of this class will be the node to be bisected at
		each iteration of the algorithm
	**/
	template <size_t dim>
	class DimensionedGodFather
	{
	  public:
		/**
			default constructor
		**/
		DimensionedGodFather() {};

		/**
			default destructor
		**/
		virtual ~DimensionedGodFather() {};

		/**
			Method to fill the _elements list.
			Implements the std::for_each pattern.
			The input functor takes an Iterator object
			and returns an element that can be inserted in the _elements list.
			By default it simply dereferenciate the iterator.
		**/
		template <typename Iterator>
		void FillElements (Iterator,
						   Iterator,
						   std::function<DimensionedNode<dim>* (Iterator)> extract =
						   	[](Iterator iter) {return *iter;});
		/**
			Refine the next leaf to be bisected.
			It takes the S() parameter of the first element of _elements list and bisects it.
			It returns a pointer to the father of the new just created leafs.
		**/
		BinaryNode* MakeBisection();
		/**
			Perform the trim operation on the binary tree.
			It navigates the tree, starting from the root node,
			and when it finds that node parameters E = e it makes the trim,
			that is it activate the node and deactivate the whole subtree rooted at that node.
			No element is destroyed.
		**/
		void SelectActiveNodes();

	  protected:
		/**
			Keep sorted the _elements list
		**/
		void SortElements();

	  protected:
		/**
			These are the binary elements of the initial mesh.
			Every element could generate a binary tree during the algorithm execution;
			_elements will always be sorted in such a way that the first element of the list,
			when S() method being called on it, must return the next element to be divided
		**/
		std::list<DimensionedNode<dim>*> _elements;
	};


	class RecursiveSelector
	{
	  public:
		/**
			default constructor
		**/
		RecursiveSelector();
		/**
			default destructor
		**/
		virtual ~RecursiveSelector();
		/**
			Perform the selection of active nodes in binary tree rooted at input node.
			The selection is done recursively through the SelectActiveNodesRecursively() method.
		**/
		void operator() (BinaryNode*);

		/**
			Deactivate every element of the subtree rooted at input node
		**/
		void DeactivateSubTree (BinaryNode*);

	  protected:
		/**
			Recursive selection of active nodes.
			If E = e it activates the input node and deactivate the subtree rooted at it,
			otherwise the node is deactivated and the recursive selection done on the children.
		**/
		void SelectActiveNodesRecursively (BinaryNode*);
	};

	template <size_t dim>
	template <typename Iterator>
	void DimensionedGodFather<dim>
	::FillElements (Iterator begin,
				    Iterator end,
				    std::function<DimensionedNode<dim>* (Iterator)> extract)
	{
		this->_elements.clear();
		for (; begin != end; ++begin)
			this->_elements.push_back (extract (begin));

		SortElements();
	};


	template <size_t dim>
	void DimensionedGodFather<dim>::SelectActiveNodes()
	{
		RecursiveSelector rs;
		std::for_each (_elements.begin(), _elements.end(), rs);
	};

	template <size_t dim>
	BinaryNode* DimensionedGodFather<dim>::MakeBisection()
	{
		SortElements();
		auto first_element = * (_elements.begin());
		/** I don't want to risk to have 2 active leafs
		    with an active ancestor, so before making the bisection,
		    that will generate 2 active leafs,
		    I deactivate all their ancestors. **/
		RecursiveSelector rs;
		rs.DeactivateSubTree (first_element);

		auto gonna_be_divided = first_element->S();

		gonna_be_divided->Bisect();
		return gonna_be_divided;
	};

	template <size_t dim>
	void DimensionedGodFather<dim>::SortElements()
	{
//*INDENT-OFF*
		//TODO: optimize it when I need to sort only the first element (i.e. after a bisection)
		this->_elements.sort
		(
			[]
			(BinaryNode * n1, BinaryNode * n2)
			{return n1->S()->Q() > n2->S()->Q();}
		);
//*INDENT-ON*
	};

}
#endif //__BINARY_GOD_FATHER_H
