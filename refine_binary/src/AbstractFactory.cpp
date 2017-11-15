#include "AbstractFactory.h"

namespace GenericFactory
{
	template<>
		string identifierAsString<Geometry::ElementType>(Geometry::ElementType const & id)
		{
			return Helpers::ElementTypeConverter(id);
		};
	template<>
		string identifierAsString<FiniteElements::BasisType>(FiniteElements::BasisType const & id)
		{
			return Helpers::BasisTypeConverter(id);
		};
} //namespace GenericFactory


