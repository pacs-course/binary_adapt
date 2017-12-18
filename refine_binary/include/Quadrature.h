#ifndef __QUADRATURE_H
#define __QUADRATURE_H

#include "LinearAlgebra.h"
#include "TypeEnumerations.h"
#include "AbstractFactory.h"

namespace Geometry
{
	/**
		Type used to store quadrature nodes
	**/
	template <size_t dim>
	using QuadPointVec = VectorPoint<dim>;
	/**
		Type used to store quadrature weigths
	**/
	using QuadWeightVec = ColumnVector;

	/**
		A wrapper class for quadrature rules.
		A quadrature rule should provide a vector of quadrature nodes
		and a vector of quadrature weights, both meant to be defined
		on the reference element of the Type the quadrature refers to.
		Reference elements considered are listed in Maps.h
	**/
	template <size_t dim>
	class QuadratureRuleInterface
	{
	  public:
		/**
			default destructor
		**/
		virtual ~QuadratureRuleInterface() {};
		/**
			Get quadrature points
		**/
		virtual QuadPointVec<dim> GetPoints() = 0;
		/**
			Get quadrature weights
		**/
		virtual QuadWeightVec GetWeights() = 0;

		/**
			Get the order of exactness of the rule.
		**/
		size_t Order()
		{
			return this->_order;
		};

	  protected:
		/**
			constructor
		**/
		QuadratureRuleInterface() : _order (0)
		{};

	  protected:
		/**
			The order of exactness of the quadrature rule
		**/
		size_t _order;
	};

	/**
		Factory for quadrature rules.
		The key is the type of the element the quadrature rule is constructed on.
		The registration is done at runtime by the dynamic library implementing the rule.
	**/
	template <size_t dim>
	using QuadratureFactory =
		GenericFactory::ObjectFactory <QuadratureRuleInterface<dim>, ElementType>;
} //namespace Geometry

#endif //__QUADRATURE_H
