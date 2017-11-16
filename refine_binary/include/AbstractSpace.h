#ifndef __ABSTRACTSPACE_H
#define __ABSTRACTSPACE_H

#include "TypeEnumerations.h"
#include "LinearAlgebra.h"


namespace FiniteElements
{
	template <size_t dim>
		class AbstractSpace_Interface
		{
			public:
				virtual ~AbstractSpace_Interface()
				{
#ifdef DESTRUCTOR_ALERT
				cout << "Distruggo AbstractSpace_Interface" << endl;
#endif //DESTRUCTOR_ALERT
				};

				virtual double	EvaluateBasisFunction(std::size_t ind, const Geometry::Point<dim>& point)const = 0;

				virtual std::size_t	BasisSize(std::size_t)const = 0;

				virtual BasisType GetFeType()const = 0;
		};

} //namespace FiniteElements

#endif //__ABSTRACTSPACE_H
