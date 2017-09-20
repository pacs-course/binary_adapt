#ifndef __FUNCTOR_H
#define __FUNCTOR_H

#include "Point.h"
#include "AbstractFactory.h"

namespace Banana
{
	void TestFun();
}

namespace BinaryTree
{
/*
	This is the base class for the functions to be interpolated in the algorithm
	Binary elements take in input a shared_ptr to this base class
	Concrete functor objects are implemented in dynamic library, so they can be loaded at runtime
*/
	template <std::size_t dim>
		class Functor
		{
			public:
				Functor(){};
				virtual double operator() (const Geometry::Point<dim>& p) const = 0;
				virtual std::string Formula()const = 0;
				virtual std::string ID()const = 0;
		};

/*
	Factory for quadrature rules
	The key is a string which identifies the function
	The registration is done by the dynamic library implementing the concrete functors
*/
	template <std::size_t dim>
		using FunctionsFactory = GenericFactory::ObjectFactory <Functor<dim>, std::string>;

	template <std::size_t dim>
		using FunctionPtr = std::shared_ptr<Functor<dim>>;

} //namespace BinaryTree
#endif //__FUNCTOR_H
