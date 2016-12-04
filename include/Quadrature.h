#ifndef __QUADRATURE_H
#define __QUADRATURE_H

#include "Point.h"
#include "TypeEnumerations.h"

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
		};

} //namespace Geometry

#endif //__QUADRATURE_H
