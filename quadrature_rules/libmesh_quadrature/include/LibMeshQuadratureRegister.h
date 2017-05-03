#ifndef __LIBMESH_QUADRATURE_REGISTER_H
#define __LIBMESH_QUADRATURE_REGISTER_H

#define DEPRECATED
#ifdef DEPRECATED
namespace LibmeshBinary
{
	/*
		The constructor makes the registration of the LibmeshQuadrature builder
		into the BinaryTree::QuadratureFactory
		a global variable of QuadratureRegister type will be instantiated in the
		.cpp file to to this job at the loading of the library
	*/
	class QuadratureRegister
	{
		public:
			static QuadratureRegister& Instance();
			~QuadratureRegister(){};

		protected:
			QuadratureRegister();
	};
} //namespace LibmeshBinary

namespace Banana
{
	void LibmeshBananaFun();
}
#endif //DEPRECATED

#endif //__LIBMESH_QUADRATURE_REGISTER_H
