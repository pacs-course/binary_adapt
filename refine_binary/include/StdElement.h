#ifndef __STD_ELEMENT_H
#define __STD_ELEMENT_H

#include "AbstractElement.h"
#include "Maps.h"
#include "Quadrature.h"

#include <utility> //move


namespace Geometry
{
	using namespace std;

	template <size_t dim, ElementType Type = InvalidElementType>
		class StdElement : public AbstractElement<dim>
		{
			public:
				StdElement()
				{
					auto& quad_factory = QuadratureFactory<dim>::Instance();
#ifdef MYDEBUG
					clog << "Cerco nella factory QuadratureFactory<" << dim << "> la quadrature_rule con chiave " << Type << endl;
					clog << "Indirizzo factory: " << &quad_factory << endl;
#endif //MYDEBUG
					_quadrature_rule = move (quad_factory.create(Type));
#ifdef MYDEBUG
					clog << "Quadrature_rule creata correttamente" << endl;
#endif //MYDEBUG
				};

			public:
				virtual ~StdElement()
				{
					//TODO
#ifdef DESTRUCTOR_ALERT
				cout << "Distruggo StdElement" << endl;
#endif //DESTRUCTOR_ALERT
				};

				virtual ElementType GetType() const
				{
					return Type;
				};

				//TODO: verify if it's convenient to store the quadrature nodes and weights as an attribute of the element
				virtual QuadPointVec<dim> GetQuadPoints()const
				{
					return _quadrature_rule->GetPoints();
				};

				virtual QuadWeightVec GetQuadWeights()const
				{
					return _quadrature_rule->GetWeights();
				};

				virtual size_t QuadratureOrder()const
				{
					return _quadrature_rule->Order();
				};

			protected:
				unique_ptr<QuadratureRuleInterface<dim> > _quadrature_rule;
		};

/*
	I don't have a general definition of std ipercube geometry since I depend on the libMesh geometries, which are defined only in 1, 2 and 3 dimensions.
	In a more general setting, it's not a priori excluded the possibility to define my std ipercube, on which the quadrature nodes and weigths are constructed by tensorization of the 1d counterparts; this is similarly already done for basis functions.
	However, in this case the default StdIperCube is set as InvalidElement, so the respectively stdInterval, stdSquare, stdCube are defined through specialization on the dim parameter.
*/
namespace detail
{
	template <size_t dim>
	struct alias
	{
		using AliasIperCube = StdElement <dim, InvalidElementType>;
	};

	template <>
	struct alias<1>
	{
		using AliasIperCube = StdElement <1, IntervalType>;
	};

	template <>
	struct alias<2>
	{
		using AliasIperCube = StdElement <2, SquareType>;
	};

	template <>
	struct alias<3>
	{
		using AliasIperCube = StdElement <3, CubeType>;
	};
}//namespace detail

	template <size_t dim>
		using StdIperCube = typename detail::alias<dim>::AliasIperCube;

};//namespace Geometry
#endif //__STD_ELEMENT_H
