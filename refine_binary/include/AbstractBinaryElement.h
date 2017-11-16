#ifndef __ABSTRACT_BINARY_ELEMENT_H
#define __ABSTRACT_BINARY_ELEMENT_H

#include "BinaryNode.h"

#include <limits> //numeric_limits::max()

namespace BinaryTree
{
	using namespace FiniteElements;

	template <size_t dim, BasisType FeType = InvalidFeType>
		class AbstractBinaryElement : public DimensionedNode<dim> 
		{
			public:

				using CoeffVector = Geometry::ColumnVector;


				/* constructor */
				AbstractBinaryElement(	FunctionPtr<dim> f,
												AbstractFElement<dim, FeType>* el_ptr
											):
												DimensionedNode<dim> (),
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
					
					BinaryNode* daddy = this->Dad();
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
					for (size_t i = 0; i < this->_coeff.Size(); ++i)
						cout << (this->_coeff)[i] << "	";
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
					return *(this->_f_element);
				};

				virtual Geometry::NodesVector<dim> Nodes() override
				{
					return this->_f_element->GetNodes();
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
					double err = _f_element->L2Norm	(
																	[&](const Geometry::Point<dim>& p)
																	{return (*(this->_f))(p) - Projection(p);}
																);

					this->ProjectionError(err);
					this->_error_updated = true;
				};

				virtual double Projection(const Geometry::Point<dim>& point)
				{
					ComputeCoefficients();

					CoeffVector basis_evaluation = this->_f_element->EvaluateBasis(point);

					size_t basis_size = basis_evaluation.Size();
					//It may happen that I've stored more coefficients than needed
					//so I take the "head" of the coeff vector
					//i.e. the first basis_size coefficients  
					auto needed_coeff = (this->_coeff).Head(basis_size);
					return basis_evaluation.Dot(needed_coeff);
				};

//TODO: it works only if the basis is orthogonal
				void ComputeCoefficients()
				{
					//I remember how many coefficients have been already computed
					size_t cursor = (this->_coeff).Size();
					//New number of coefficients
					size_t s = this->_f_element->BasisSize();

					if (cursor >= s)
						//Coefficients already computed
						return;

					//memory allocation
					(this->_coeff).Resize(s);

					//I don't recompute already stored coefficients
					for(; cursor < s; ++cursor)
					{
						(this->_coeff)[cursor] = this->_f_element->L2Prod	(
																								*(this->_f),
																								[this, cursor]
																								(const Geometry::Point<dim>& p)
																								{ return this->_f_element->EvaluateBasisFunction(cursor, p);}
																							);

						double norm_squared = this->_f_element->BasisNormSquared(cursor);

						(this->_coeff)[cursor] /= norm_squared;
					}
				};

			protected:
/*
				The functor whose projection error has to be computed
*/
				FunctionPtr<dim> _f;

/*
				attribute created with optimization purpose
				it stores the coefficients of the projection, so they are not recomputed every time
				during the object construction all the coefficients that will be needed during the algorithm are computed,
				since adding other values when p_level gets higher implies the recomputing of the (already computed and stored)
				lower p_levels  coefficients (this because of how the evaluation of the legendre basis works,
				see "LegendreBasis.h" and/or "WarpedBasis.h").
				When evaluating the Projection, only the first BasisSize(p_level) coefficients will be used  
*/
				CoeffVector _coeff;

				unique_ptr<AbstractFElement<dim, FeType> > _f_element;

				double _projection_error;
				bool	 _error_updated;
		};
} //namespace BinaryTree

#endif //__ABSTRACT_BINARY_ELEMENT_H
