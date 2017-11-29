#ifndef __ABSTRACT_H
#define __ABSTRACT_H

#include "Basis.h"
#include "Quadrature.h"
#include "Functor.h"

#include <functional> //function

#ifdef TRY_IT
//TODO: use numeric_limits
#define TOLL 1E-18
#endif //TRY_IT

namespace Geometry
{
	using namespace std;

/**
	Abstract class for elements that can do integration.
	AbstractElement is characterized by its geometric type ElementType,
	which at this level is unknown.
	It provides methods to compute integrals on it.
**/
	template <size_t dim>
		class AbstractElement
		{
			public:
/**
				default destructor
**/
				virtual ~AbstractElement();

/**
				Geometric type of the object
**/
				virtual ElementType GetType() const = 0;

/**
				The quadrature nodes.
				Method declared pure virtual, since quadrature nodes
				depend on the ElementType of the object, which is unknown at this level.
**/
				virtual QuadPointVec<dim> GetQuadPoints() const = 0;

/**
				The quadrature weights.
				Method declared pure virtual, since quadrature weights
				depend on the ElementType of the object, which is unknown at this level.
**/
				virtual QuadWeightVec GetQuadWeights() const = 0;

/**
				Exactness level of underlying quadrature rule.
**/
				virtual size_t QuadratureOrder() const = 0;

/**
				Integrate input function on the element.
**/
				double Integrate(const function<double(Point<dim>)>& f) const;

/**
				Integrate a multifunction.
				It takes as input a multifunction:

					x -> (f_1(x), f_2(x), f_3(x),...)

				returning:

					(I(f_1), I(f_2), I(f_3),...)

				where I(f_i) is the integral of the f_i function.
**/
				ColumnVector MultiIntegrate(const function<ColumnVector(Point<dim>)>& multi_f) const;

/**
				L^2 scalar product of input functions. 
**/
				template <typename F1, typename F2>
				double L2Prod (const F1& f, const F2& g) const;

/**
				L^2 norm of input function.
**/
				template <typename F>
				double L2Norm (const F& f) const;

#ifdef TRY_IT
				protected:
					bool CheckSimmetry(const QuadWeightVec& vec) const
					{
						int length = vec.Size();
						int i = 0, j = length - 1;
						for(; i < length / 2; ++i, --j)
							if ( abs(vec(i) - vec(j)) > TOLL )
								return false;

						return true;
					};
#endif //TRY_IT
		};

	template <size_t dim>
		AbstractElement<dim>::~AbstractElement()
		{};

	template <size_t dim>
		double AbstractElement<dim>::Integrate(const function<double(Point<dim>)>& f) const
		{
			QuadWeightVec weights = GetQuadWeights();
			QuadWeightVec fval(weights.Size());
			QuadPointVec<dim> nodes = GetQuadPoints();

			//quadrature nodes and weights must have the same length
			if (nodes.Size() != weights.Size() )
				throw std::length_error("Trying to integrate with different number of nodes and weights!");

			//TODO: can be optimized using weights simmetry
			for (size_t i = 0; i < nodes.Size(); ++i)
			{
				auto p = nodes[i];
				fval[i] = f(p);
			}

			return fval.Dot(weights);
		};

	template <size_t dim>
		ColumnVector AbstractElement<dim>::MultiIntegrate(const function<ColumnVector(Point<dim>)>& multi_f) const
		{
			QuadWeightVec weights = GetQuadWeights();
			QuadPointVec<dim> nodes = GetQuadPoints();

			//quadrature nodes and weights must have the same length
			if (nodes.Size() != weights.Size() )
				throw std::length_error("Trying to integrate with different number of nodes and weights!");

			ColumnVector first_values = multi_f(nodes[0]);
			size_t r = first_values.Size();
			size_t c = nodes.Size();
			DynamicMatrix multi_fval(r, c);

			multi_fval.SetCol(0, first_values);

			//TODO: can be optimized using weights simmetry
			for (size_t i = 1; i < c; ++i)
			{
				auto p = nodes[i];
				ColumnVector values = multi_f(p);
				multi_fval.SetCol(i, values);
			}

			return (multi_fval * weights);
		};

	template <size_t dim>
		template <typename F1, typename F2>
		double AbstractElement<dim>::L2Prod (const F1& f, const F2& g) const
		{
			return Integrate([&](const Point<dim>& p){return f(p)*g(p);});
		};

	template <size_t dim>
		template <typename F>
		double AbstractElement<dim>::L2Norm (const F& f) const
		{
			return sqrt(L2Prod(f,f));
		};

} //namespace Geometry

#endif //__ABSTRACT_H
