#ifndef __BINARY_ELEMENT_H
#define __BINARY_ELEMENT_H

#include "Point.h"
#include "Maps.h"
#include "Element.h"
#include "FiniteFunction.h"

#include <face_tri3.h> //libMesh::Tri3
#include <edge_edge2.h> //libMesh::Edge2

#include <memory> //std::shared_ptr
#include <functional> //std::function

using namespace std;
using namespace Maps;

namespace BinaryTree
{
	using LibmeshInterval = libMesh::Edge2;
	using LibmeshTriangle = libMesh::Tri3;

	
	//TODO: verify if it's convenient to not derive BinTreeElement and then derive my concrete elements from three classes
	template <size_t DIM>
		class BinTreeElement : public FElement<DIM>
		{
			protected:
				BinTreeElement()
				{
					//TODO
				};

			public:
				virtual ~BinTreeElement()
				{
#ifdef MYDEBUG
				cout << "Distruggo BinTreeElement" << endl;
#endif //MYDEBUG
				};
				virtual elementType type() = 0;
				virtual nodes_ptr get_nodes() = 0;

				virtual double projectionError() const
				{
					return _projectionError;
				};

				virtual double projectionError(const function<double(Point<DIM>)>& f, size_t order)
				{
					FiniteFunction<DIM> fp (*this, f);

					//TODO: let the error norm to be modifiable, maybe to be choosable at runtime 
					//L2 norm of the interpolation error
					_projectionError = this->integrate	(
																		[&](const Point<DIM>& p){
																											double val = f(p)-fp(p);
																											return val*val;
																										}
																	);
					return _projectionError;
				};

			//TODO: could it be useful?
//				virtual double projectionError(size_t p_order, const function<double(Point<DIM>)>& f) const
//				{
//					FiniteFunction<DIM> fp (*this, f);
//					return integrate(
//											[&](const Point<DIM>& p){
//																				double val = f(p)-fp(p);
//																				return val*val;
//																			}
//										);
//				};

			protected:
				void projectionError(const double& val)
				{
					_projectionError = val;
				};

			protected:
				double _projectionError;
				//TODO: attributes needed by the algorithm
		};

	
	//It must be modified the way libMesh elements divides theirselves
	//The info about the procreation should be stored in the elem.embedding_matrix() method

	//TODO: check if the virtual inheritance could be dangerous in this case
	class Interval : public LibmeshInterval, public BinTreeElement<1>
	{
		public:
			Interval() : BinTreeElement<1>(){};
			virtual ~Interval()
			{
#ifdef MYDEBUG
				cout << "Distruggo Interval" << endl;
#endif //MYDEBUG
			};

			virtual elementType type()override
			{
				return LibmeshInterval::type();
			};
			virtual nodes_ptr get_nodes()override
			{
				return LibmeshInterval::get_nodes();
			};
		protected:

	};

	
	class Triangle : public LibmeshTriangle, public BinTreeElement<2>
	{
		public:
			Triangle() : BinTreeElement<2>(){};
			virtual ~Triangle(){};

			virtual elementType type()override
			{
				return LibmeshTriangle::type();
			};
			virtual nodes_ptr get_nodes()override
			{
				return LibmeshTriangle::get_nodes();
			};			
		protected:

	};


}; //namespace BinaryTree

#endif //__BINARY_ELEMENT_H
