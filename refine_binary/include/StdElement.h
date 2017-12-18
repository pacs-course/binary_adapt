#ifndef __STD_ELEMENT_H
#define __STD_ELEMENT_H

#include "AbstractElement.h"
#include "Maps.h"
#include "Quadrature.h"

#include <utility> //std::move


namespace Geometry
{
	using namespace std;

	/**
		Class for reference elements.
		It represents the standard element for elements with ElementType = Type
		It stores the quadrature rule, which gives quadrature nodes and weights.
	**/
	template <size_t dim, ElementType Type = InvalidElementType>
	class StdElement : public AbstractElement<dim>
	{
	  public:
		/**
			constructor.
			It accesses quadrature rule factory with Type key to get the rule.
		**/
		StdElement();

	  public:
		/**
			default destructor
		**/
		virtual ~StdElement();

		/**
			The identifier of the geometry
		**/
		virtual ElementType GetType() const;

		/**
			The quadrature points of the standard element
		**/
		virtual QuadPointVec<dim> GetQuadPoints() const;

		/**
			The quadrature weights of the standard element
		**/
		virtual QuadWeightVec GetQuadWeights() const;

		/**
			The exactness order of the quadrature rule
		**/
		virtual size_t QuadratureOrder() const;

	  protected:
		/**
			The quadrature rule
		**/
		unique_ptr<QuadratureRuleInterface<dim>> _quadrature_rule;
	};

	template <size_t dim, ElementType Type>
	StdElement<dim, Type>::StdElement()
	{
		auto& quad_factory = QuadratureFactory<dim>::Instance();
		_quadrature_rule = move (quad_factory.create (Type));
	};

	template <size_t dim, ElementType Type>
	StdElement<dim, Type>::~StdElement()
	{};

	template <size_t dim, ElementType Type>
	ElementType StdElement<dim, Type>::GetType() const
	{
		return Type;
	};

	template <size_t dim, ElementType Type>
	QuadPointVec<dim> StdElement<dim, Type>::GetQuadPoints() const
	{
		//TODO: verify if it's convenient to store
		//the quadrature nodes and weights as an attribute of the element
		return _quadrature_rule->GetPoints();
	};

	template <size_t dim, ElementType Type>
	QuadWeightVec StdElement<dim, Type>::GetQuadWeights() const
	{
		return _quadrature_rule->GetWeights();
	};

	template <size_t dim, ElementType Type>
	size_t StdElement<dim, Type>::QuadratureOrder() const
	{
		return _quadrature_rule->Order();
	};


	/*
		I don't have a general definition of std ipercube geometry,
		since geometries are taken from external libraries,
		which are defined only in 1, 2 and 3 dimensions.

		In a more general setting,
		it's not a priori excluded the possibility to define my std ipercube,
		on which the quadrature nodes and weigths
		are constructed by tensorization of the 1d counterparts;
		this is similarly already done for basis functions.

		However, in this case the default StdIperCube is set as InvalidElement,
		so the respectively stdInterval, stdSquare, stdCube
		are defined through specialization on the dim parameter.
	*/
	namespace detail
	{
		/**
			Ipercube aliases.
			Specialization of template using is not allowed,
			so this struct pattern is needed.
		**/
		template <size_t dim>
		struct alias
		{
			using AliasIperCube = StdElement <dim, InvalidElementType>;
		};

		/**
			The standard interval
		**/
		template <>
		struct alias<1>
		{
			using AliasIperCube = StdElement <1, IntervalType>;
		};

		/**
			The standard square
		**/
		template <>
		struct alias<2>
		{
			using AliasIperCube = StdElement <2, SquareType>;
		};

		/**
			The standard cube
		**/
		template <>
		struct alias<3>
		{
			using AliasIperCube = StdElement <3, CubeType>;
		};
	}//namespace detail

	/**
		The standard ipercube
	**/
	template <size_t dim>
	using StdIperCube = typename detail::alias<dim>::AliasIperCube;

};//namespace Geometry
#endif //__STD_ELEMENT_H
