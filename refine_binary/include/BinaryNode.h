#ifndef __BINARY_NODE_H
#define __BINARY_NODE_H

#include <vector> //std::vector

#include "LinearAlgebra.h"

namespace BinaryTree
{
/*
	Class for a node of the binary tree.
	Class decribing an element of the binary tree used by Binev algorithm.
	It is independent on the dimensionality and on the geometry of the mesh.
	It contains attributes and relatively to-access-method needed by the algorithm.
*/
	//TODO: raw pointers to be replaced: verify if smart pointers or const pointers are more useful
	class BinaryNode
	{
		public:
/**
			constructor.
			Parameters are initialized to numeric_limits<double>::max() meaningless value.
			Correct values will be set by the Init() method, implemented in derived class AbstractBinaryElement.
			It cannot be done here, since at this level the projection error e cannot be computed.
**/
			BinaryNode();
/**
			default destructor
**/
			virtual ~BinaryNode();
/**
			copy constructor
**/
			BinaryNode (const BinaryNode&);
/**
			assignment operator
**/
			BinaryNode& operator = (const BinaryNode&);
/**
			initialize the element.
			Defined in derived class AbstractBinaryElement, it should initialize all the attributes to their default value,
			i.e. the value of the parameter in a leaf.
			It cannot be done here, since at this level the projection error e cannot be computed.
			It must be compulsory called after the construction
**/
			virtual void Init() = 0;

/**
			get the s parameter
**/
			virtual BinaryNode* S() const;
/**
			set the s parameter
**/
			virtual void S(BinaryNode*);

/**
			get the E parameter
**/
			virtual double E() const;
/**
			set the s parameter
**/
			virtual void E(double);

/**
			get the E~ parameter
**/
			virtual double ETilde() const;
/**
			set the E~ parameter
**/
			virtual void ETilde(double);

/**
			get the q parameter
**/
			virtual double Q() const;
/**
			set the q parameter
**/
			virtual void Q(double val);

/**
			get the e~ parameter
**/
			virtual double TildeError() const;
/**
			set the e~ parameter
**/
			virtual void TildeError(double val);

/**
			get the pointer to the left child node of the element.
			It returns nullptr if the node is a leaf
**/
			virtual BinaryNode* Left() = 0;
/**
			get the pointer to the right child node of the element.
			It returns nullptr if the node is a leaf
**/
			virtual BinaryNode* Right() = 0;

/**
			get the pointer to the parent node of the element.
			It returns nullptr if the node has no parent
**/
			virtual BinaryNode* Dad() = 0;

/**
			get the e parameter
**/
			virtual double ProjectionError() = 0;
/**
			Refine the element.
			It generates two children nodes
**/
			virtual void Bisect() = 0;

/**
			set the p refinement level of the element
**/
			virtual void PLevel(std::size_t) = 0;
/**
			get the p refinement level of the element
**/
			virtual std::size_t PLevel() const = 0;

/**
			Make this node an active element of the mesh.
			
**/
			virtual void Activate() = 0;
/**
			Make this node an unactive element of the mesh.
			It will still be part of the tree, but not of the extracted mesh
**/
			virtual void Deactivate() = 0;

/**
			get the ID of the element.
			It uniquely identifies the node
**/
			virtual size_t NodeID() = 0;

		protected:
/**
			s = s(argmax{q(D1), q(D2)}).
			leaf value : s = this.
			It points to the leaf of the subtree rooted from this node
			which will be refined at the next step of the algorithm 
**/
			BinaryNode* _s_element;
/**
			E = min{E(D1) + E(D2), e}.
			leaf value : E = e
**/
			double _E;
/**
			E~ = E*( previous E~) / (E + previous E~).
			leaf value : E~ = E
**/
			double _E_tilde;
/**
			q = min{ max{q(D1), q(D2)}, E~}. 
			leaf value : q = E~
**/
			double _q;
/**
			e~ = e * (parent e~) / (e + (parent e~)).
			leaf value : e~ = e
**/
			double _tilde_error;
	};

/**
		class implementing additional methods for a binary tree node which depend on the dimensionality.
**/
	template <size_t dim>
		class DimensionedNode : public BinaryNode
		{
			public:
/**
				default constructor
**/
				DimensionedNode() : BinaryNode(){};
/**
				default destructor
**/
				virtual ~DimensionedNode(){};
/**
				projected function computed at input point.
				It returns the value of the objective function of the algorithm,
				projected on this node, at point passed by input.
**/
				virtual double Projection(const Geometry::Point<dim>&) const = 0;
/**
				vertexes of the underliying geometry
**/
				virtual Geometry::NodesVector<dim> Nodes() const = 0;
		};


} //namespace BinaryTree
#endif //__BINARY_NODE_H
