#include "SandiaQuadrature.h"

#define DEPRECATED

#ifdef DEPRECATED
#include "SandiaQuadratureRegister.h"
#endif //DEPRECATED

#include "BinaryTreeHelper.h"

using namespace std;

#ifdef DEPRECATED
namespace Banana
{
	void SandiaBananaFun()
#else //DEPRECATED
namespace SandiaQuadrature
{
	__attribute__((constructor))
	static void RegisterFunction()
#endif //DEPRECATED
	{
#ifdef DEPRECATED
/*
		I instantiate a global variable which does the registration
		when the library is loaded
		I use the unnamed namespace to avoid conflicts
		This does not cause any problem since this variable is not needed anywhere else 
*/
		auto& qr(SandiaQuadrature::QuadratureRegister::Instance());
		(void) qr;
	};
}

namespace SandiaQuadrature
{

	QuadratureRegister& QuadratureRegister::Instance()
	{
		static QuadratureRegister qr;
		return qr;
	};

	QuadratureRegister::QuadratureRegister()
	{
#endif //DEPRECATED
		auto& q_one_d_factory (Geometry::QuadratureFactory<1>::Instance());
		auto& q_two_d_factory (Geometry::QuadratureFactory<2>::Instance());

		q_one_d_factory.add (Geometry::IntervalType,
									&HelperFunctions::Builders <SandiaQuadratureRule<1>,
																		 Geometry::QuadratureRuleInterface<1>
																		>::BuildObject);
		q_two_d_factory.add (Geometry::SquareType,
									&HelperFunctions::Builders <SandiaQuadratureRule<2>,
																		 Geometry::QuadratureRuleInterface<2>
																		>::BuildObject);
	};
} //namespace LibmeshBinary

