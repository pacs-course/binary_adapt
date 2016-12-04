#include "Register.h"
#include "LibMeshQuadrature.h"

using namespace BinaryTree;
using namespace FiniteElements;
using namespace std;

	Register::Register()
	{
/*
		Registering the rules for the basis construction 
*/
		auto& bb_factory(BasisBuilderFactory::Instance());

		bb_factory.add(FiniteElements::LegendreType, &HelperFunctions::Builders <LegendreBuilder, PoliBaseBuilderRule> ::BuildObject);
		bb_factory.add(FiniteElements::TrivialType, &HelperFunctions::Builders <TrivialBuilder, PoliBaseBuilderRule> ::BuildObject);
		bb_factory.add(FiniteElements::InvalidFeType, &HelperFunctions::Builders <InvalidBuilder, PoliBaseBuilderRule> ::BuildObject);
/*
		Registering std elements in template dependent factories; the registration is demanded to a function
		which guarantees that unuseful registrations are avoided
*/
		TemplateDependentRegistration <FiniteElements::TrivialType> ();
		TemplateDependentRegistration <FiniteElements::LegendreType> ();

/*
		Registering maps
*/
		auto& am_one_d_factory (AffineMapFactory<1>::Instance());
		auto& am_two_d_factory (AffineMapFactory<2>::Instance());
/*		for the 1D case I don't need any map between std elements, since the stdFElem is already the ipercube */
		auto& stdm_two_d_factory(StdMapFactory<2>::Instance());

		am_one_d_factory.add(IntervalType, &HelperFunctions::Builders <IntervalMap, AffineMap<1>> ::BuildObject);
		am_two_d_factory.add(TriangleType, &HelperFunctions::Builders <TriMap, AffineMap<2>> ::BuildObject);
		stdm_two_d_factory.add(TriangleType, &HelperFunctions::Builders <StdTriMap, Map<2>> ::BuildObject);

//TODO: to be transferred into a dynamic object
/*
		Registering quadrature
*/
		auto& q_one_d_factory (QuadratureFactory<1>::Instance());
		auto& q_two_d_factory (QuadratureFactory<2>::Instance());

		q_one_d_factory.add (IntervalType,
									&HelperFunctions::Builders <LibmeshBinary::LibmeshQuadratureRule<1, IntervalType>,
																		 QuadratureRuleInterface<1>
																		>::BuildObject);

		q_two_d_factory.add (SquareType,
									&HelperFunctions::Builders <LibmeshBinary::LibmeshQuadratureRule<2, SquareType>,
																		 QuadratureRuleInterface<2>
																		>::BuildObject);
		q_two_d_factory.add (TriangleType,
									&HelperFunctions::Builders <LibmeshBinary::LibmeshQuadratureRule<2, TriangleType>,
																		 QuadratureRuleInterface<2>
																		>::BuildObject);

	};

	Register& Register::Instance()
	{
		static Register r;
		return r;
	};

	Register::~Register()
	{
#ifdef DESTRUCTOR_ALERT
		cout << "Distruggo Register" << endl;
#endif //DESTRUCTOR_ALERT
	};
