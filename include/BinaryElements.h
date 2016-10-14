#ifndef __BINARY_ELEMENT_H
#define __BINARY_ELEMENT_H

#include "Point.h"
#include "Element.h"

#include <memory> //std::shared_ptr
#include <functional> //std::function
#include <limits> //numeric_limits::max()

#define ENABLE_IF_VERSION
//TODO: verify if it can be written an ENABLE_IF_VERSION-undefined compiling version of the class

using namespace std;

namespace BinaryTree
{	
	template <size_t DIM, Basis::basisType FETYPE = Basis::INVALID, class LibmeshGeometry = libMesh::Edge2>
		class BinTreeElement : public FElement<DIM, FETYPE, LibmeshGeometry>
		{
			public:
				BinTreeElement(shared_ptr<function<double(Point<DIM>)>> f): _f(f),
																								_coeff(),
																								_projectionError(numeric_limits<double>::max())
				{};

				virtual ~BinTreeElement()
				{
#ifdef DESTRUCTOR_ALERT
				cout << "Distruggo BinTreeElement" << endl;
#endif //DESTRUCTOR_ALERT
				};

				virtual double computeProjectionError()
				{
					//TODO: let the error norm to be modifiable, maybe to be choosable at runtime 
/*
					L2 norm of the interpolation error
*/
					return this->integrate	(
															[&](const Point<DIM>& p){
																								double val = (*_f)(p) - projection(p);
																								return val*val;
																							}
														);
				};

			protected:
				void projectionError(const double& val)
				{
					_projectionError = val;
				};

				double projection(const Point<DIM>& point)
				{
					computeCoefficients();
					double result(0);
					vector<double> basisEvaluation = this->evaluateBasis(point);

					//TODO: optimizable: use eigen dot product
					for (size_t i(0); i < _coeff.size(); ++i)
					{
						result += _coeff[i] * basisEvaluation[i];
					}
					return result;
				};

			protected:
#ifdef ENABLE_IF_VERSION
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
						size_t s = this->basisSize();
						if (cursor == s)
							//Coefficients already computed
							return;
						_coeff.resize(s);

						//I don't recompute already stored coefficients
						for(; cursor < s; ++cursor)
						{
							_coeff[cursor] = this->L2prod	(
																		*_f,
																		[&](const Point<DIM>& p)	{
																												return this->evaluateBasisFunction(cursor, p);
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
#endif //ENABLE_IF_VERSION
					void computeCoefficients()
					{
						throw invalid_argument("I don't know how to project function on this finite element space!!!");
					};

			protected:
				//function whom projection error has to be computed
				shared_ptr<function<double(Point<DIM>)>> _f;
				//coefficients of the projection
				vector<double> _coeff;

				double _projectionError;

				//TODO: attributes needed by the algorithm
		};

#ifndef ENABLE_IF_VERSION
	template <size_t DIM, class LibmeshGeometry = libMesh::Edge2>
			void BinTreeElement<DIM, Basis::LEGENDRE, LibmeshGeometry>::computeCoefficients()
			{
				
				//I remember how many coefficients have been already computed
				size_t cursor = _coeff.size();
				//New number of coefficients
				size_t s = this->basisSize();
				if (cursor == s)
					//Coefficients already computed
					return;
				_coeff.resize(s);

				//I don't recompute already stored coefficients
				for(; cursor < s; ++cursor)
				{
					_coeff[cursor] = this->L2prod	(
																*_f,
																[&](const Point<DIM>& p)	{
																										return this->evaluateBasisFunction(cursor, p);
																									}
															);
				}
			};

	template <size_t DIM, class LibmeshGeometry = libMesh::Edge2>
			void BinTreeElement<DIM, Basis::INVALID, LibmeshGeometry>::computeCoefficients()
			{
#ifdef MYDEBUG
				cout << "Trying to project function on INVALID finite element space!!!" << endl;
#endif //MYDEBUG
				throw bad_typeid();
			};

#endif //ENABLE_IF_VERSION

	//TODO: use GetPot to select FElement basisType at runtime	
	using Interval = BinTreeElement<1, Basis::LEGENDRE, LibmeshInterval>;

	//It must be modified the way libMesh elements divides theirselves
	//The info about the procreation should be stored in the elem.embedding_matrix() method
	class Triangle : public BinTreeElement<2, Basis::LEGENDRE, LibmeshTriangle>
	{
		public:
			Triangle(shared_ptr<function<double(Point<2>)>> f) : BinTreeElement(f){};
			virtual ~Triangle(){};

			//TODO: override the divide method to make it bisective
	};


}; //namespace BinaryTree

#endif //__BINARY_ELEMENT_H
