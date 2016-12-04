#ifndef __CUBIC_T_H
#define __CUBIC_T_H

#include <functional> //function
#include <type_traits> //enable_if

#include "Tensorize.h"

using namespace std;

namespace Geometry
{
	template <typename T, size_t DIM>
		function< T (array<T, DIM>) > standardProd = [](array<T, DIM> input){
																									size_t tot(1);
																									for (auto val : input)
																										tot *= val;
																									return tot;
																									};

	template <typename T, size_t DIM>
		class CubicTensor : public Tensor<T, DIM>
		{
			public:
				//constructors for cubic tensor
				CubicTensor(size_t length);
				{
					Tensor<T, DIM>::_arrayLength = (pow(length, DIM));
					Tensor<T, DIM>::_tData = move(new T[Tensor<T, DIM>::_arrayLength]);
					Tensor<T, DIM>::_lengths.fill(length);
				};

		};

}

#endif //__CUBIC_T_H
