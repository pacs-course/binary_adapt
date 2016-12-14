#ifndef __BINARY_TREE_HELPER_H
#define __BINARY_TREE_HELPER_H

#include "Point.h"

#include <memory> //std::shared_ptr, std::make_shared, std::make_unique
#include <functional> //std::function

namespace BinaryTree
{
	template <std::size_t dim>
		using FunctionPtr = std::shared_ptr<std::function<double(Geometry::Point<dim>)>>;
} //namespace BinaryTree

namespace HelperFunctions
{
	template <typename ConcreteProduct, typename AbstractProduct = ConcreteProduct>
		struct Builders
		{
			static std::shared_ptr<AbstractProduct> BuildSingleton()
			{
#ifndef SINGLETON_ENABLED
				static std::shared_ptr<AbstractProduct> ptr = std::make_shared<ConcreteProduct>();			
#else
				std::shared_ptr<AbstractProduct> ptr = ConcreteProduct::Instance();
#endif //SINGLETON_ENABLED
				return ptr;
			};

			static std::unique_ptr<AbstractProduct> BuildObject()
			{
				return std::make_unique<ConcreteProduct>();
			};
		};
} //namespace HelperFunctions

#endif //__BINARY_TREE_HELPER_H
