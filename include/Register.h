#ifndef __REGISTER_H
#define __REGISTER_H

#include "Element.h"
#include "Basis.h"
#include "AbstractProxy.h"

/*
	A class which register all the elements in all the factory needed by the program 
*/
using namespace Basis;
using namespace std;
using namespace GenericFactory;

namespace BinaryTree
{
/*
		Based on the dimension I can have different geometries
*/
	template <size_t DIM, elementType TYPE, basisType FETYPE>
		using StdFElementProxy = Proxy	<	StdFElement<DIM, TYPE, FETYPE>,
														StdFElementFactory<DIM, FETYPE >
													>;

	template <size_t DIM, basisType FETYPE>
		using StdFIperCubeProxy = Proxy	<	StdFIperCube<DIM, FETYPE>,
														StdFElementFactory<DIM, FETYPE >
													>;

	template <basisType FETYPE>
		void template_dependent_registration()
		{
/*
					in the 1D case the reference element is directly the ipercube
*/
					////TODO: add edge3 and edge4
					StdFIperCubeProxy	<	1, FETYPE> psef_edge_two (LibmeshIntervalType);
#ifdef MYDEBUG
					cout << "Aggiunto StdFIperCube<1, "<< FETYPE << "> a StdFElementFactory<1, " << FETYPE << "> con chiave " << LibmeshIntervalType << endl;
#endif //MYDEBUG

////				//TODO: add tri6 and different geometries as quad4
					StdFElementProxy < 2, libMesh::TRI3, FETYPE > psef_tri_3 (libMesh::TRI3);

					//TODO: 3d
		};

	class Register
	{
		public:
			static Register& Instance();
			~Register();

		protected:
			Register();
	};


}//namespace BinaryTree

#endif //__REGISTER_H


//	namespace detail
//	{
//		template <size_t DIM, elementType TYPE, basisType FETYPE>
//			using StdFElementProxy = Proxy	<	StdFElement<DIM, TYPE, FETYPE>,
//															StdFElementFactory<DIM, FETYPE >
//														>;

//		template< size_t DIM, basisType FETYPE >
//			struct registrator
//			{
//			  static void do_job()
//				{
//					throw runtime_error("Invalid DIM parameter");
//				}
//			};

//		template< basisType FETYPE >
//			struct registrator< 1, FETYPE >
//			{
//				static void do_job()
//				{
///*
//					in the 1D case the reference element is directly the ipercube
//*/
//					//TODO: add edge3 and edge4
//					Proxy	<	StdFIperCube<1, FETYPE>,
//								StdFElementFactory<1, FETYPE >
//							>
//								psef_edge_two (LibmeshIntervalType);
//#ifdef MYDEBUG
//					cout << "Aggiunto StdFIperCube<1, "<< FETYPE << "> a StdFElementFactory<1, " << FETYPE << "> con chiave " << LibmeshIntervalType << endl;
//#endif //MYDEBUG
//				}
//			};

//		template< basisType FETYPE >
//			struct registrator< 2, FETYPE >
//			{
//				static void do_job()
//				{
//					//TODO: add tri6 and different geometries as quad4
//					StdFElementProxy < 2, libMesh::TRI3, FETYPE > psef_tri_3 (libMesh::TRI3);
//				}
//			};

//		template< basisType FETYPE >
//			struct registrator< 3, FETYPE >
//			{
//				static void do_job()
//				{
//					//TODO
//				}
//			};
//	}  // namespace detail

//	template <size_t DIM, basisType FETYPE>
//		void template_dependent_registration()
//		{
//			detail::registrator<DIM, FETYPE>::do_job();
//		};

