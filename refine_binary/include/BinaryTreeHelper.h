#ifndef __BINARY_TREE_HELPER_H
#define __BINARY_TREE_HELPER_H

#include "TypeEnumerations.h"

#include <memory>//shared_ptr, make_shared, make_unique
#include <functional> //function
#include <iostream>
#include <string> //string



/**
	Helpful tools, classes and functions needed by the library objects.
**/
namespace Helpers
{
	/**
		Function behaving as make_unique.
		MakeUnique gives an interface for a function which behaves exactly as
		make_unique [available since standard c++14].
		If the compiling options make available the make_unique,
		it simply returns its output.
	**/
	template <class T, class... Args>
	std::unique_ptr<T> MakeUnique (Args&& ... args)
	{
#ifdef __cpp_lib_make_unique
		return std::make_unique<T> (args...);
#else
		return std::unique_ptr<T> (new T (std::forward<Args> (args)...));
#endif
	}

	/**
		Builders to be registered in the factories
	**/
	template <typename ConcreteProduct, typename AbstractProduct = ConcreteProduct>
	struct Builders
	{
		/**
			It makes sure that only one element is constructed.
		**/
		static std::shared_ptr<AbstractProduct> BuildSingleton()
		{
			static std::shared_ptr<AbstractProduct> ptr =
				std::make_shared<ConcreteProduct>();
			return ptr;
		};

		static std::unique_ptr<AbstractProduct> BuildObject()
		{
			return MakeUnique<ConcreteProduct>();
		};
	};

	/**
		Function to convert BasisType enum into string
	**/
	std::string BasisTypeConverter (FiniteElements::BasisType const& id);
	/**
		Function to convert ElementType enum into string
	**/
	std::string ElementTypeConverter (Geometry::ElementType const& id);

	/**
		An optimized function to compute powers when the exponent is
		an unsigned integer and it is known at runtime
	**/
	inline double IntPower (double basis, size_t exp)
	{
		if (exp == 0)
			return 1.0;
		if (exp == 1)
			return basis;
		if (exp == 2)
			return basis * basis;

		double half_exp_result = IntPower (basis * basis, exp / 2);
		return exp % 2 ? basis * half_exp_result : half_exp_result;
	}

	/**
		An optimized function to compute powers when the exponent is
		an unsigned integer and it is known at compile time
	**/
	template <size_t exp>
	double Power (double basis)
	{
		double result =
			exp % 2 ? basis * Power < exp / 2 > (basis * basis) : Power < exp / 2 >
			(basis * basis);
		return result;
	}

	template <>
	double Power<0> (double);

	template <>
	double Power<1> (double);

	template <>
	double Power<2> (double);


} //namespace Helpers

#endif //__BINARY_TREE_HELPER_H
