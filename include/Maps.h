#ifndef __MAPS_H
#define __MAPS_H

#include <utility> //pair

#include "AbstractFactory.h"
#include "Point.h"

#include "Eigen/Dense"

#include <face_quad4.h>
#include <face_tri3.h>
#include <edge_edge2.h>

/*
	maps from generic elements to reference element
	the reference elements considered are the same of libmesh library,
	in order to use quadrature nodes and weights provided by libmesh
*/

//TODO insert a control on library reference elements
//		 be sure that if something changes the maps still do their job 

using namespace Geometry;

namespace Maps
{
	using libMeshInterval = libMesh::Edge2;
	using libMeshSquare   = libMesh::Quad4;
	using libMeshTriangle = libMesh::Tri3;

	using nodes_ptr = const libMesh::Node* const*;

	template <size_t DIM>
		class Map
		{
			public:
				Map(){};

				//It returns the mapped point
				virtual Point<DIM> evaluate			(const Point<DIM>&)const = 0;

				//It computes the inverse mapping function
				virtual Point<DIM> computeInverse	(const Point<DIM>&)const = 0;

				//It returns the evaluation of the jacobian at the input point
				virtual double		 evaluateJacobian	(const Point<DIM>&)const = 0;

				virtual ~Map(){};
			protected:

		};

	template <size_t DIM>
		class AffineMap : public Map<DIM>
		{
			//only derived classes can be constructed, so protected constructor
			protected:
				AffineMap(){};

			public:
				using MatrixType			= Eigen::Matrix <double, static_cast<int>(DIM), static_cast<int>(DIM)>;
				using VecType				= Eigen::Matrix <double, static_cast<int>(DIM), 1>;
				using VecMapType			= Eigen::Map	 <VecType>;
				using constVecMapType	= Eigen::Map	 <const VecType>;
				using MatrixMapType		= Eigen::Map	 <MatrixType>;
				using constMatrixMapType= Eigen::Map	 <const MatrixType>;

				virtual void init(nodes_ptr) = 0;
				virtual ~AffineMap()
				{
					//TODO
#ifdef DESTRUCTOR_ALERT
				cout << "Distruggo AffineMap" << endl;
#endif //DESTRUCTOR_ALERT
				};

				//TODO derive Point<DIM> from Eigen type and modify consequently the evaluation
				virtual Point<DIM> evaluate(const Point<DIM>& p)const override
				{
					Point<DIM> result;
					VecMapType eig_result(result.data());
					constVecMapType eig_p (p.data());
					eig_result = _mat * eig_p + _trasl;
					return result;
				};

				virtual Point<DIM> computeInverse	(const Point<DIM>& p)const override
				{
					Point<DIM> result;
					VecMapType eig_result(result.data());
					constVecMapType eig_p (p.data());
					eig_result = _inverse * (eig_p - _trasl);
					return result;
				};

				virtual double	evaluateJacobian(const Point<DIM>& p)const
				{
					//to silence the warning compiler, since the map is affine the jacobian does not depend on the evaluation point 
					(void)p;
					return _jacobian;
				};

				//to use the fact that jacobian is constant for affine maps
				virtual double	Jacobian()const
				{
					return _jacobian;
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
			IntervalMap()
			{
				//TODO
			};
			virtual ~IntervalMap(){};
			virtual void init(nodes_ptr vert)
			{
				double a = (*(vert[0]))(0);
				double b = (*(vert[1]))(0);
//#ifdef MYMYMYDEBUG
//				cout << "Primo estremo: " << a << endl;
//				cout << "Secondo estremo: " << b << endl;
//#endif //MYMYMYDEBUG
//				this->_mat(0) = (b - a) / 2; //ECCOLO!!!
//				this->_trasl(0) = (a + b) / 2; //ECCOLO!!!
				this->_jacobian = this->_mat(0);
				this->_inverse(0) = 1 / this->_mat(0);
			};

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
			TriMap()
			{
				//TODO
			};

			void init(nodes_ptr vert)
			{
				double x1 = (*(vert[0]))(0);
				double y1 = (*(vert[0]))(1);
				double x2 = (*(vert[1]))(0);
				double y2 = (*(vert[1]))(1);
				double x3 = (*(vert[2]))(0);
				double y3 = (*(vert[2]))(1);

				this->_mat(0,0) = x2 - x1;
				this->_mat(0,1) = x3 - x1;
				this->_mat(1,0) = y2 - y1;
				this->_mat(1,1) = y3 - y1;

				this->_jacobian = _mat.determinant();

				this->_trasl(0) = x1;
				this->_trasl(1) = y1;

				//TODO: optimize it
				this->_inverse = _mat.inverse();
			};

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
			QuadMap()
			{
				//TODO
			};

			QuadMap(libMeshSquare* element)
			{
				//TODO
			};

			virtual ~QuadMap(){};

			QuadMap					(const QuadMap&) = delete;
			QuadMap& operator = 	(const QuadMap&) = delete;	
	};


	/*
		It maps the reference square in the reference triangle
		It's a singular map
	*/
	class StdTriMap : public Map<2> 
	{
		public:
			StdTriMap()
			{
				//TODO
			};

			virtual ~StdTriMap()
			{
				//TODO
			};

			StdTriMap					(const StdTriMap&) = default;
			StdTriMap& operator = 	(const StdTriMap&) = default;	

			virtual Point<2> evaluate (const Point<2>& p)const override
			{
				Point<2> result;
				double a = p[0];
				double b = p[1];
				result[0] = (1 + a) * (1 - b) / 4;
				result[1] = (1 + b) / 2;
				return result;
			};

			virtual Point<2> computeInverse (const Point<2>& p)const override
			{
				Point<2> result;
				double csi = p[0];
				double eta = p[1];
				result[0] = 2 * csi / (1 - eta) - 1;
				result[1] = 2 * eta - 1;
				return result;
			};

			virtual double	evaluateJacobian (const Point<2>& p)const override
			{
				return (1 - p[1]) / 4;
			};
	};
};//namespace Maps

#endif //__MAPS_H
