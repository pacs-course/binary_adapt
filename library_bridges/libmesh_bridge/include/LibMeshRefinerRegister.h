#ifndef __LIBMESH_REFINE_REGISTER_H
#define __LIBMESH_REFINE_REGISTER_H

namespace LibmeshBinary
{
	/*
		The constructor makes the registration of the LibmeshRefiner builder
		into the BinaryTree::MeshRefinerFactory
		a global variable of RefinerRegister type will be instantiated in the
		.cpp file to to this job at the loading of the library
	*/
	class RefinerRegister
	{
		public:
			static RefinerRegister& Instance();
			~RefinerRegister(){};

		protected:
			RefinerRegister();
	};
} //namespace LibmeshBinary

#endif //__LIBMESH_REFINE_REGISTER_H
