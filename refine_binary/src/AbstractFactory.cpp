#include "AbstractFactory.h"

namespace GenericFactory
{
	template<>
		string identifierAsString<Geometry::ElementType>(Geometry::ElementType const & id)
		{
			return HelperFunctions::ElementTypeConverter(id);
		};
	template<>
		string identifierAsString<FiniteElements::BasisType>(FiniteElements::BasisType const & id)
		{
			return HelperFunctions::BasisTypeConverter(id);
		};
} //namespace GenericFactory


