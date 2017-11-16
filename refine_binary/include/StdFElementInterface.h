#ifndef __STD_BANANA_F_ELEMENT_H
#define __STD_BANANA_F_ELEMENT_H

#include "StdElement.h"
#include "AbstractSpace.h"


namespace FiniteElements
{
	using namespace std;

/*
	This is a hat class for StdFElement which represents the return type of the std FElements factory
	It must not depend on the ElementType template parameter, since it is known at runtime
	Moreover it must guarantee the AbstractSpace_Interface interface
*/
	template <size_t dim, BasisType FeType = InvalidFeType>
		class StdFElementInterface : public AbstractSpace_Interface<dim>, public Geometry::AbstractElement<dim>
		{
			public:
/*
				destructor.
*/
				virtual ~StdFElementInterface(){};
/*
				AbstractSpace_Interface methods
*/
				virtual double	EvaluateBasisFunction (size_t ind, const Geometry::Point<dim>&)const = 0;
				virtual size_t	BasisSize (size_t)const = 0;
				virtual BasisType GetFeType ()const = 0;

/*
				AbstractElement methods
*/
				virtual Geometry::ElementType GetType() const = 0;
				virtual Geometry::QuadPointVec<dim> GetQuadPoints()const = 0;
				virtual Geometry::QuadWeightVec GetQuadWeights()const = 0;
				virtual size_t QuadratureOrder()const = 0;

				/* from now on stuff introduced with optimization purpose */

				virtual Geometry::Vector EvaluateBasis (size_t degree, const Geometry::Point<dim>&)const = 0;
/**
				The square of L2 norm of basis function with input index.
				Method implementing optimized norm computation of basis functions.
				If the value is already stored in _norm_values attribute, it returns it,
				otherwise it computes it through the pure virtual ComputeNormSquared method,
				which will be defined by derived classes.
**/
				double BasisNormSquared(size_t);

			protected:
/*
				constructor.
*/
				StdFElementInterface();
				virtual double ComputeNormSquared(size_t) = 0;
				virtual void UpdateNorms(size_t);

			protected:
				Geometry::Vector _norm_values;
		};

	template <size_t dim, BasisType FeType>
		StdFElementInterface<dim, FeType>::StdFElementInterface() : _norm_values(){};

	template <size_t dim, BasisType FeType>
		double StdFElementInterface<dim, FeType>::BasisNormSquared(size_t ind)
		{
			UpdateNorms(ind);

			return (this->_norm_values)[ind];
		};

	template <size_t dim, BasisType FeType>
		void StdFElementInterface<dim, FeType>::UpdateNorms(size_t ind)
		{

			auto l = (this->_norm_values).Size();
			auto new_l = ind + 1;

			if (l < new_l)
				(this->_norm_values).Resize(new_l);

			for (size_t i = l; i < new_l; ++i)
				(this->_norm_values)[i] = this->ComputeNormSquared(i);
		};


} //namespace FiniteElements

#endif //__STD_BANANA_F_ELEMENT_H
