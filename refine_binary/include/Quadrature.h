#ifndef __QUADRATURE_H
#define __QUADRATURE_H

#include "Point.h"
#include "TypeEnumerations.h"
#include "AbstractFactory.h"

#include "Eigen/Dense"

namespace Geometry
{
/*
	Type used to store quadrature nodes
*/
		//TODO: verify if it could be an optimization to store it as a Matrix
		//			with particular focus on the mapping process
	template <size_t dim>
		using QuadPointVec = std::vector<Point<dim>>;
/*
	Type used to store quadrature weigths
*/
	using QuadWeightVec = Eigen::Matrix <double, 1, Eigen::Dynamic>;

	template <size_t dim>
		class QuadratureRuleInterface
		{
			public:
				virtual ~QuadratureRuleInterface(){};
				virtual QuadPointVec<dim> GetPoints() = 0;
				virtual QuadWeightVec GetWeights() = 0;
				virtual size_t Order() = 0;
			protected:
				QuadratureRuleInterface()
				{
#ifdef MYDEBUG
					std::cout << "Sono nel costruttore di QuadratureRuleInterface" << std::endl;
#endif //MYDEBUG
				};
		};

/*
	Factory for quadrature rules
	The key is the type of the element the quadrature rule is constructed on
	The registration is done by the dynamic library implementing the rule
*/
	template <size_t dim>
		using QuadratureFactory = GenericFactory::ObjectFactory <QuadratureRuleInterface<dim>, ElementType>;
} //namespace Geometry

#endif //__QUADRATURE_H
