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
				virtual double operator() (const Geometry::Point<1>& p)const override
				{
					double result(1.0);
					for(std::size_t e(0); e < exp; ++e)
						result *= p[0];
					
					return result;
				};

				virtual std::string Name()const override
				{
					return "x^" + std::to_string(exp);
				};
		};

	template <int b_mu>
		class AdvectionDiffusionSolution : public BinaryTree::Functor<1>
		{
			public:
				AdvectionDiffusionSolution(){};
				virtual double operator() (const Geometry::Point<1>& p)const override
				{
					return (exp(b_mu * p[0]) - 1) / (exp(b_mu) - 1);
				};

				virtual std::string Name()const override
				{
					return "(exp(" + std::to_string(b_mu) + "*x) - 1 ) / (exp(" + std::to_string(b_mu) + ") - 1)";
				};
		};

	using XSquared = XExpBeta<2>;

	class HalfStep : public BinaryTree::Functor<1>
	{
		public:
			HalfStep(){};
			virtual double operator() (const Geometry::Point<1>&)const override;
			virtual std::string Name()const override;
	};

	class HalfSqrt : public BinaryTree::Functor<1>
	{
		public:
			HalfSqrt(){};
			virtual double operator() (const Geometry::Point<1>&)const override;			
			virtual std::string Name()const override;
	};

	class HalfX : public BinaryTree::Functor<1>
	{
		public:
			HalfX(){};
			virtual double operator() (const Geometry::Point<1>&)const override;			
			virtual std::string Name()const override;
	};

	class HalfSquare : public BinaryTree::Functor<1>
	{
		public:
			HalfSquare(){};
			virtual double operator() (const Geometry::Point<1>&)const override;			
			virtual std::string Name()const override;
	};

	class HalfTwenty : public BinaryTree::Functor<1>
	{
		public:
			HalfTwenty(){};
			virtual double operator() (const Geometry::Point<1>&)const override;			
			virtual std::string Name()const override;
	};

	class SqrtX : public BinaryTree::Functor<1>
	{
		public:
			SqrtX(){};
			virtual double operator() (const Geometry::Point<1>&)const override;
			virtual std::string Name()const override;
	};

} //namespace MyFunctions

#endif //__INTERPOLATING_FUNCTIONS_H
