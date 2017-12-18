#ifndef __STD_F_ELEMENT_INTERFACE_H
#define __STD_F_ELEMENT_INTERFACE_H

#include "StdElement.h"
#include "AbstractSpace.h"


namespace FiniteElements
{
	using namespace std;

	/**
		Interface of a standard element.
		This is a hat class for StdFElement which represents the return type of the standard FElements factory.
		ElementType parameter cannot be set as template parameter, since it is known at runtime.
		As for generic FElements, a standard FElement has to provide the abstract space interface and the
		abstract element one.
	**/
	template <size_t dim, BasisType FeType = InvalidFeType>
	class StdFElementInterface :
		public AbstractSpaceInterface<dim>,
		public Geometry::AbstractElement<dim>
	{
	  protected:
		/**
			default constructor.
		**/
		StdFElementInterface();

	  public:
		/**
			destructor.
		**/
		virtual ~StdFElementInterface();
		/*
			AbstractSpaceInterface methods
		*/
		virtual double	EvaluateBasisFunction (size_t ind,
											   const Geometry::Point<dim>&)const = 0;
		virtual size_t	BasisSize (size_t)const = 0;
		virtual BasisType GetFeType ()const = 0;

		/*
			AbstractElement methods
		*/
		virtual Geometry::ElementType GetType() const = 0;
		virtual Geometry::QuadPointVec<dim> GetQuadPoints() const = 0;
		virtual Geometry::QuadWeightVec GetQuadWeights() const = 0;
		virtual size_t QuadratureOrder() const = 0;

		/* from now on stuff introduced with optimization purpose */

		/**
			Evaluate whole basis untill fixed degree.
			Input parameters:
				- degree of the basis to be evaluated
				- point of evaluation
		**/
		virtual Geometry::Vector EvaluateBasis (size_t degree,
												const Geometry::Point<dim>&) const = 0;
		/**
			The square of the L2 norm of basis function with input index.
			Method implementing optimized norm computation of basis functions.
			If the value is already stored in _norm_values attribute, it returns it,
			otherwise it computes it through the pure virtual ComputeNormSquared method,
			which will be defined by derived classes.
		**/
		double BasisNormSquared (size_t);

		/**
			Set the _max_degree attribute.
			It set the _max_degree attribute to the input parameter,
			so when the basis evaluation will be firstly called
			the first _max_degree norms will be called.
			Needed for optimization purposes:
			if the maximum number of basis norms is known a priori,
			all them can be immediately computed, even if they are
			not immediately needed.
			Depending on the basis evaluation algorithm, this method could be
			much more performant than compute each basis norm when effectively needed.
		**/
		static void InitNorms (size_t);
	  protected:
		/**
			Compute the norm of the basis of input index.
		**/
		virtual double ComputeNormSquared (size_t) = 0;
		/**
			Update the norms vector.
			If the norms vector is shorter than a _max_degree basis size,
			it computes the missing norms.
		**/
		virtual void UpdateNorms();

	  protected:
		/**
			The values of basis functions norms.
		**/
		Geometry::Vector _norm_values;
		/**
			The maximum basis degree needed.
			It is updated by UpdateNorms() method,
			but it can be also set a priori through the InitNorms() method.
		**/
		static size_t _max_degree;
	};

	/* Initialization of the static attribute */
	template <size_t dim, BasisType FeType>
	size_t StdFElementInterface<dim, FeType>::_max_degree = 0;

	template <size_t dim, BasisType FeType>
	void StdFElementInterface<dim, FeType>::InitNorms (size_t degree)
	{
		_max_degree = degree;
	};

	template <size_t dim, BasisType FeType>
	StdFElementInterface<dim, FeType>::StdFElementInterface() : _norm_values()
	{};

	template <size_t dim, BasisType FeType>
	StdFElementInterface<dim, FeType>::~StdFElementInterface()
	{
		/*I reset to zero the static attribute.
		If a new std element is created after the destruction of this one,
		I want the InitNorms method to be called again */
		this->_max_degree = 0;
	};

	template <size_t dim, BasisType FeType>
	double StdFElementInterface<dim, FeType>::BasisNormSquared (size_t ind)
	{
		while ( ind >= this->BasisSize (_max_degree))
			++_max_degree;

		this->UpdateNorms();

		return (this->_norm_values)[ind];
	};

	template <size_t dim, BasisType FeType>
	void StdFElementInterface<dim, FeType>::UpdateNorms()
	{
		auto l = (this->_norm_values).Size();
		auto new_l = this->BasisSize (_max_degree);

		if (l < new_l)
		{
#ifdef VERBOSE
			clog << "Resizing the basis norm values vector" << endl;
#endif //VERBOSE
			(this->_norm_values).Resize (new_l);
		}

		for (size_t i = l; i < new_l; ++i)
			(this->_norm_values)[i] = this->ComputeNormSquared (i);
	};


} //namespace FiniteElements

#endif //__STD_F_ELEMENT_INTERFACE_H
