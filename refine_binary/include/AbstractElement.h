#ifndef __ABSTRACT_H
#define __ABSTRACT_H

#include "Basis.h"
#include "Quadrature.h"
#include "Functor.h"

#include <functional> //function

#ifdef SIMMETRY_OPT
//TODO: use numeric_limits
#define TOLL 1E-18
#endif //SIMMETRY_OPT

namespace Geometry
{
	using namespace std;

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
				virtual size_t QuadratureOrder()const = 0;


				double Integrate(const function<double(Point<dim>)>& f) const
				{
					QuadWeightVec weights = GetQuadWeights();
					QuadWeightVec fval(weights.Size());
					QuadPointVec<dim> nodes = GetQuadPoints();

					//quadrature nodes and weights must have the same length
					if (nodes.Size() != weights.Size() )
						throw std::length_error("Trying to integrate with different number of nodes and weights!");

					//TODO: can be optimized using weights simmetry
					for (size_t i = 0; i < nodes.Size(); ++i)
					{
						auto p = nodes[i];
						fval[i] = f(p);
					}

					return fval.Dot(weights);
				};

				template <typename F1, typename F2>
				double L2Prod (const F1& f, const F2& g)const
				{
					return Integrate([&](const Point<dim>& p){return f(p)*g(p);});
				};

				template <typename F>
				double L2Norm (const F& f)const
				{
					return sqrt(L2Prod(f,f));
				};

				protected:
#ifdef SIMMETRY_OPT
					bool CheckSimmetry(const QuadWeightVec& vec) const
					{
						int length = vec.Size();
						int i = 0, j = length - 1;
						for(; i < length / 2; ++i, --j)
							if ( abs(vec(i) - vec(j)) > TOLL )
								return false;

						return true;
					};
#endif //SIMMETRY_OPT
		};

} //namespace Geometry

#endif //__ABSTRACT_H
