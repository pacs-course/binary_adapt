#ifndef __WARPED_BASIS_H
#define __WARPED_BASIS_H

#include "Basis.h"


namespace FiniteElements
{
	/**
		Class implementing warped tensor product basis.
		It has the property of being orthogonal on triangles wrt the L^2 scalar product.
		See:
			Quarteroni, "Numerical Models for Differential Problems", 2nd edition; p 261-263
		for details.
	**/
	class WarpedBasis : public TensorialBasis<2>
	{
	  public:
		/**
			default constructor
		**/
		WarpedBasis();
		/**
			default destructor
		**/
		virtual ~WarpedBasis();
		/**
			copy constructor
		**/
		WarpedBasis (const WarpedBasis&);
		/**
			assignment operator
		**/
		WarpedBasis& operator= (const WarpedBasis&);

		/**
			This basis is not the result of a simple tensorization,
			so a 1D version of it is not available.
			If OneDEvaluation() is accessed it means that something has gone wrong;
			an exception is raised in the case.
		**/
		virtual double OneDEvaluation (size_t, double)const override;
		/**
			Evaluate one function at input point.
			Input parameters:
				- index of the basis function to be evaluated
				- evaluation point
			This method is inefficient, since because of the jacobi_polynomial
			implementation it's not possible to evaluate one function without
			evaluating all the previous ones (i.e. all basis functions with lower degree);
			so its use is deprecated.
		**/
		virtual double Evaluate (size_t, const Geometry::Point<2>&) override;
		/**
			Evaluate functions until input degree at fixed point.
			Input parameters:
				- the degree of the basis: all basis functions with degree not higher than input
					will be evaluated;
				- evaluation point.

			This is the function recommended to get basis evaluation values.
		**/
		virtual Geometry::Vector EvaluateBasis (size_t,
												const Geometry::Point<2>&) override;
	};

}//namespace FiniteElements

#endif //__WARPED_BASIS_H
