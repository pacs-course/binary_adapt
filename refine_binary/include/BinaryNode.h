#ifndef __BINARY_NODE_H
#define __BINARY_NODE_H

#include <iostream> //std::size_t
#include <list> //std::list
#include <functional> //std::function
namespace BinaryTree
{
/*
	It contains attributes and relatively to-access-method needed by the algorithm
	This is meant to be the type of the master nodes of the binary tree
*/
	//TODO: pointers to be substituted: verify if smart pointers or const pointers are more useful
	class BinaryNode
	{
		public:
			BinaryNode();
			BinaryNode (const BinaryNode&);
			BinaryNode& operator = (const BinaryNode&);
			virtual void Init() = 0;

			virtual BinaryNode* S() const;
			virtual void S(BinaryNode*);

			virtual double E() const;
			virtual void E(double);

			virtual double ETilde() const;
			virtual void ETilde(double);

			virtual double Q() const;
			virtual void Q(double val);

			virtual double TildeError() const;
			virtual void TildeError(double val);

			/* They return nullptr if the node is a leaf */
			virtual BinaryNode* Left() = 0;
			virtual BinaryNode* Right() = 0;

			/* It returns nullptr if the node has no parent */
			virtual BinaryNode* Dad() = 0;

			virtual double ProjectionError() = 0;
			virtual void Bisect() = 0;

			virtual void PLevel(std::size_t) = 0;
			virtual std::size_t PLevel() const = 0;

			virtual void Activate() = 0;
			virtual void Deactivate() = 0;

			virtual size_t NodeID() = 0;

		protected:
			virtual void UpdateProjectionError() = 0;

		protected:
			BinaryNode* _s_element;
			double _E;
			double _E_tilde;
			double _q;
			double _tilde_error;
	};

/*
	class for the object which has to work as the father of every element of the initial mesh
	it could be meant as a "ghost" node
	the output of the S() method of this class will be the node to be bisected at 
	each iteration of the algorithm
*/
	class BinaryGodfather
	{
		public:
			BinaryGodfather(){};

			template <typename Iterator>
			void FillElements(Iterator begin, Iterator end, std::function<BinaryNode*(Iterator)> extract = [](Iterator iter){return *iter;})
			{
				//TODO : maybe a resize could be fine, push_back can be avoided
				_elements.clear();
				for (; begin != end; ++begin)
					_elements.push_back(extract(begin));
				
				SortElements();
			};

			BinaryNode* MakeBisection();
			void SelectActiveNodes();

		protected:
			void SortElements();
			void DeactivateSubTree(BinaryNode*);
//This is wrong at the moment!
#ifdef CUT_FROM_THE_BOTTOM
			bool FindERecursively(BinaryNode*, const double);
			void SelectActiveNodesRecursively(BinaryNode*, const double);
#else
			void SelectActiveNodesRecursively(BinaryNode*);
#endif
		protected:
/*
			these are the binary elements of the initial mesh;
			every element could generate a binary tree during the algo;
			_elements will always be sorted in such a way that the first element of the vector
			must point to (i.e. the return of the S() method must be) the next element to be divided
*/
			std::list<BinaryNode*> _elements;
	};

} //namespace BinaryTree
#endif //__BINARY_NODE_H
