#include "SandiaQuadrature.h"

#include "BinaryTreeHelper.h"

using namespace std;

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
	{
#ifdef MYDEBUG
		clog << "Registering in Quadrature factory" << endl;
#endif //MYDEBUG

		auto& q_one_d_factory (Geometry::QuadratureFactory<1>::Instance());
		auto& q_two_d_factory (Geometry::QuadratureFactory<2>::Instance());

#ifdef MYDEBUG
			cerr << "Indirizzo della factory 1D di quadrature : " << &q_one_d_factory << endl;
#endif //MYDEBUG

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

