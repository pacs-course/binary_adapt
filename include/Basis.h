#ifndef __BASIS__HH
#define __BASIS__HH

#include <memory> //unique_ptr
#include <stdexcept> //range_error

#include "Polinomial.h"
#include "BasisBuilderRule.h"
#include "Maps.h"

namespace Basis
{
	using namespace std;
	using namespace Maps;

	enum basisType
	{
		TRIVIAL = 0,
		LEGENDRE = 1,
		INVALID = 99
	};

	//TODO: maybe checks on the object to be effectively a basis (linearly independent and maximal)
	//have to be inserted in the construction or after it

	using BasisBuilderFactory = GenericFactory::ObjectFactory<PoliBaseBuilderRule, basisType>;

/*
	The basis is a Singleton. Here I store the basis functions, which can be accessed through operator[] or through iterator
	basis functions are computed only one time, while the degree, stored in the _degree attribute, could change at runtime.
*/
	template <size_t DIM, basisType FETYPE = INVALID> //working in |R ^ DIM
		class PolinomialBasis
		{
			public:
#ifdef SINGLETON_ENABLED

				PolinomialBasis& operator =	(const PolinomialBasis&) = delete;
				PolinomialBasis					(const PolinomialBasis&) = delete;

				static shared_ptr<PolinomialBasis> Instance()
				{
					static shared_ptr<PolinomialBasis> pb = shared_ptr<PolinomialBasis>(new PolinomialBasis);
					return pb;
				};

			protected:
/*
				It is a singleton
*/
#else //SINGLETON_ENABLED

				PolinomialBasis& operator =	(const PolinomialBasis& inputBasis)
				{
					if (this != &inputBasis)
					{
						_degree = inputBasis._degree;
						_size = inputBasis._size;
						_sizeUpdated = inputBasis._sizeUpdated;
						if (!_sizeUpdated)
							throw length_error("Trying to copy a basis of unknown size!");

						_elements = move(unique_ptr<Polinomial<DIM>[]> (new Polinomial<DIM>[_size]));

						size_t i = 0;
						for (const auto& pol : inputBasis)
							_elements[i++] = pol;

					}
					return *this;
				};
				PolinomialBasis					(const PolinomialBasis& inputBasis)
				{
					*this = inputBasis;
				};

#endif //SINGLETON_ENABLED

				//default constructor
				PolinomialBasis (size_t degree = 0) : _degree(degree)
				{
					//TODO: optimize it, maybe create a degree->size map
					_size = computeSize (_degree);
					_sizeUpdated = true;

					auto& factory(BasisBuilderFactory::Instance());
					_rule = factory.create(FETYPE);
					vector<Polinomial<1> > oneDPol = (*_rule)(degree);
					constructBasis(oneDPol);
				};

			public:

				using BasisFunction = Polinomial<DIM>;

				virtual size_t degree()const
				{
					return _degree;
				};

#ifndef SINGLETON_ENABLED

				//TODO: evaluate the possibility of having a unique basis, adding new elements if the degree is increased, reducing the _size attribute otherwise, without distroying already computed elements, but keeping memory of what has been computed (when _size attribute is modified, all informations on the effectively length of the smart array _elements are definitely lost)
				virtual void degree(size_t new_degree)
				{
					//TODO: creepy! Recomputation of already stored functions must be avoided!
					//solution: create directly the maximal basis needed by the alghorithm (it's given by the degree of freedom) when the program begins
					//WORSE alternative: use vectors instead of arrays
					(*this) = PolinomialBasis(new_degree);
				};

#else //SINGLETON_ENABLED
				virtual void degree(size_t new_degree)
				{
					//TODO: as above
					_size = computeSize(new_degree);
					_degree = new_degree;
					constructBasis((*_rule)(_degree));
				}
#endif //SINGLETON_ENABLED

				virtual size_t size() const
				{
					if (!_sizeUpdated)
					{
						throw length_error("_elements length and _size attribute does not fit!");
					}
					return _size;
				};

				virtual void print() const
				{
					for (const auto& pol : *this)
						pol.print();
				};

				virtual Polinomial<DIM>& operator[] (size_t ind)
				{
					if (ind > _size)
					{
						throw range_error("Trying to access out of range basis function!");
					}
					return _elements[ind];
				};

				//TODO: verify if these pseudo iterators work fine
				virtual Polinomial<DIM>* begin()
				{
					return _elements.get();
				};
				virtual Polinomial<DIM>* end()
				{
					return begin() + size();
				};

				//want to mime const_iterators
				virtual Polinomial<DIM>* begin() const
				{
					return _elements.get();
				};
				virtual Polinomial<DIM>* end() const
				{
					return begin() + size();
				};
			private:
			//1D construction from 1D polinomial
				template <size_t DUMMY = DIM, typename enable_if< (DUMMY == 1), size_t>::type = 0>
					void constructBasis (const vector<Polinomial<1> >& input)
					{
						if (_size != (_degree + 1))
							throw length_error("In constructBasis basis degree and polinomial input size do not fit!");
						
						//memory allocation
						_elements = move(unique_ptr<Polinomial<DIM>[]> (new Polinomial<DIM>[_size]));

						for (size_t i = 0; i < _size; ++i)
							_elements[i] = input[i];
					};

			//multidimensional construction from 1D polinomial
				template <size_t DUMMY = DIM, typename enable_if< (DUMMY > 1), size_t>::type = 0>
					void constructBasis (const vector<Polinomial<1> >& input)
					{
						//memory allocation
//	#ifdef MYDEBUG
//						cout << "Grado >>>>>> " << _degree << endl;
//						cout << "Dimensione base >>> " << _size << endl;
//	#endif //MYDEBUG
						_elements = move(unique_ptr<Polinomial<DIM>[]> (new Polinomial<DIM>[_size]));

						Polinomial<DIM>*	position	= _elements.get();
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
//	#ifdef MYDEBUG
//						static int call(0);
//						++call;
//						cout << call << endl;
//	#endif //MYDEBUG
						**position_ptr = result.tensor(v[degree]);
						++(*position_ptr);
					};

			protected:
				//size is (degree + DIM)! / degree! DIM!
				virtual size_t computeSize (const size_t& degree) const
				{
					if (DIM == 1)
						return degree + 1;

					size_t dimFact(1), degreePlusDim(1);
					for (size_t i(2); i <= DIM; ++i)
						dimFact *= i;
					for (size_t k(1); k <= DIM; ++k)
						degreePlusDim *= (degree + k);

					return degreePlusDim / dimFact;
				};
			
			protected:
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

				size_t _degree;
				size_t _size;
				bool	 _sizeUpdated;
		};

};//namespace Basis

#endif //__BASIS__HH


