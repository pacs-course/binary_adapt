#ifndef __CONCRETE_FACTORIES_H
#define __CONCRETE_FACTORIES_H

#include "AbstractFactory.h"
#include "TypeEnumerations.h"
#include "Maps.h"
#include "Basis.h"
#include "StdBananaFElement.h"

/*
	List of all the concrete instances of the factories useful in the library
	Full specializations of the constructors make the necessary registrations
	The possibility to add other builders is left to an external user 
*/
/* NB: It's not allowed to specialize aliases,
	so I need to replace aliases with correspondent full class names */

namespace GenericFactory
{
/*---Object Factories---*/

/*
	Factory for affine maps between an element and its correspondent std one
	The key is the type of the element to be mapped in the corresponding std element
*/
	template <std::size_t dim>
		using AffineMapFactory = ObjectFactory <Geometry::AffineMap<dim>, Geometry::ElementType>;

/* I specialize the AffineMapFactory constructor */
	template <>
		Factory <Geometry::AffineMap<1>, Geometry::ElementType, std::unique_ptr<Geometry::AffineMap<1>> >::Factory();

	template <>
		Factory <Geometry::AffineMap<2>, Geometry::ElementType, std::unique_ptr<Geometry::AffineMap<2>> >::Factory();

/*
	Factory for maps between standard elements (i.e. between reference triangle and reference square)
	The key is the type of the element to be mapped in the corresponding standard ipercube
*/
	template <std::size_t dim>
		using StdMapFactory = ObjectFactory <Geometry::Map<dim>, Geometry::ElementType>;

/* I specialize the StdMapFactory constructor */
/*		for the 1D case I don't need any map between std elements, since the stdFElem is already the ipercube */
	template <>
		Factory <Geometry::Map<2>, Geometry::ElementType, std::unique_ptr<Geometry::Map<2>> >::Factory();


/*
	Factory for basis needed by StdFElement objects
	The key is the type of the basis
*/
	template <std::size_t dim>
		using BasisFactory = ObjectFactory<FiniteElements::AbstractBasis<dim>, FiniteElements::BasisType>;

/* I specialize the BasisFactory constructor */
	template <>
		Factory <FiniteElements::AbstractBasis<1>, FiniteElements::BasisType, std::unique_ptr<FiniteElements::AbstractBasis<1>> >::Factory();

	template <>
		Factory <FiniteElements::AbstractBasis<2>, FiniteElements::BasisType, std::unique_ptr<FiniteElements::AbstractBasis<2>> >::Factory();

/*---Singleton Factories---*/

/*
	Factory for standard elements needed by FElements objects
	The key is the ElementType denoting the geometry of the object
*/
	template <std::size_t dim, FiniteElements::BasisType FeType>
		using StdFElementFactory = SingletonFactory <FiniteElements::StdBananaFElement<dim, FeType>, Geometry::ElementType>;

	template <>
		Factory <FiniteElements::StdBananaFElement<1, FiniteElements::LegendreType>,
					Geometry::ElementType,
					std::shared_ptr<FiniteElements::StdBananaFElement<1, FiniteElements::LegendreType>> >::Factory();

	template <>
		Factory <FiniteElements::StdBananaFElement<2, FiniteElements::LegendreType>,
					Geometry::ElementType,
					std::shared_ptr<FiniteElements::StdBananaFElement<2, FiniteElements::LegendreType>> >::Factory();

	template <>
		Factory <FiniteElements::StdBananaFElement<2, FiniteElements::WarpedType>,
					Geometry::ElementType,
					std::shared_ptr<FiniteElements::StdBananaFElement<2, FiniteElements::WarpedType>> >::Factory();

}

#endif //__CONCRETE_FACTORIES_H
