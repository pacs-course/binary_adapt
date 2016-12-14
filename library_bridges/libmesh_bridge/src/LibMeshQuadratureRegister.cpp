#include "LibMeshQuadrature.h"
#include "LibMeshQuadratureRegister.h"

#include "Quadrature.h"
#include "BinaryTreeHelper.h"

using namespace std;

namespace LibmeshBinary
{
	QuadratureRegister::QuadratureRegister()
	{
		auto& q_one_d_factory (Geometry::QuadratureFactory<1>::Instance());
		auto& q_two_d_factory (Geometry::QuadratureFactory<2>::Instance());

		q_one_d_factory.add (Geometry::IntervalType,
									&HelperFunctions::Builders <LibmeshQuadratureRule<1, Geometry::IntervalType>,
																		 Geometry::QuadratureRuleInterface<1>
																		>::BuildObject);

		q_two_d_factory.add (Geometry::SquareType,
									&HelperFunctions::Builders <LibmeshQuadratureRule<2, Geometry::SquareType>,
																		 Geometry::QuadratureRuleInterface<2>
																		>::BuildObject);
		q_two_d_factory.add (Geometry::TriangleType,
									&HelperFunctions::Builders <LibmeshQuadratureRule<2, Geometry::TriangleType>,
															 			 Geometry::QuadratureRuleInterface<2>
																		>::BuildObject);
	};

	QuadratureRegister& QuadratureRegister::Instance()
	{
		static QuadratureRegister qr;
		return qr;
	};

} //namespace LibmeshBinary

/*
	I instantiate a global variable which does the registration
	when the library is loaded
	I use the unnamed namespace to avoid conflicts
	This does not cause any problem since this variable is not needed anywhere else 
*/
namespace
{
	auto& qr(LibmeshBinary::QuadratureRegister::Instance());
}
