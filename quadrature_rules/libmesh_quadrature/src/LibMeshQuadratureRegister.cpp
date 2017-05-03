#include "LibMeshQuadrature.h"

#define DEPRECATED
#ifdef DEPRECATED
#include "LibMeshQuadratureRegister.h"
#endif //DEPRECATED

#include "BinaryTreeHelper.h"

using namespace std;

#ifdef DEPRECATED
namespace Banana
{
	void LibmeshBananaFun()
#else //DEPRECATED
namespace LibmeshBinary
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
		auto& qr(LibmeshBinary::QuadratureRegister::Instance());
		(void) qr;
	};
}

namespace LibmeshBinary
{

	QuadratureRegister& QuadratureRegister::Instance()
	{
		static QuadratureRegister qr;
		return qr;
	};

	QuadratureRegister::QuadratureRegister()
	{
#endif //DEPRECATED
		cerr << "BANANA" << endl;
		auto& q_one_d_factory (Geometry::QuadratureFactory<1>::Instance());
		auto& q_two_d_factory (Geometry::QuadratureFactory<2>::Instance());

		q_one_d_factory.add (Geometry::IntervalType,
									&HelperFunctions::Builders <LibmeshQuadratureRule<1, Geometry::IntervalType>,
																		 Geometry::QuadratureRuleInterface<1>
																		>::BuildObject);
#ifdef MYDEBUG
		cout << "Registrata in QuadratureFactory<1> LibmeshQuadratureRule<1, " << Geometry::IntervalType << "> con chiave: " << Geometry::IntervalType << endl;
		cout << "Indirizzo factory: " << &q_one_d_factory << endl;
#endif //MYDEBUG
		q_two_d_factory.add (Geometry::SquareType,
									&HelperFunctions::Builders <LibmeshQuadratureRule<2, Geometry::SquareType>,
																		 Geometry::QuadratureRuleInterface<2>
																		>::BuildObject);
		q_two_d_factory.add (Geometry::TriangleType,
									&HelperFunctions::Builders <LibmeshQuadratureRule<2, Geometry::TriangleType>,
															 			 Geometry::QuadratureRuleInterface<2>
																		>::BuildObject);
	};
} //namespace LibmeshBinary

