#ifndef __ABSTRACT_BINARY_ELEMENT_H
#define __ABSTRACT_BINARY_ELEMENT_H

#include "BinaryNode.h"

#include <limits> //numeric_limits::max()

namespace BinaryTree
{
	using namespace FiniteElements;

/**
	Abstract class for nodes of a binary tree of dimensionality dim.
	It defines the e, e~, E, E~, q parameters computation,
	and the projection of an objective function over the underlying finite element.
	Still missing the bisection method.
**/
	template <size_t dim, BasisType FeType = InvalidFeType>
		class AbstractBinaryElement : public DimensionedNode<dim> 
		{
			public:

				using CoeffVector = Geometry::ColumnVector;

/**
				constructor
**/
				AbstractBinaryElement(	FunctionPtr<dim>,
												AbstractFElement<dim, FeType>*);
/**
				default destructor
**/
				virtual ~AbstractBinaryElement();

/**
				Initialize the element.
				It sets the e, e~, E, E~, q, S parameter to their default values,
				i.e. the ones assumed by leafs.
**/
				virtual void Init();

/**
				Get the interpolation error on the element.
**/
				virtual double ProjectionError();
				
/**
				Print the projection coefficients to standard output.
**/
				virtual void PrintCoefficients() const;

/**
				Modify the p refinement level of the element.
**/
				virtual void PLevel(size_t);
/**
				Get the p refinement level of the element.
**/
				virtual size_t PLevel() const;

/**
				Get reference to underlying finite element.
**/
				const AbstractFElement<dim, FeType>& GetFElement() const;

/**
				Get vertices of underlying geometry.
**/
				virtual Geometry::NodesVector<dim> Nodes() const override;

			protected:
/**
				Set the projection error.
**/
				virtual void ProjectionError(const double&);

/**
				Compute the projection error in L^2 norm.
**/
				virtual void UpdateProjectionError();

/**
				Evaluate the interpolated function.
**/
				virtual double Projection(const Geometry::Point<dim>&) const;

/**
				Compute the coefficients of the projection of _f function.
				TODO: it assumes that the basis is orthogonal
**/
				void ComputeCoefficients() const;

			protected:
/**
				The functor whose projection error has to be computed.
**/
				FunctionPtr<dim> _f;

/**
				The finite element the function _f is projected on.
**/
				unique_ptr<AbstractFElement<dim, FeType> > _f_element;

/**
				The interpolation error
**/
				double _projection_error;

			private:
/**
				Flag telling if the _projection_error is updated.
				Every method which modifies the finite element shall set _error_updated = false.
				UpdateProjectionError() will then recompute the interpolation error, it is the
				only method able to set the _error_updated = true.
**/
				bool _error_updated;
/**
				Vector storing the coefficients of the projection, so they are not recomputed every time.
				Attribute created with optimization purpose.
				During the object construction all the coefficients that will be needed during the algorithm are computed,
				since adding other values when p_level gets higher implies the recomputing of the (already computed and stored)
				lower p_levels  coefficients (this because of how the evaluation of the legendre basis works,
				see "LegendreBasis.h" and/or "WarpedBasis.h").
				When evaluating the Projection, only the first BasisSize(p_level) coefficients will be used.
				If needed it is updated by const method Projection(), so it is labeled mutable.
**/
				mutable CoeffVector _coeff;
		};

	template <size_t dim, BasisType FeType>
		AbstractBinaryElement<dim,FeType>::AbstractBinaryElement	(	FunctionPtr<dim> f,
																						AbstractFElement<dim, FeType>* el_ptr
																					):
																						DimensionedNode<dim> (),
																						_f (f),
																						_f_element (el_ptr),
																						_projection_error (numeric_limits<double>::max()),
																						_error_updated (false),
																						_coeff ()
		{};

	template <size_t dim, BasisType FeType>
		AbstractBinaryElement<dim,FeType>::~AbstractBinaryElement()
		{};

	template <size_t dim, BasisType FeType>
		void AbstractBinaryElement<dim,FeType>::Init()
		{
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

	template <size_t dim, BasisType FeType>
		double AbstractBinaryElement<dim,FeType>::ProjectionError()
		{
			if (! _error_updated)
				UpdateProjectionError();

			return this->_projection_error;
		};
				
	template <size_t dim, BasisType FeType>
		void AbstractBinaryElement<dim,FeType>::PrintCoefficients() const
		{
			ComputeCoefficients();
			for (size_t i = 0; i < this->_coeff.Size(); ++i)
				cout << (this->_coeff)[i] << "	";
			cout << endl;
		};

	template <size_t dim, BasisType FeType>
		void AbstractBinaryElement<dim,FeType>::PLevel(size_t val)
		{
			this->_f_element->PLevel(val);
			this->_error_updated = false;
		};

	template <size_t dim, BasisType FeType>
		size_t AbstractBinaryElement<dim,FeType>::PLevel() const
		{
			return this->_f_element->PLevel();
		};

	template <size_t dim, BasisType FeType>
		const AbstractFElement<dim, FeType>& AbstractBinaryElement<dim,FeType>::GetFElement()const
		{
			return *(this->_f_element);
		};

	template <size_t dim, BasisType FeType>
		Geometry::NodesVector<dim> AbstractBinaryElement<dim,FeType>::Nodes() const
		{
			return this->_f_element->GetNodes();
		};

	template <size_t dim, BasisType FeType>
		void AbstractBinaryElement<dim,FeType>::ProjectionError(const double& val)
		{
			this->_projection_error = val;
		};

	template <size_t dim, BasisType FeType>
		void AbstractBinaryElement<dim,FeType>::UpdateProjectionError()
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

	template <size_t dim, BasisType FeType>
		double AbstractBinaryElement<dim,FeType>::Projection(const Geometry::Point<dim>& point) const
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

	template <size_t dim, BasisType FeType>
		void AbstractBinaryElement<dim,FeType>::ComputeCoefficients() const
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

} //namespace BinaryTree

#endif //__ABSTRACT_BINARY_ELEMENT_H
