#ifndef __FUNCTIONS_REGISTER_H
#define __FUNCTIONS_REGISTER_H

namespace MyFunctions
{
	/*
		The constructor makes the registration of the concrete Functor builders
		into the BinaryTree::FunctionsFactory
		a global variable of FunctionsRegister type will be instantiated in the
		.cpp file to to this job at the loading of the library
	*/
	class FunctionsRegister
	{
		public:
			static FunctionsRegister& Instance();
			~FunctionsRegister(){};

		protected:
			FunctionsRegister();
	};
} //namespace MyFunctions

#endif //__FUNCTIONS_REGISTER_H
