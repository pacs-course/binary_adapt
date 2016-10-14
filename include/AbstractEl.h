#ifndef __ABSTRACT_H
#define __ABSTRACT_H

#include "Basis.h"

#include "Eigen/Dense"

#include "enum_elem_type.h"

#include <functional> //function

using namespace std;
using namespace Geometry;

namespace BinaryTree
{

/*
	Type used to store quadrature nodes
*/
	//TODO: verify if it could be an optimization to store it as a Matrix
	//			with particular focus on the mapping process
	template <size_t DIM>
		using QuadPointVec = vector<Point<DIM>>;
/*
	Type used to store quadrature weigths
*/
	using QuadWeightVec = Eigen::Matrix <double, 1, Eigen::Dynamic>;

//TODO: maybe create an interface for libmesh quadrature rules


	using elementType = libMesh::ElemType;

/*
	Abstract class: it implements all the (geometry independent) stuff
	that an element of the binary tree must have to be used for the
	binary-tree hp-adaptivity algorithm proposed in Binev paper
*/
	template <size_t DIM>
		class AbstractElement
		{
			public:
				AbstractElement()
				{};

				virtual ~AbstractElement()
				{
#ifdef DESTRUCTOR_ALERT
				cout << "Distruggo AbstractElement" << endl;
#endif //DESTRUCTOR_ALERT
				};

				virtual elementType type() = 0;

				virtual QuadPointVec<DIM> getQuadPoints()const = 0;
				virtual QuadWeightVec getQuadWeights()const = 0;

				double integrate(const function<double(Point<DIM>)>& f) const
				{
					QuadWeightVec w = getQuadWeights();
					QuadWeightVec fval(w.size());
					QuadPointVec<DIM> nodes = getQuadPoints();

					//quadrature nodes and weights must have the same length
					if (nodes.size() != static_cast<size_t> (w.size()) )
						throw std::length_error("Trying to integrate with different number of nodes and weights!");

					size_t i(0);
					for (auto p : nodes)
						fval(i++) = f(p);

					return fval.dot(w);
				};

				double L2prod (const function<double(Point<DIM>)>& f, const function<double(Point<DIM>)>& g)const
				{
					return integrate([&](const Point<DIM>& p){return f(p)*g(p);});
				};
		};


	template <size_t DIM>
		class AbstractSpace
		{
			public:
				AbstractSpace()
				{			};

				virtual ~AbstractSpace()
				{
#ifdef DESTRUCTOR_ALERT
				cout << "Distruggo AbstractSpace" << endl;
#endif //DESTRUCTOR_ALERT
				};

				//TODO: maybe if evaluateBasisFunction and evaluateBasis are defined separately they are better optimizable
				//		  verify it and, if not, define here evaluateBasis in terms of evaluateBasisFunction
				virtual double				evaluateBasisFunction(size_t ind, const Point<DIM>& point)const = 0;
				virtual vector<double>	evaluateBasis			(				 const Point<DIM>& point)const = 0;

				virtual size_t	basisSize()const = 0;

				virtual Basis::basisType FEType() = 0;
		};

};//namespace BinaryTree

#endif //__ABSTRACT_H
