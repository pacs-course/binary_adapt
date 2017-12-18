#ifndef __FUNCTOR_H
#define __FUNCTOR_H

#include "LinearAlgebra.h"
#include "AbstractFactory.h"

namespace BinaryTree
{
	/**
		This is the base class for the functions to be interpolated in the algorithm.
		Binary elements take as input a shared_ptr to Functor.
		Concrete functor objects are implemented in dynamic library, so they can be loaded at runtime.
	**/
	template <std::size_t dim>
	class Functor
	{
	  public:
		/**
			default constructor
		**/
		Functor() {};
		/**
			default destructor
		**/
		virtual ~Functor() {};

		/**
			call operator.
			It takes as input the Point where the functor has to be evaluated.
		**/
		virtual double operator() (const Geometry::Point<dim>& p) const = 0;
		/**
			The mathematical expression implented by the functor.
		**/
		virtual std::string Formula() const = 0;
		/**
			A printable identifier of the class.
		**/
		virtual std::string ID() const = 0;
	};

	/**
		Factory for Functor objects.
		The key is a string which identifies the function.
		The registration is done at runtime by the dynamic library implementing the concrete functors.
	**/
	template <std::size_t dim>
	using FunctionsFactory =
		GenericFactory::ObjectFactory <Functor<dim>, std::string>;

	template <std::size_t dim>
	using FunctionPtr = std::shared_ptr<Functor<dim>>;

} //namespace BinaryTree
#endif //__FUNCTOR_H
