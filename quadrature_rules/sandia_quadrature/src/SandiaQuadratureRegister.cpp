#include "SandiaQuadrature.h"

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
	__attribute__((destructor))
	static void ExitFunction()
	{
#ifdef MYDEBUG
		clog << "I'm closing the sandia quadrature library" << endl;
#endif //MYDEBUG
	};

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
#ifdef MYDEBUG
		clog << "Registering in Quadrature factory" << endl;
#endif //MYDEBUG

		auto& q_one_d_factory (Geometry::QuadratureFactory<1>::Instance());
//		auto& q_two_d_factory (Geometry::QuadratureFactory<2>::Instance());

#ifdef MYDEBUG
			cerr << "Indirizzo della factory 1D di quadrature : " << &q_one_d_factory << endl;
#endif //MYDEBUG

		q_one_d_factory.add (Geometry::IntervalType,
									&HelperFunctions::Builders <SandiaQuadratureRule<1>,
																		 Geometry::QuadratureRuleInterface<1>
																		>::BuildObject);
//		q_two_d_factory.add (Geometry::SquareType,
//									&HelperFunctions::Builders <SandiaQuadratureRule<2>,
//																		 Geometry::QuadratureRuleInterface<2>
//																		>::BuildObject);
	};
} //namespace LibmeshBinary

