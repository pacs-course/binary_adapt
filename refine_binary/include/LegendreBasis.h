#ifndef __LEGENDRE_BASIS_H
#define __LEGENDRE_BASIS_H

#include "Basis.h"

#include <memory> //std::shared_ptr

//jacobi_polynomial.hpp need the "using namespace std" before the include
//TODO: I'm in a header file, I don't want to put "using namespace std" outside the namespace FiniteElements
using namespace std;
#include "jacobi_polynomial.hpp"

namespace FiniteElements
{
/**
	Class implementing Legendre basis of dimensionality dim.
	It has the property of being orthogonal on ipercubes wrt the L^2 scalar product.
	See:
		Quarteroni, "Numerical Models for Differential Problems", 2nd edition; p 229-232
	for details.
**/
	template <size_t dim>
		class LegendreBasis : public TensorialBasis<dim>
		{
			public:
/**
				default constructor
**/
				LegendreBasis();
/**
				default destructor
**/
				virtual ~LegendreBasis();
/**
				copy constructor
**/
				LegendreBasis(const LegendreBasis&);
/**
				assignment operator
**/
				LegendreBasis& operator = (const LegendreBasis&);

/**
				Evaluate functions until input degree at fixed point.
				Input parameters:
					- the degree of the basis: all basis functions with degree not higher than input
						will be evaluated;
					- evaluation point.

				This is the function recommended to get basis evaluation values,
				since in the Legendre case, thanks to the jacobi_polynomial working principle,
				the method is more optimized.
**/
				virtual Geometry::Vector EvaluateBasis(size_t, const Geometry::Point<dim>&) override;

			protected:
/**
				Evaluate 1D basis function at input point.
				Input parameters:
					- index of the basis function to be evaluated
					- 1D evaluation point
				This method is inefficient, since because of the jacobi_polynomial
				implementation it's not possible to evaluate one function without
				evaluating all the previous ones (i.e. all basis functions with lower degree);
				so its use is deprecated.
**/
				virtual double OneDEvaluation(size_t, double) const override;
		};

	template <size_t dim>
		LegendreBasis<dim>::LegendreBasis() : TensorialBasis<dim>()
		{};

	template <size_t dim>
		LegendreBasis<dim>::~LegendreBasis()
		{};

	template <size_t dim>
		LegendreBasis<dim>& LegendreBasis<dim>::operator = (const LegendreBasis<dim>& input_basis)
		{
			if (this != &input_basis)
			{
				TensorialBasis<dim>::Copy(input_basis);
			}
			return *this;
		};

	template <size_t dim>
		LegendreBasis<dim>::LegendreBasis(const LegendreBasis& input_basis)
		{
			*this = input_basis;
		};

#ifdef INTERNAL_LEGENDRE
	unique_ptr<double[]> ManualOneDLegendreEvaluation(size_t index, double x)
	{
		unique_ptr<double[]> result(new double[index+1]);
		double previous_value = 1.0;
		double value = x;
		result[0] = previous_value;
		if (index >= 1)
			result[1] = value;
			
		for (size_t k = 1; k < index; ++k)
		{
			double k_val = static_cast<double>(k);
			double temp = previous_value;
			previous_value = value;
			value = ( (2*k_val + 1) * x * value - k_val * temp) / (k_val + 1);
			result[k + 1] = value;
		}
		return move(result);
	};
#endif //INTERNAL_LEGENDRE

	template <size_t dim>
		Geometry::Vector LegendreBasis<dim>::EvaluateBasis(size_t degree, const Geometry::Point<dim>& p)
		{
			size_t length = this->ComputeSize(degree);
			this->UpdateSize(length);
			std::vector<std::unique_ptr<double[]>> evaluations;
			auto p_iter = p.begin();
			for(size_t i(0); i < dim; ++i)
			{
				double x = *(p_iter++);
#ifndef INTERNAL_LEGENDRE
				evaluations.push_back(std::move(std::unique_ptr<double[]> (j_polynomial(1, degree, 0, 0, &x))));
#else
				evaluations.push_back(std::move(ManualOneDLegendreEvaluation(degree, x)));
#endif
			}

			Geometry::Vector result(length);
			for( size_t i(0); i < length; ++i)
			{
				double tot(1);
				for (size_t j(0); j < dim; ++j)
					tot *= evaluations[j][this->_tensorial_indexes[i][j]];
				result[i] = tot;
			};

			return result;
		};

	template <size_t dim>
		double LegendreBasis<dim>::OneDEvaluation(size_t index, double x) const
		{
#ifdef VERBOSE
			static bool called = true;
			if (called)
			{
				cerr << "Warning: OneDEvaluation method in LegendreBasis is deprecated because of inefficiency" << endl;
				called = false;
			}
#endif //VERBOSE
#ifndef INTERNAL_LEGENDRE 
			unique_ptr<double[]> eval(j_polynomial(1, index, 0, 0, &x));
#else
			unique_ptr<double[]> eval = move(ManualOneDLegendreEvaluation(index, x));
#endif
			return eval[index];
		};

} //namespace FiniteElements
#endif //__LEGENDRE_BASIS_H
