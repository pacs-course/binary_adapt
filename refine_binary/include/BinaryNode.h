#ifndef __BINARY_NODE_H
#define __BINARY_NODE_H

#include <iostream> //std::size_t
#include <list> //std::list
#include <functional> //std::function
#include <vector> //std::vector

#include "LinearAlgebra.h"

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

	template <size_t dim>
		class DimensionedNode : public BinaryNode
		{
			public:
				DimensionedNode() : BinaryNode(){};
				virtual double Projection(const Geometry::Point<dim>&) = 0;
				virtual std::vector<Geometry::Point<dim>> Nodes() = 0;
		};


} //namespace BinaryTree
#endif //__BINARY_NODE_H
