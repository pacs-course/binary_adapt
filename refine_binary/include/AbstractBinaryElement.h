#ifndef __ABSTRACT_BINARY_ELEMENT_H
#define __ABSTRACT_BINARY_ELEMENT_H

#include "BinaryNode.h"
#include "CoefficientComputer.h"

#include <limits> //numeric_limits::max()

namespace BinaryTree
{
	using namespace FiniteElements;

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
												_error_updated (false)
				{};

				virtual void Init()
				{
#ifdef MYDEBUG
					cerr << "Inizializzo AbstractBinaryElement" << endl;
#endif //MYDEBUG
					_f_element->Init();

					UpdateProjectionError();
					
					auto daddy = Dad();
					daddy == nullptr ?	this->_tilde_error = this->_projection_error :
												this->_tilde_error = this->_projection_error * daddy->TildeError()
																			/
																			(this->_projection_error + daddy->TildeError());
					this->_E			= this->_projection_error;
					this->_E_tilde	= this->_tilde_error;
					this->_q			= this->_tilde_error;
				};

				virtual double ProjectionError()
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
					this->_f_element->PLevel(val);
					this->_error_updated = false;
				};
				virtual size_t PLevel() const
				{
					return this->_f_element->PLevel();
				};

				AbstractFElement<dim, FeType>& GetFElement()const
				{
					return *_f_element;
				};

			protected:
				virtual void ProjectionError(const double& val)
				{
					this->_projection_error = val;
				};

				virtual void UpdateProjectionError() override
				{
					//TODO: let the error norm to be modifiable, maybe to be choosable at runtime 
/*
					L2 norm of the interpolation error
*/
					double err = sqrt	(_f_element->Integrate
												(
													[&](const Geometry::Point<dim>& p)	{
																										double val = (*(this->_f))(p) - Projection(p);
																										return val*val;
																									}
												)
											);

					this->ProjectionError(err);
					this->_error_updated = true;
				};

				virtual double Projection(const Geometry::Point<dim>& point)
				{
					ComputeCoefficients();
					double result(0);
					vector<double> basis_evaluation = this->_f_element->EvaluateBasis(point);

					//TODO: optimizable: use eigen dot product
					for (size_t i(0); i < this->_coeff.size(); ++i)
					{
						result += this->_coeff[i] * basis_evaluation[i];
					}
					return result;
				};

				void ComputeCoefficients()
				{
					FiniteElements::CoefficientComputer<dim, FeType>::ComputeCoefficients(*(this->_f_element),
																												 this->_f,
																												 &(this->_coeff));
				};

			protected:
				//function whose projection error has to be computed
				FunctionPtr<dim> _f;
				//coefficients of the projection
				vector<double> _coeff;

				//TODO: verify if unique_ptr is better
				shared_ptr<AbstractFElement<dim, FeType> > _f_element;

				double _projection_error;
				bool	 _error_updated;
		};
} //namespace BinaryTree

#endif //__ABSTRACT_BINARY_ELEMENT_H
