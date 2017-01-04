#ifndef __ABSTRACT_H
#define __ABSTRACT_H

#include "Basis.h"
#include "Quadrature.h"
#include "Functor.h"

#include <functional> //function

//TODO: use numeric_limits
#define TOLL 1E-18
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

				double Integrate(const BinaryTree::Functor<dim>& f)
				{
					return Integrate([&f](const Point<dim>& p){return f(p);});
				};

				double Integrate(const function<double(Point<dim>)>& f) const
				{
					QuadWeightVec weights = GetQuadWeights();
					QuadWeightVec fval(weights.size());
					QuadPointVec<dim> nodes = GetQuadPoints();

					//quadrature nodes and weights must have the same length
					if (nodes.size() != static_cast<size_t> (weights.size()) )
						throw std::length_error("Trying to integrate with different number of nodes and weights!");

#ifdef SIMMETRY_OPT
					bool simmetric = CheckSimmetry(weights);

					if (simmetric)
					{
						int length = weights.size();
						int add_if_odd = length % 2;
						int new_length = length / 2 + add_if_odd;

//						cout << "Pesi prima" << endl;
//						for (int i = 0; i < length; ++i)
//							cout << weights(i) << " ";
//						cout << endl;

						weights.conservativeResize (new_length);
//						cout << "Pesi dopo" << endl;
//						for (int i = 0; i < new_length; ++i)
//							cout << weights(i) << " ";
//						cout << endl;

						fval.resize (new_length);

						auto left_iter = nodes.begin();
						auto right_iter = nodes.rbegin();
						int cont(0);
						for (; cont < length / 2; ++cont)
						{
							double val = f(*(left_iter++)) + f(*(right_iter++));
							fval(cont) = (/*val < TOLL ? 0.0 : */val);
						}

						if(add_if_odd)
							fval(cont) = f(nodes[cont]);
					}
					else
#endif //SIMMETRY_OPT
					{
						size_t cont(0);
						for (auto p : nodes)
							fval(cont++) = f(p);
					}

					return fval.dot(weights);
				};

				template <typename F1, typename F2>
				double L2prod (const F1& f, const F2& g)const
				{
					return Integrate([&](const Point<dim>& p){return f(p)*g(p);});
				};

				protected:
					bool CheckSimmetry(const QuadWeightVec& vec) const
					{
						int length = vec.size();
						int i = 0, j = length - 1;
						for(; i < length / 2; ++i, --j)
							if ( abs(vec(i) - vec(j)) > TOLL )
								return false;

						return true;
					};
		};

} //namespace Geometry

#endif //__ABSTRACT_H
