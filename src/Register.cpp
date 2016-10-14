#include "Register.h"

using namespace BinaryTree;
using namespace std;

	Register::Register()
	{
/*
		Registering the rules for the basis construction 
*/
//		Proxy <LegendreBuilder,	BasisBuilderFactory> pbbf_legendre (Basis::LEGENDRE);
//		Proxy <TrivialBuilder,	BasisBuilderFactory> pbbf_triangle (Basis::TRIVIAL);
/*
		Registering std elements in template dependent factories; the registration is demanded to a function
		which guarantees that unuseful registrations are avoided
*/
		template_dependent_registration<Basis::TRIVIAL>();
//		template_dependent_registration<Basis::LEGENDRE>(); //ECCOLO!!!

/*
		Registering maps
*/
//		Proxy < IntervalMap, AffineMapFactory<1>	> pamf_interval (libMesh::EDGE2);
//		Proxy < TriMap, AffineMapFactory<2>	> pamf_triangle (libMesh::TRI3);
//		////for the 1D case I don't need any map between std elements, since the stdFElem is already the ipercube
//		Proxy < StdTriMap, StdMapFactory<2>> psmf_triangle (libMesh::TRI3);
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
