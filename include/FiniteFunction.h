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
	//TODO: to have a reference to AbstractSpace I need this one to derive from AbstractElement, since I need the L2prod method
	//		  but deriving from AbstractElement I end to write two times on _elemType when I construct derived classes
	//		  verify which solution is better
	template <size_t DIM>
		class FiniteFunction
		{
			public:
				FiniteFunction(FElement<DIM>& domain) : _domain(domain)
				{
				};

				FiniteFunction(FElement<DIM>& domain, const vector<double>& coeff) :	_domain(domain),
																											_coeff(coeff)
				{			};			

				FiniteFunction(FElement<DIM>& domain, const function<double(Point<DIM>)>& f) :
				FiniteFunction(domain)
				{
					computeCoefficients(f, _domain.FEType());
				};

				double operator() (const Point<DIM>& point)
				{
					//TODO: recompute coefficients in case domain basis has been modified
					double result(0);
					vector<double> basisEvaluation = _domain.evaluateBasis(point);

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

/*
				The computeCoefficients algorithm must depend on the Basis::basisType
*/
				//TODO: e' brutto! rendere piu' elegante e robusto
				void computeCoefficients(const function<double(Point<DIM>)> f, Basis::basisType FEtype)
				{
					switch (FEtype)
					{
						case(Basis::LEGENDRE):
						{
							size_t s = _domain.basisSize();
							_coeff.resize(s);

							for(size_t i(0); i < s; ++i)
							{
								_coeff[i] = _domain.L2prod	(
																		f,
																		[&](const Point<DIM>& p)	{
																												return _domain.evaluateBasisFunction(i, p);
																											}
																	);
							};
						}
						break;

						case(Basis::INVALID):
//#ifdef MYDEBUG
//							cout << "Trying to project function on INVALID finite element space!!!" << endl;
//#endif //MYDEBUG
							throw bad_typeid();
						break;

						default:
							throw invalid_argument("Trying to project function on unknown finite element space!!!");
					};
				};

			protected:
				vector<double> _coeff;
				//TODO: verify that the domain is not destroyed until the FiniteFunction defined on him is alive
				//		or: make the reference a shared_ptr
				FElement<DIM>& _domain;
		};


};//namespace BinaryTree
#endif //__FINITE_F_H
