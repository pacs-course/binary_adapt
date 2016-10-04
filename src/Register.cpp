#include "Register.h"

using namespace BinaryTree;
using namespace std;

	template <>
		void template_dependent_registration<1>()
		{
			//TODO: add edge3 and edge4
			Proxy < StdElement<1, libMesh::EDGE2>,	StdElementFactory<1>	> psef_edge_two (libMesh::EDGE2);

			Proxy < IntervalMap						,	AffineMapFactory<1>	> pamf_interval (libMesh::EDGE2);
		};

	template <>
		void template_dependent_registration<2>()
		{
			//TODO: add tri6 and different geometries as quad4
			Proxy < StdElement<2, libMesh::EDGE2>,	StdElementFactory<2>	> psef_tri_3 	 (libMesh::TRI3);

			Proxy < TriMap,								AffineMapFactory<2>	> pamf_triangle (libMesh::TRI3);
		};

	template <>
		void template_dependent_registration<3>()
		{
			//TODO:
		};

	Register::Register()
	{
/*
		Registering the rules for the basis construction 
*/
		Proxy <LegendreBuilder,						BasisBuilderFactory	> pbbf_legendre (Basis::LEGENDRE);
		Proxy <TrivialBuilder,						BasisBuilderFactory	> pbbf_triangle (Basis::TRIVIAL);
/*
		Registering in template dependent factories; the registration is demanded to a function
		which guarantees that unuseful registrations are avoided 
*/
		Proxy < StdSpace<1, Basis::LEGENDRE>, StdSpaceFactory<1> > pssf_legendre1 (Basis::LEGENDRE);
		Proxy < StdSpace<2, Basis::LEGENDRE>, StdSpaceFactory<2> > pssf_legendre2 (Basis::LEGENDRE);
		Proxy < StdSpace<3, Basis::LEGENDRE>, StdSpaceFactory<3> > pssf_legendre3 (Basis::LEGENDRE);
/*
		Based on the dimension I can have different geometries
*/
		template_dependent_registration<1>();
		template_dependent_registration<2>();
	};

	Register& Register::Instance()
	{
		static Register r;
		return r;
	};

	Register::~Register()
	{
#ifdef MYDEBUG
		cout << "Distruggo Register" << endl;
#endif //MYDEBUG
	};
