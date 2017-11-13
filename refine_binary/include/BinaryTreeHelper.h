#ifndef __BINARY_TREE_HELPER_H
#define __BINARY_TREE_HELPER_H

#include "TypeEnumerations.h"

#include <memory> //std::shared_ptr, std::make_shared, std::make_unique
#include <functional> //std::function


namespace HelperFunctions
{
	template <class T, class... Args>
	std::unique_ptr<T> MakeUnique(Args&&... args)
	{
#ifdef __cpp_lib_make_unique
		return std::make_unique<T>(args...);
#else
		return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
#endif
	};

	template <typename ConcreteProduct, typename AbstractProduct = ConcreteProduct>
		struct Builders
		{
			static std::shared_ptr<AbstractProduct> BuildSingleton()
			{
				static std::shared_ptr<AbstractProduct> ptr = std::make_shared<ConcreteProduct>();
				return ptr;
			};

			static std::unique_ptr<AbstractProduct> BuildObject()
			{
				return HelperFunctions::MakeUnique<ConcreteProduct>();
			};
		};

	//Functions to convert my enums into strings
	std::string BasisTypeConverter(FiniteElements::BasisType const & id);
	std::string ElementTypeConverter(Geometry::ElementType const & id);

	/**
		An optimized function to compute powers when the exponent is an unsigned integer
		and it is known at runtime
	**/
	double IntPower (double b, size_t e);

	/**
		An optimized function to compute powers when the exponent is an unsigned integer
		and it is known at compile time
	**/
	template <size_t exp>
		double Power (double basis)
		{
			double result = 
			exp % 2 ? basis * Power<exp/2> (basis*basis) : Power<exp/2> (basis*basis);
			return result;
		};

	template <>
		double Power<0> (double);

	template <>
		double Power<1> (double);

	template <>
		double Power<2> (double);
	

} //namespace HelperFunctions

#endif //__BINARY_TREE_HELPER_H
