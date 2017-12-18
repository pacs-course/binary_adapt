#ifndef __MAPS_H
#define __MAPS_H

#include "LinearAlgebra.h"
#include "AbstractFactory.h"
#include "TypeEnumerations.h"

//TODO insert a control on library reference elements
//		 be sure that if something changes the maps still do their job

namespace Geometry
{
	/**
		Abstract class for map function.
		It represent a map function from R^@pdim to R^@pdim;
		it maps points between elements with dimensionality @pdim.
	**/
	template <size_t dim>
	class Map
	{
	  public:
		/**
			default constructor
		**/
		Map() {};
		/**
			default destructor
		**/
		virtual ~Map() {};

		/**
			Get the mapped point.
			It returns the result of the map evaluation at input point.
		**/
		virtual Point<dim> Evaluate (const Point<dim>&) const = 0;

		/**
			Get the inverse map evaluation.
			It returns the result of the evaluation at input point of the inverse map.
		**/
		virtual Point<dim> ComputeInverse (const Point<dim>&) const = 0;

		/**
			Get the map Jacobian evaluated at the input point.
		**/
		virtual double	EvaluateJacobian (const Point<dim>&) const = 0;
	};

	/**
		Abstract class for affine transformation in R^dim.
		The evaluation can be represented as the composition of a linear application and a traslation,
		i.e. x -> L*x + b, where L is a dim x dim matrix, x and b column vector in R^dim.
		Since the map is linear, its jacobian is constant.

		The Init() method, which depends on the geometry, has to be called after the construction,
		and it is defined in concrete derived classes.

		Affine maps are used to map reference elements to the correspondent generic element.

		The QuadratureRuleInterface has to ensure that reference elements considered are the same
		of the ones used by the concrete affine maps.
	**/
	template <size_t dim>
	class AffineMap : public Map<dim>
	{
	  protected:
		/**
			default constructor.
			Attributes are initialized by Init() method
		**/
		AffineMap() {};

	  public:
		/**
			default destructor.
		**/
		virtual ~AffineMap()
		{};

		/**
			Initialize the attribute.
			It depends on the geometry.
			It takes as input the vertexes of the generic element
			the reference element will be mapped to.
		**/
		virtual void Init (const NodesVector<dim>&) = 0;

		/**
			Map evaluation x -> L*x + b.
			The input Point<dim> belongs to the reference element;
			the returned one belongs to the generic element which owns the map object.
		**/
		virtual Point<dim> Evaluate (const Point<dim>& p) const override
		{
			return this->_mat * p + this->_trasl;
		};

		/**
			Evaluate the map in a series of points.
			Depending on the MatrixType and VectorPoint implementation,
			this method can be more performant than:
				for p in input_vector
					Evaluate(p);
		**/
		virtual VectorPoint<dim> Evaluate (const VectorPoint<dim>& vec) const
		{
			return this->_mat * vec + this->_trasl;
		};

		/**
			Evaluate the inverse map.
			Input Point<dim> belongs to the generic element;
			the returned one belongs to the reference element.
		**/
		virtual Point<dim> ComputeInverse (const Point<dim>& p) const override
		{
			return (this->_inverse) * (p - this->_trasl);
		};

		/**
			Evaluate the inverse map in a series of points.
			Depending on the MatrixType and VectorPoint implementation,
			this method can be more performant than:
				for p in input_vector
					ComputeInverse(p);
		**/
		virtual VectorPoint<dim> ComputeInverse (const VectorPoint<dim>& vec) const
		{
			return this->_inverse * (vec - this->_trasl);
		};

		/**
			Get the constant Jacobian of the map.
		**/
		virtual double	EvaluateJacobian (const Point<dim>&) const override
		{
			//since the map is affine the jacobian does not depend on the evaluation point
			return this->_jacobian;
		};

		/**
			Get the constant Jacobian of the map.
			No need of input parameters.
		**/
		virtual double	Jacobian()const
		{
			return this->_jacobian;
		};

	  protected:
		/**
			Linear application matrix
		**/
		MatrixType<dim>	_mat;
		/**
			Matrix representative of the inverse application
		**/
		MatrixType<dim>	_inverse;
		/**
			Traslation vector
		**/
		VecType<dim> _trasl;
		/**
			Constant Jacobian of the map
		**/
		double _jacobian;
	};

	/**
		Affine map between reference interval and generic interval.
		Reference interval:

		 |--------------|
		(-1)				(1)
	**/
	class IntervalMap : public AffineMap<1>
	{
	  public:
		/**
			default constructor
		**/
		IntervalMap();
		/**
			default destructor
		**/
		virtual ~IntervalMap();
		/**
			Initialization takes as input generic interval boundaries.
			Generic interval:	|--------|
								a		 b
			Map: x -> x*(b-a)/2 + (b+a)/2
		**/
		virtual void Init (const NodesVector<1>& vert) override;

		/**
			default copy constructor
		**/
		IntervalMap (const IntervalMap&) = default;
		/**
			default assignment operator
		**/
		IntervalMap& operator = (const IntervalMap&) = default;
	};


	/**
		Affine map between reference triangle and generic triangle.
		Reference triangle:

		(-1,1)
			|\
			| \
			|  \
			|   \
			|____\
		(-1,-1)	(1,-1)
	**/
	class TriMap : public AffineMap<2>
	{
	  public:
		/**
			default constructor
		**/
		TriMap();
		/**
			default destructor
		**/
		virtual ~TriMap();

		/**
			Initialization takes as input generic triangle vertexes.
		**/
		virtual void Init (const NodesVector<2>& vert) override;

		/**
			default copy constructor
		**/
		TriMap (const TriMap&) = default;
		/**
			default assignment operator
		**/
		TriMap& operator = (const TriMap&) = default;
	};


	/**
		Affine map between reference square and generic square.
		Reference square:

	    (-1,1)__________(1,1)
			|			|
			|		    |
			|		    |
			|		    |
			|			|
			|___________|
	    (-1,-1)			(1,-1)
	**/
	class QuadMap : public AffineMap<2>
	{
	  public:
		/**
			default constructor
		**/
		QuadMap();
		/**
			default destructor
		**/
		virtual ~QuadMap();

		//TODO
		/**
			Initialization takes as input generic square vertexes.
		**/
		virtual void Init (const NodesVector<2>& vert) = 0;

		/**
			default copy constructor
		**/
		QuadMap (const QuadMap&) = default;
		/**
			default assignment operator
		**/
		QuadMap& operator = (const QuadMap&) = default;
	};


	//	Now maps (which are not affine) between standard elements and standard ipercube


	/**
		Map from the reference square to the reference triangle.
		It implements the singular map which takes a point of the
		reference square, that is the 2D ipercube, and maps it to the reference triangle.

		Formulas for evaluation of direct and inverse map, and for the jacobian taken from:
			Quarteroni, "Numerical Models for Differential Problems", 2nd edition; p 261-262
	**/
	class StdTriMap : public Map<2>
	{
	  public:
		/**
			default constructor
		**/
		StdTriMap();
		/**
			default destructor
		**/
		virtual ~StdTriMap();
		/**
			default copy constructor
		**/
		StdTriMap (const StdTriMap&) = default;
		/**
			default assignment operator
		**/
		StdTriMap& operator = (const StdTriMap&) = default;

		virtual Point<2> Evaluate (const Point<2>& p)const override;

		virtual Point<2> ComputeInverse (const Point<2>& p)const override;

		virtual double	EvaluateJacobian (const Point<2>& p)const override;
	};

} //namespace Geometry

#endif //__MAPS_H
