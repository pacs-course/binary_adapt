#ifndef __ABSTRACT_H
#define __ABSTRACT_H

#include "Basis.h"
#include "Quadrature.h"

#include <functional> //function

using namespace std;

namespace Geometry
{
/*
	Abstract class: it implements all the (geometry independent) stuff
	that an element of the binary tree must have to be used for the
	binary-tree hp-adaptivity algorithm proposed in Binev paper
*/
	template <size_t dim>
		class AbstractElement
		{
			public:
				virtual ~AbstractElement()
				{
#ifdef DESTRUCTOR_ALERT
				cout << "Distruggo AbstractElement" << endl;
#endif //DESTRUCTOR_ALERT
				};

				virtual ElementType GetType() const = 0;

				virtual QuadPointVec<dim>	GetQuadPoints() const = 0;
				virtual QuadWeightVec		GetQuadWeights()const = 0;

				double Integrate(const function<double(Point<dim>)>& f) const
				{
					QuadWeightVec w = GetQuadWeights();
					QuadWeightVec fval(w.size());
					QuadPointVec<dim> nodes = GetQuadPoints();

					//quadrature nodes and weights must have the same length
					if (nodes.size() != static_cast<size_t> (w.size()) )
						throw std::length_error("Trying to integrate with different number of nodes and weights!");

					size_t i(0);
					for (auto p : nodes)
						fval(i++) = f(p);

					return fval.dot(w);
				};

				double L2prod (const function<double(Point<dim>)>& f, const function<double(Point<dim>)>& g)const
				{
					return Integrate([&](const Point<dim>& p){return f(p)*g(p);});
				};
		};

} //namespace Geometry

#endif //__ABSTRACT_H
