#ifndef __REGISTER_H
#define __REGISTER_H

#include "AbstractFElement.h"
#include "Basis.h"
#include "BinaryTreeHelper.h"

/*
	A class which register all the elements in all the factory needed by the program 
*/
using namespace std;
using namespace GenericFactory;
using namespace FiniteElements;

namespace BinaryTree
{
	template <BasisType FeType>
		void TemplateDependentRegistration()
		{
			//get the factories. First time creates them.
			auto& fe_one_d_factory(StdFElementFactory<1, FeType >::Instance());
			auto& fe_two_d_factory(StdFElementFactory<2, FeType >::Instance());

/*
			in the 1D case the reference element is directly the ipercube
*/
			fe_one_d_factory.add	(	IntervalType,
											&HelperFunctions::Builders	<StdFIperCube<1, FeType>,
																				 StdBananaFElement<1, FeType>
																				>::BuildSingleton
										);
#ifdef MYDEBUG
			cout << "Aggiunto StdFIperCube<1, "<< FeType << "> a StdFElementFactory<1, " << FeType << "> con chiave " << IntervalType << endl;
#endif //MYDEBUG

			fe_two_d_factory.add	(	TriangleType,
											&HelperFunctions::Builders <StdFElement<2, TriangleType, FeType>,
																				 StdBananaFElement<2, FeType>
																				>::BuildSingleton
										);

//				//TODO: 3d
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
