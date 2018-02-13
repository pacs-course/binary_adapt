#include "LibMeshQuadrature.h"

#include "BinaryTreeHelper.h"

using namespace std;

namespace LibmeshBinary
{
	__attribute__ ((destructor))
	static void ExitFunction()
	{}

	/**
		Function doing quadrature rules builders registrations at plugin loading.
		Automatically called when the plugin is loaded,
		it makes the registrations of quadrature rules builders in
		quadrature rules factories.
	**/
	__attribute__ ((constructor))
	static void RegisterFunction()
	{
		auto& q_one_d_factory (Geometry::QuadratureFactory<1>::Instance());
		auto& q_two_d_factory (Geometry::QuadratureFactory<2>::Instance());

		q_one_d_factory.add (Geometry::IntervalType,
							 &Helpers::Builders <LibmeshQuadratureRule<1, Geometry::IntervalType>,
												 Geometry::QuadratureRuleInterface<1>
												>::BuildObject);

		q_two_d_factory.add (Geometry::SquareType,
							 &Helpers::Builders <LibmeshQuadratureRule<2, Geometry::SquareType>,
												 Geometry::QuadratureRuleInterface<2>
												>::BuildObject);

		q_two_d_factory.add (Geometry::TriangleType,
							 &Helpers::Builders <ModifiedTriangleRule,
												 Geometry::QuadratureRuleInterface<2>
												>::BuildObject);

	}
} //namespace LibmeshBinary

