#include "ConcreteFactories.h"
#include "BinaryTreeHelper.h"
#include "LegendreBasis.h"
#include "WarpedBasis.h"
#include "StdFElement.h"

using namespace Geometry;
using namespace FiniteElements;
using namespace std;

namespace GenericFactory
{
	template <>
		Factory <AffineMap<1>,
					ElementType,
					unique_ptr<AffineMap<1>> >::Factory()
		{
			add(	IntervalType,
					&Helpers::Builders <IntervalMap,
														 AffineMap<1>
														>::BuildObject
				);
		};

	template <>
		Factory <AffineMap<2>,
					ElementType,
					unique_ptr<AffineMap<2>> >::Factory()
		{
			add(	TriangleType,
					&Helpers::Builders <TriMap,
														 AffineMap<2>
														>::BuildObject
				);
		};

	template <>
		Factory <Map<2>,
					ElementType,
					unique_ptr<Map<2>> >::Factory()
		{
			add(	TriangleType,
					&Helpers::Builders <StdTriMap,
														 Map<2>
														>::BuildObject
				);
		};

	template <>
		Factory <TensorialBasis<1>,
					BasisType,
					unique_ptr<TensorialBasis<1>> >::Factory()
		{
			add(	LegendreType,
					&Helpers::Builders	<LegendreBasis<1>,
														 TensorialBasis<1>
														>::BuildObject
				);
		};

	template <>
		Factory <TensorialBasis<2>,
					BasisType,
					unique_ptr<TensorialBasis<2>> >::Factory()
		{
			add(	LegendreType,
					&Helpers::Builders	<LegendreBasis<2>,
														 TensorialBasis<2>
														>::BuildObject
				);

			add(	WarpedType,
					&Helpers::Builders	<WarpedBasis,
														 TensorialBasis<2>
														>::BuildObject
				);
		};

	template <>
		Factory <StdFElementInterface<1, LegendreType>,
					ElementType,
					shared_ptr<StdFElementInterface<1, LegendreType>> >::Factory()
		{
			add(	IntervalType,
					&Helpers::Builders	<StdFIperCube<1, LegendreType>,
														 StdFElementInterface<1, LegendreType>
														>::BuildSingleton
				);
#ifdef MYDEBUG
			cout << "Aggiunto StdFIperCube a stdfelementfactory di indirizzo: " << this << endl;
			cout << "Chiave: " << IntervalType << endl;
#endif //MYDEBUG
		};

	template <>
		Factory <StdFElementInterface<2, LegendreType>,
					ElementType,
					shared_ptr<StdFElementInterface<2, LegendreType>> >::Factory()
		{
			add(	SquareType,
					&Helpers::Builders <StdFIperCube<2, LegendreType>,
														 StdFElementInterface<2, LegendreType>
														>::BuildSingleton
				);
		};

	template <>
		Factory <StdFElementInterface<2, WarpedType>,
					ElementType,
					shared_ptr<StdFElementInterface<2, WarpedType>> >::Factory()
		{
			add(	TriangleType,
					&Helpers::Builders <StdFElement<2, TriangleType, WarpedType>,
														 StdFElementInterface<2, WarpedType>
														>::BuildSingleton
				);
		};

} //namespace GenericFactory

