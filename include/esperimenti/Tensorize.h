#ifndef __TENSOR__H
#define __TENSOR__H

#include <array>
#include <cmath> //pow
#include <stdexcept> //invalid_argument
#include <string> //string
#include <memory> //unique_ptr
#include <array> //array
#include <vector> //vector
#include <iterator> //array::iterator

namespace Geometry
{
	using namespace std;


	template <typename T, size_t DIM>
		class Tensor
		{
			public:

				Tensor() : _tData(nullptr){};

				//constructor from array of size, memory allocated and default initialized
				template <typename T, size_t DIM>
					Tensor (array<size_t, DIM> lengths) :	_lengths(lengths), _arrayLength(1)
					{
						for (auto dim : _lengths)
							_arrayLength *= dim;

						_tData = move (unique_ptr<T[]> (new T[_arrayLength]) );
					};


				//copy constructor
				Tensor& operator = (Tensor&& t)
				{
					if (&t != this)
					{
						_tData = move (t._tData);
						_arrayLength = t._arrayLength;
						_lengths = t._lengths;
						t._arrayLength = 0;
						t._lengths.fill(0);
					}
					return *this;
				};


				//call operator, returns tensor element at the input indexes position
				//say (i,j,k) indexes of an element in a 3D-Tensor T of _lengths (N,M,L),
				//then T(i,j,k) = _tData[ (N*M)*k + N*j + i ]
				T& operator() (const array<size_t, DIM>& indexes)
				{
					size_t ind = indexes[0];
					size_t step = 1;

					for (size_t j = 1; j < DIM; ++j)
					{
						step *= _arrayLengths[j - 1];
						ind += step * indexes[j];
					}					
					return _tData[ind];
				};

				array<size_t, DIM> measures()	{return _lengths;};

				size_t size() 						{return _arrayLength;};

			private:
				//matrics stored by columns, parallelepiped by successive matrices and so on
				unique_ptr<T[]> _tData;

				//array of tensor dimensions
				array<size_t, DIM> _lengths;

				//length of _tdata, is the product structure of _lengths elements
				size_t _arrayLength;
		};



}
#endif //__TENSOR__H

