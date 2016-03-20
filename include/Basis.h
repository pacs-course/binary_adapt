#ifndef __BASIS__HH
#define __BASIS__HH

#include <memory> //unique_ptr

#include "Polinomial.h"
#include "BaseBuilderRule.h"

using namespace std;


template <size_t DIM> //working in |R ^ DIM
	class PolinomialBasis
	{
		public:

		//multidimensional case constructor
			template <size_t DUMMY = DIM, typename enable_if< (DUMMY > 1), size_t>::type = 0>
				PolinomialBasis (const vector<Polinomial<1> >& input)
				{
					//memory allocation
					_degree = input.size() - 1;
					_size = computeSize (_degree);
#ifdef DEBUG
					cout << "Grado >>>>>> " << _degree << endl;
					cout << "Dimensione base >>> " << _size << endl;
#endif //DEBUG
					_elements = move(unique_ptr<Polinomial<DIM>[]> (new Polinomial<DIM>[_size]));

					Polinomial<DIM>* position = _elements.get();
					Polinomial<DIM>** position_ptr = &(position);

					//I store elements in degree-ascendent order
					for (size_t currentDegree(0); currentDegree <= _degree; ++currentDegree)
					{
						for (size_t ind = currentDegree; ind + 1; --ind)
						{
							initialize<DIM - 1> (input, currentDegree - ind, position_ptr, input[ind]);
						}			
					}
				};

		private:
			//recursive initialization of the memory pointed by *position_ptr, the partial tensor product is stored in result
			template <size_t N, size_t DUMMY = DIM, typename enable_if< (DUMMY > 1 && N > 1), size_t>::type = 0>
				void initialize 	(	const vector<Polinomial<1> >& v,
											size_t degree,
											Polinomial<DIM>** position_ptr,
											Polinomial<DIM - N> result
										)
				{
					for (size_t ind = degree; ind + 1; --ind)
						initialize<N - 1> (v, degree - ind, position_ptr, result.tensor(v[ind]));
				};

			//specializing the last call to fillMemory in the multidimensional case: end of recursion
			template <size_t N, size_t DUMMY = DIM, typename enable_if< (DUMMY > 1 && N == 1), size_t>::type = 0>
				void initialize	(	const vector<Polinomial<1> >& v,
											size_t degree,
											Polinomial<DIM>** position_ptr,
											Polinomial<DIM - N> result
										)
				{
#ifdef DEBUG
					static int call(0);
					++call;
					cout << call << endl;
#endif //DEBUG
					**position_ptr = result.tensor(v[degree]);
					++(*position_ptr);
				};

		public:
		//1D case constructor
			template <size_t DUMMY = DIM, typename enable_if< (DUMMY == 1), size_t>::type = 0>
				PolinomialBasis (const vector<Polinomial<1> >& input)
				{
					//memory allocation
					_size = input.size();
					_degree = _size - 1;

					_elements = move(unique_ptr<Polinomial<DIM>[]> (new Polinomial<DIM>[_size]));

					for (size_t i = 0; i < _size; ++i)
						_elements[i] = input[i];
				};

			PolinomialBasis (PoliBaseBuilderRule_ptr rule, size_t degree) : PolinomialBasis ((*rule)(degree))
			{
				_rule = move(rule);
			};

			PolinomialBasis (size_t degree, PoliBaseBuilderRule_ptr rule) : PolinomialBasis (rule, degree){};

			Polinomial<DIM>& operator[] (size_t ind)
			{
				return _elements[ind];
			};

			size_t degree()
			{
				return _degree;
			};

			size_t size()
			{
				return _size;
			};

			void print()
			{
				for (size_t i(0); i < _size; ++i)
					_elements[i].print();
			};

		private:
			//size is (degree + DIM)! / degree! DIM!
			size_t computeSize (const size_t& degree) const
			{
				size_t dimFact(1), degreePlusDim(1);
				for (size_t i(2); i <= DIM; ++i)
					dimFact *= i;
				for (size_t k(1); k <= DIM; ++k)
					degreePlusDim *= (degree + k);

				return degreePlusDim / dimFact;
			};

			
		private:
//a 1D base is built calling the Functor pointed by _rule
//then a multivariate base is obtained doing the tensor product between 1D polinomials
//the degree of the basis remains the same
			PoliBaseBuilderRule_ptr _rule;

//elements are stored in degree ascendent order;
//if DIM > 1 there are multiple elements with the same degree;
//say x, y, z... the variables order, elements with the same degree are stored:
//	ascendent by variable, then descendent by order
//	e.g. if DIM=3, degree=2, I have:
// 1, x, y, z, x^2, xy, xz, y^2, yz, z^2
			unique_ptr< Polinomial<DIM>[] > _elements;
			size_t _size;
			size_t _degree;
	};


#endif //__BASIS__HH


