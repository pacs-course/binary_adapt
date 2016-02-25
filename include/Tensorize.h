#ifndef __TENSOR__H
#define __TENSOR__H

#include <array>
#include <cmath> //pow
#include <Point.h>

namespace Geometry
{
	using namespace std;
	//class to manage square tensors of N dimensions equal to L
	// T has to have operator* defined
	template <typename T, size_t L, size_t N = 1>
		class Tensor
		{
			public:
			//compute tensorial product vec * vec * vec... N times
				Tensor( array<T, L> vec ) : _matrix()
				{
						vector<size_t> indexes();
						for ( size_t j = 0; j < N; ++j)
							indexes.push_back(0);
		
						size_t i(0);

						while (indexes[0] < L)
						{
							auto last_index = indexes.rbegin();
							T val(vec(*last_index));
							for (auto index = last_index + 1; index != indexes.rend(); ++index)
							{
								val *= vec(*index);
								if ( *(index - 1) % L == 0)
									(*index)++;
							}
							_matrix[i++] = val;
							(*last_index)++;
						}
				}
			
				Tensor(Vector<L> vec) : Tensor(vec.get()){};

				T& operator[] (array<size_t,N> indexes)
				{
					size_t index(0), exp(0);
					for (auto iter : indexes)
						index += iter * pow(L, exp++);
					return _matrix[index];
				}
				
			private:
				array<T, pow(L, N)> _matrix;
		};

//template <typename T, size_t L>
//	Tensor<T, L, 2> tensorize (Tensor<T, L> w1, Tensor<T, L> w2)
//	{
//		Tensor<T, L, 2> result();
//		size_t i(0);
//		for (auto iter1 : w1._matrix)
//			for (auto iter2 : w2._matrix)
//				result._matrix[i++] = iter1 * iter2;
//		return result;
//	}

}

#endif //__TENSOR__H
