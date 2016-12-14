#ifndef __INTERPOLATING_FUNCTIONS_H
#define __INTERPOLATING_FUNCTIONS_H

#include "Functor.h"

namespace MyFunctions
{
	template <int exp>
		class XExpBeta : public BinaryTree::Functor<1>
		{
			public:
				XExpBeta(){};
				virtual double operator() (const Geometry::Point<1>& p)override
				{
					double result(1.0);
					for(std::size_t e(0); e < exp; ++e)
						result *= p[0];
					
					return result;
				};
		};

	using XSquared = XExpBeta<2>;

	class SqrtX : public BinaryTree::Functor<1>
	{
		public:
			SqrtX(){};
			virtual double operator() (const Geometry::Point<1>&)override;
	};

} //namespace MyFunctions

#endif //__INTERPOLATING_FUNCTIONS_H
