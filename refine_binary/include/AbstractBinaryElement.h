#ifndef __ABSTRACT_BINARY_ELEMENT_H
#define __ABSTRACT_BINARY_ELEMENT_H

#include "BinaryNode.h"
#include "AbstractFElement.h"

#include <limits> //numeric_limits::max()

using namespace FiniteElements;

namespace BinaryTree
{
	template <size_t dim, BasisType FeType = InvalidFeType>
		class AbstractBinaryElement : public BinaryNode 
		{
			public:
				AbstractBinaryElement(	FunctionPtr<dim> f,
												shared_ptr<AbstractFElement<dim, FeType> > el_ptr
											):
												BinaryNode (),
												_f (f),
												_coeff (),
												_f_element (el_ptr),
												_projection_error (numeric_limits<double>::max()),
												_error_updated (false),
												_tilde_error (numeric_limits<double>::max())
				{};

//				AbstractBinaryElement() : AbstractBinaryElement(nullptr, nullptr){};
				virtual void Init()
				{
					_f_element->Init();

					UpdateProjectionError();
					
					auto daddy = Dad();
					//TODO : check the correctness
					daddy == nullptr ?	this->_tilde_error = this->_projection_error :
												//TODO : optimize it
												this->_tilde_error = this->_projection_error * daddy->ProjectionError()
																			/
																			(this->_projection_error + daddy->ProjectionError());
					this->_E			= this->_projection_error;
					this->_E_tilde	= this->_tilde_error;
					this->_q			= this->_tilde_error;
				};

				double ProjectionError()
				{
					if (! _error_updated)
						UpdateProjectionError();

					return this->_projection_error;
				};
				
				virtual void PrintCoefficients()
				{
					ComputeCoefficients();
					for (auto& c : this->_coeff)
						cout << c << "	";
					cout << endl;
				};

				virtual void PLevel(size_t val)
				{
					_f_element->PLevel(val);
					this->_error_updated = false;
				};
				virtual size_t PLevel() const
				{
					return _f_element->PLevel();
				};

				AbstractFElement<dim, FeType>& GetFElement()const
				{
					return *_f_element;
				};

			protected:
				void ProjectionError(const double& val)
				{
					this->_projection_error = val;
				};

				virtual void UpdateProjectionError() override
				{
					//TODO: let the error norm to be modifiable, maybe to be choosable at runtime 
/*
					L2 norm of the interpolation error
*/
					double err =  sqrt(	_f_element->Integrate	(
																					[&](const Point<dim>& p){
																														double val = (*(this->_f))(p) - Projection(p);
																														return val*val;
																													}
																				)
											);

					this->ProjectionError(err);
					this->_error_updated = true;
				};

				virtual double Projection(const Point<dim>& point)
				{
					ComputeCoefficients();
					double result(0);
//					vector<double> basisEvaluation = this->evaluateBasis(point);

					//TODO: optimizable: use eigen dot product
					for (size_t i(0); i < this->_coeff.size(); ++i)
					{
						result += this->_coeff[i] * _f_element->EvaluateBasisFunction(i, point);
					}
					return result;
				};

//TODO: verify if it can be written an ENABLE_IF_VERSION-undefined compiling version of the class
				template <	BasisType Dummy = FeType,
								typename enable_if<	Dummy == LegendreType,
															size_t
														>::type = 0
							>
					void ComputeCoefficients()
					{
						
						//I remember how many coefficients have been already computed
						size_t cursor = this->_coeff.size();
						//New number of coefficients
						size_t s = _f_element->BasisSize();
						if (cursor == s)
							//Coefficients already computed
							return;
						this->_coeff.resize(s);

						//I don't recompute already stored coefficients
						for(; cursor < s; ++cursor)
						{
							(this->_coeff)[cursor] = _f_element->L2prod( *(this->_f),
																						[&](const Point<dim>& p)
																						{ return _f_element->EvaluateBasisFunction(cursor, p);}
																					 );
						}
					};

				template <	BasisType Dummy = FeType,
								typename enable_if<	Dummy == InvalidFeType,
															size_t
														>::type = 0
							>
					void ComputeCoefficients()
					{
#ifdef MYDEBUG
						cout << "Trying to project function on InvalidFeType finite element space!!!" << endl;
#endif //MYDEBUG
						throw bad_typeid();
					};

				//TODO: verify if there's a way to define a default case
				template <	BasisType Dummy = FeType,
								typename enable_if<	Dummy != LegendreType && Dummy != InvalidFeType,
															size_t
														>::type = 0
							>
					void computeCoefficients()
					{
						throw invalid_argument("I don't know how to project function on this finite element space!!!");
					};

			protected:
				//function whom projection error has to be computed
				FunctionPtr<dim> _f;
				//coefficients of the projection
				vector<double> _coeff;

				//TODO: verify if unique_ptr is better
				shared_ptr<AbstractFElement<dim, FeType> > _f_element;

				double _projection_error;
				bool	 _error_updated;
				double _tilde_error;
		};
} //namespace BinaryTree

#endif //__ABSTRACT_BINARY_ELEMENT_H
