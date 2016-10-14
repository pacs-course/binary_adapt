#ifndef __FINITE_F_H
#define __FINITE_F_H

#include "Element.h"

#include <memory> //unique_ptr
#include <vector> //vector
#include <stdexcept> //invalid_argument
#include <typeinfo> //bad_typeid

using namespace std;
using namespace Geometry;

namespace BinaryTree
{
	/*
		It represents a polinomial function defined on the functional space FElement
	*/
	template <size_t DIM, Basis::basisType FETYPE = Basis::INVALID>
		class FiniteFunction
		{
			public:
				FiniteFunction(shared_ptr<AbstractFElement<DIM, FETYPE>> domain, shared_ptr<function<double(Point<DIM>)>> f) :
				_domain(domain),
				_function(f)	
				{};

				FiniteFunction(AbstractFElement<DIM, FETYPE>* domain, shared_ptr<function<double(Point<DIM>)>> f) :
				_domain(domain),
				_function(f)	
				{};

				double operator() (const Point<DIM>& point)
				{
					computeCoefficients();
					double result(0);
					vector<double> basisEvaluation = _domain->evaluateBasis(point);

					//TODO: optimizable: use eigen dot product
					for (size_t i(0); i < _coeff.size(); ++i)
					{
						result += _coeff[i] * basisEvaluation[i];
					}
					return result;
				};

			protected:

				double& operator[] (size_t ind)
				{
					return _coeff[ind];
				};

				template <	Basis::basisType DUMMY = FETYPE,
								typename enable_if<	DUMMY == Basis::LEGENDRE,
															size_t
														>::type = 0
							>
					void computeCoefficients()
					{
						
						//I remember how many coefficients have been already computed
						size_t cursor = _coeff.size();
						//New number of coefficients
						size_t s = _domain->basisSize();
						if (cursor == s)
							//Coefficients already computed
							return;
						_coeff.resize(s);

						//I don't recompute already stored coefficients
						for(; cursor < s; ++cursor)
						{
							_coeff[cursor] = _domain->L2prod	(
																			*_function,
																			[&](const Point<DIM>& p)	{
																													return _domain->evaluateBasisFunction(cursor, p);
																												}
																		);
						}
					};

				template <	Basis::basisType DUMMY = FETYPE,
								typename enable_if<	DUMMY == Basis::INVALID,
															size_t
														>::type = 0
							>
					void computeCoefficients()
					{
#ifdef MYDEBUG
						cout << "Trying to project function on INVALID finite element space!!!" << endl;
#endif //MYDEBUG
						throw bad_typeid();
					};

				//TODO: verify if there's a way to define a default case
				template <	Basis::basisType DUMMY = FETYPE,
								typename enable_if<	DUMMY != Basis::LEGENDRE && DUMMY != Basis::INVALID,
															size_t
														>::type = 0
							>
					void computeCoefficients()
					{
						throw invalid_argument("I don't know how to project function on this finite element space!!!");
					};

			protected:
/*
				The domain cannot be modified! It's needed in the construction of FiniteFunction
				//TODO: is there a way to make this compulsory?
*/
				shared_ptr<AbstractFElement<DIM, FETYPE>> _domain;
				shared_ptr<function<double(Point<DIM>)> > _function;
				vector<double> _coeff;
		};


};//namespace BinaryTree
#endif //__FINITE_F_H
