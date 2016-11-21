#include "Register.h"

using namespace BinaryTree;
using namespace std;

	Register::Register()
	{
/*
		Registering the rules for the basis construction 
*/
		auto& bb_factory(BasisBuilderFactory::Instance());

		bb_factory.add(Basis::LEGENDRE,	&detail::Helper< PoliBaseBuilderRule, LegendreBuilder	>::Build_object);
		bb_factory.add(Basis::TRIVIAL,	&detail::Helper< PoliBaseBuilderRule, TrivialBuilder	>::Build_object);
		bb_factory.add(Basis::INVALID,	&detail::Helper< PoliBaseBuilderRule, InvalidBuilder	>::Build_object);
/*
		Registering std elements in template dependent factories; the registration is demanded to a function
		which guarantees that unuseful registrations are avoided
*/
		template_dependent_registration<Basis::TRIVIAL>();
		template_dependent_registration<Basis::LEGENDRE>();

/*
		Registering maps
*/
		auto& amOneD_factory		(	AffineMapFactory<1>::Instance());
		auto& amTwoD_factory		(	AffineMapFactory<2>::Instance());
/*		for the 1D case I don't need any map between std elements, since the stdFElem is already the ipercube */
		auto& stdmTwoD_factory	(		StdMapFactory<2>::Instance());

		amOneD_factory.	add(libMesh::EDGE2,	&detail::Helper<AffineMap<1>,	IntervalMap	>::Build_object);
		amTwoD_factory.	add(libMesh::TRI3,	&detail::Helper<AffineMap<2>,	TriMap		>::Build_object);
		stdmTwoD_factory.	add(libMesh::TRI3,	&detail::Helper<Map<2>,			StdTriMap	>::Build_object);
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
