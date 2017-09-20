#ifndef __MAPS_H
#define __MAPS_H

#include "Point.h"
#include "AbstractFactory.h"
#include "TypeEnumerations.h"

#include "Eigen/Dense"

//TODO insert a control on library reference elements
//		 be sure that if something changes the maps still do their job 

namespace Geometry
{
	template <size_t dim>
		using NodesVector = vector<Point<dim>>;

	template <size_t dim>
		class Map
		{
			public:
				Map(){};

				//It returns the mapped point
				virtual Point<dim> Evaluate			(const Point<dim>&)const = 0;

				//It computes the inverse mapping function
				virtual Point<dim> ComputeInverse	(const Point<dim>&)const = 0;

				//It returns the evaluation of the jacobian at the input point
				virtual double		 EvaluateJacobian	(const Point<dim>&)const = 0;

				virtual ~Map(){};
			protected:

		};

/*
	maps from the reference element to the generic element
	the reference elements considered are represented before each specific map definition;
	the library which provides the quadrature nodes must ensure
	that the library reference elements are the same, otherwise they have to be mapped 
*/
	template <size_t dim>
		class AffineMap : public Map<dim>
		{
			//only derived classes can be constructed, so protected constructor
			protected:
				AffineMap(){};

			public:
				using MatrixType			= Eigen::Matrix <double, static_cast<int>(dim), static_cast<int>(dim)>;
				using VecType				= Eigen::Matrix <double, static_cast<int>(dim), 1>;
				using VecMapType			= Eigen::Map	 <VecType>;
				using ConstVecMapType	= Eigen::Map	 <const VecType>;
				using MatrixMapType		= Eigen::Map	 <MatrixType>;
				using ConstMatrixMapType= Eigen::Map	 <const MatrixType>;

				virtual void Init(const NodesVector<dim>&) = 0;
				virtual ~AffineMap()
				{
					//TODO
#ifdef DESTRUCTOR_ALERT
				cout << "Distruggo AffineMap" << endl;
#endif //DESTRUCTOR_ALERT
				};

				//TODO derive Point<dim> from Eigen type and modify consequently the evaluation
/*
				The Point<dim> p belongs to the reference element
				The returned one belongs to the element which owns the map object	
*/
				virtual Point<dim> Evaluate(const Point<dim>& p) const override
				{
					Point<dim> result;
					VecMapType eig_result(result.data());
					ConstVecMapType eig_p (p.data());
					eig_result = this->_mat * eig_p + this->_trasl;
					return result;
				};

/*
				It is the inverse function of the bijective map.
				p belongs to the generic element, while the returned value to the reference element
*/
				virtual Point<dim> ComputeInverse(const Point<dim>& p) const override
				{
					Point<dim> result;
					VecMapType eig_result(result.data());
					ConstVecMapType eig_p (p.data());
					eig_result = (this->_inverse) * (eig_p - this->_trasl);
					return result;
				};

				virtual double	EvaluateJacobian(const Point<dim>& p) const
				{
					//to silence the warning compiler, since the map is affine the jacobian does not depend on the evaluation point 
					(void)p;
					return this->_jacobian;
				};

				//to use the fact that jacobian is constant for affine maps
				virtual double	Jacobian()const
				{
					return this->_jacobian;
				};

			protected:
				MatrixType	_mat;
				MatrixType	_inverse;
				VecType		_trasl;
				double		_jacobian;
		};

/*
	reference interval:

	 |--------------|
	(-1)				(1)
*/
	class IntervalMap : public AffineMap<1>
	{
		public:
			IntervalMap(){};
			virtual ~IntervalMap(){};
			virtual void Init(const NodesVector<1>& vert) override;

			IntervalMap					(const IntervalMap&) = default;
			IntervalMap& operator = (const IntervalMap&) = default;	
	};


/*
	reference triangle:

	(0,1)
		|\
		| \
		|  \
		|   \
		|____\
	(0,0)		(1,0)
*/
	class TriMap : public AffineMap<2>
	{
		public:
			TriMap(){};

			virtual void Init(const NodesVector<2>& vert) override;

			virtual ~TriMap(){};

			TriMap					(const TriMap&) = default;
			TriMap& operator =	(const TriMap&) = default;	
	};


/*
	reference square:

  (-1,1)________(1,1)
		|			 |
		|		    |
		|		    |
		|		    |
		|_________|
  (-1,-1)		(1,-1)
*/
	class QuadMap : public AffineMap<2>
	{
		public:
			QuadMap(){};

			//TODO
			virtual void Init(const NodesVector<2>& vert) = 0;

			virtual ~QuadMap(){};

			QuadMap					(const QuadMap&) = default;
			QuadMap& operator = 	(const QuadMap&) = default;	
	};

/*
	Now maps (which are not affine) between standard elements and standard ipercube
*/

/*
	It maps the reference square in the reference triangle
	It's a singular map
*/
	class StdTriMap : public Map<2> 
	{
		public:
			StdTriMap(){};

			virtual ~StdTriMap(){};

			StdTriMap					(const StdTriMap&) = default;
			StdTriMap& operator = 	(const StdTriMap&) = default;	

			virtual Point<2> Evaluate (const Point<2>& p)const override;

			virtual Point<2> ComputeInverse (const Point<2>& p)const override;

			virtual double	EvaluateJacobian (const Point<2>& p)const override;
	};

} //namespace Geometry

#endif //__MAPS_H
