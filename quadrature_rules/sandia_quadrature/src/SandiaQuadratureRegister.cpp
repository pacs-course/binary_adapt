#include "SandiaQuadrature.h"

#include "BinaryTreeHelper.h"

using namespace std;

namespace SandiaQuadrature
{
	__attribute__ ((destructor))
	static void ExitFunction()
	{};

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
							 &Helpers::Builders <SandiaQuadratureRule<1>,
												 Geometry::QuadratureRuleInterface<1>
												>::BuildObject);
		q_two_d_factory.add (Geometry::SquareType,
							 &Helpers::Builders <SandiaQuadratureRule<2>,
												 Geometry::QuadratureRuleInterface<2>
												>::BuildObject);
	};
} //namespace LibmeshBinary

