#ifndef __STD_BANANA_F_ELEMENT_H
#define __STD_BANANA_F_ELEMENT_H

#include "StdElement.h"
#include "AbstractSpace.h"

using namespace std;
using namespace Geometry;

namespace FiniteElements
{
/*
	This is a hat class for StdFElement which represents the return type of the std FElements factory
	It must not depend on the ElementType template parameter, since it is known at runtime
	Moreover it must guarantee the AbstractSpace_Interface interface
*/
	template <size_t dim, BasisType FeType = InvalidFeType>
		class StdBananaFElement : public AbstractSpace_Interface<dim>, public AbstractElement<dim>
		{
			public:
				virtual ~StdBananaFElement(){};
/*
				AbstractSpace_Interface methods
*/
				virtual double	EvaluateBasisFunction (size_t ind, const Point<dim>& point)const = 0;
				virtual size_t	BasisSize (size_t)const = 0;
				virtual BasisType GetFeType ()const = 0;

				/* for possible optimizations */
				virtual vector<double> EvaluateBasis (size_t degree, const Point<dim>& point)const = 0;

/*
				AbstractElement methods
*/
				virtual ElementType GetType() const = 0;
				virtual QuadPointVec<dim> GetQuadPoints()const = 0;
				virtual QuadWeightVec GetQuadWeights()const = 0;
				virtual size_t QuadratureOrder()const = 0;
		};

} //namespace FiniteElements

#endif //__STD_BANANA_F_ELEMENT_H
