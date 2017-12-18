#ifndef __ABSTRACTSPACE_H
#define __ABSTRACTSPACE_H

#include "TypeEnumerations.h"
#include "LinearAlgebra.h"


namespace FiniteElements
{
	/**
		Abstract class for spaces that store a basis.
	**/
	template <size_t dim>
	class AbstractSpaceInterface
	{
	  public:
		/**
			default destructor
		**/
		virtual ~AbstractSpaceInterface()
		{};

		/**
			Get the evaluation of the underlying basis.
			Input parameters:
				- the index of the basis function to be evaluated;
				- the point where the basis function has to be evaluated.
		**/
		virtual double	EvaluateBasisFunction (std::size_t ind,
											   const Geometry::Point<dim>& point) const = 0;

		/**
			The number of functions of the underlying basis
		**/
		virtual std::size_t BasisSize (std::size_t)const = 0;

		/**
			The identifier of the underlying basis type
		**/
		virtual BasisType GetFeType()const = 0;
	};

} //namespace FiniteElements

#endif //__ABSTRACTSPACE_H
