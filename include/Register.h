#ifndef __REGISTER_H
#define __REGISTER_H

#include "Element.h"
#include "Basis.h"
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
namespace detail
{
	template <typename AbstractProduct, typename ConcreteProduct>
		struct Helper
		{
			static shared_ptr<AbstractProduct> Build_singleton()
			{
#ifdef MYDEBUG
				ConcreteProduct s;
				cout << "Voglio creare uno shared_ptr<> con make_shared< " << typeid(s).name() << ">" << endl;
#endif //MYDEBUG
#ifndef SINGLETON_ENABLED
				static shared_ptr<AbstractProduct> ptr = make_shared<ConcreteProduct>();			
#else 
				shared_ptr<AbstractProduct> ptr(ConcreteProduct::Instance());
#endif //SINGLETON_ENABLED
				return ptr;
			};

			static unique_ptr<AbstractProduct> Build_object()
			{
				return make_unique<ConcreteProduct>();
			};
		};
} //namespace detail

	template <basisType FETYPE>
		void template_dependent_registration()
		{
					//get the factories. First time creates them.
					auto& feOneD_factory(StdFElementFactory<1, FETYPE >::Instance());
					auto& feTwoD_factory(StdFElementFactory<2, FETYPE >::Instance());

/*
					in the 1D case the reference element is directly the ipercube
*/
					////TODO: add edge3 and edge4
					feOneD_factory.add(	LibmeshIntervalType,
												&detail::Helper<	StdBananaFElement<1, FETYPE>,
																		StdFIperCube<1, FETYPE>
																	>::Build_singleton
											);
#ifdef MYDEBUG
					cout << "Aggiunto StdFIperCube<1, "<< FETYPE << "> a StdFElementFactory<1, " << FETYPE << "> con chiave " << LibmeshIntervalType << endl;
#endif //MYDEBUG

//////				//TODO: add tri6 and different geometries as quad4
					feTwoD_factory.add(	libMesh::TRI3,
												&detail::Helper<	StdBananaFElement<2, FETYPE>,
																		StdFElement<2, libMesh::TRI3, FETYPE>
																	>::Build_singleton
											);

//					//TODO: 3d
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
