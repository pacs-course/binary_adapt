#ifndef __CONCRETE_FACTORIES_H
#define __CONCRETE_FACTORIES_H

#include "AbstractFactory.h"
#include "TypeEnumerations.h"
#include "Maps.h"
#include "Basis.h"
#include "StdFElementInterface.h"

/**
	List of all the concrete instances of the factories used in the library.
	Full specializations of the constructors make the necessary registrations;
	The possibility to add other builders is left to an external user.
**/
/*  NB: It's not allowed to specialize aliases,
	so I need to replace aliases with correspondent full class names */

namespace GenericFactory
{
	/*---Object Factories---*/

	/**
		Factory for affine maps between an element and its correspondent std one.
		The key is the type of the element corresponding std element has to be mapped to.
	**/
	template <size_t dim>
	using AffineMapFactory = ObjectFactory <Geometry::AffineMap<dim>,
											Geometry::ElementType>;

	/* Specializing the AffineMapFactory constructor */
	template <>
	Factory <Geometry::AffineMap<1>,
			 Geometry::ElementType,
			 std::unique_ptr<Geometry::AffineMap<1>>>::Factory();

	template <>
	Factory <Geometry::AffineMap<2>,
			 Geometry::ElementType,
			 std::unique_ptr<Geometry::AffineMap<2>>>::Factory();

	/**
		Factory for maps between standard elements (i.e. between reference triangle and reference square).
		The key is the type of the element corresponding standard ipercube has to be mapped to.
		For the 1D case no map between standard elements is needed, since the standard element is already the ipercube.
	**/
	template <size_t dim>
	using StdMapFactory = ObjectFactory <Geometry::Map<dim>,
										 Geometry::ElementType>;

	/* Specializing the StdMapFactory constructor */
	template <>
	Factory <Geometry::Map<2>,
			 Geometry::ElementType,
			 std::unique_ptr<Geometry::Map<2>>>::Factory();


	/**
		Factory for basis needed by StdFElement objects.
		The key is the type of the basis.
	**/
	template <size_t dim>
	using TensorialBasisFactory = ObjectFactory <FiniteElements::TensorialBasis<dim>,
												 FiniteElements::BasisType>;

	/*	Specializing the BasisFactory constructor */
	template <>
	Factory <FiniteElements::TensorialBasis<1>,
			 FiniteElements::BasisType,
			 std::unique_ptr <FiniteElements::TensorialBasis<1>>>::Factory();

	template <>
	Factory <FiniteElements::TensorialBasis<2>,
			 FiniteElements::BasisType,
			 std::unique_ptr<FiniteElements::TensorialBasis<2>>>::Factory();

	/*---Singleton Factories---*/

	/**
		Factory for standard elements needed by FElements objects.
		The key is the ElementType denoting the geometry of the object.
		The factory ensures that only one standard element is constructed.
	**/
	template <size_t dim, FiniteElements::BasisType FeType>
	using StdFElementFactory =
		  SingletonFactory <FiniteElements::StdFElementInterface<dim, FeType>,
							Geometry::ElementType>;

	/*	Specializing the StdFElementFactory constructor */
	template <>
	Factory <FiniteElements::StdFElementInterface<1, FiniteElements::LegendreType>,
			 Geometry::ElementType,
			 std::shared_ptr <FiniteElements::StdFElementInterface <1,FiniteElements::LegendreType>>>::Factory();

	template <>
	Factory <FiniteElements::StdFElementInterface <2, FiniteElements::LegendreType>,
			 Geometry::ElementType,
			 std::shared_ptr<FiniteElements::StdFElementInterface<2, FiniteElements::LegendreType>>>::Factory();

	template <>
	Factory <FiniteElements::StdFElementInterface<2, FiniteElements::WarpedType>,
			 Geometry::ElementType,
			 std::shared_ptr<FiniteElements::StdFElementInterface<2, FiniteElements::WarpedType>>>::Factory();

}

#endif //__CONCRETE_FACTORIES_H
