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
	template <size_t DUMMY>
		void template_dependent_registration()
		{
			throw runtime_error("Invalid DIM parameter");
		};

	template <>
		void template_dependent_registration<1>();

	template <>
		void template_dependent_registration<2>();

	template <>
		void template_dependent_registration<3>();


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
