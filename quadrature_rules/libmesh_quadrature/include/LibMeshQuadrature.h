#ifndef __LIBMESH_QUADRATURE_H
#define __LIBMESH_QUADRATURE_H

#include <quadrature.h> //libMesh::QBase

#include "Quadrature.h" //ElementType

#include "LibMeshHelper.h"

namespace LibmeshBinary
{
	template <size_t dim, Geometry::ElementType Type>
		class LibmeshQuadratureRule : public Geometry::QuadratureRuleInterface<dim>
		{
			public:
				LibmeshQuadratureRule()
				{
					_order = libMesh::FORTYTHIRD;
					//TODO: use GetPot to configure quadrature options
					_quadrature_rule = libMesh::QBase::build(libMesh::QGAUSS, dim, _order);
					_quadrature_rule->init(ConvertType<Type>());
				};

				virtual ~LibmeshQuadratureRule(){};

				virtual Geometry::QuadPointVec<dim> GetPoints() override
				{
					vector<libMesh::Point> std_points = _quadrature_rule->get_points();
					Geometry::QuadPointVec<dim> result;
					//TODO: convert a libmesh point into a point<dim>
					for (auto lp : std_points)
						result.push_back(ConvertPoint<dim>(lp));

					return result;
				};

				virtual Geometry::QuadWeightVec GetWeights() override
				{
					vector<libMesh::Real> std_weights = _quadrature_rule->get_weights();
					Geometry::QuadWeightVec result(std_weights.size());
					size_t i(0);
					for (auto w : std_weights)
						result(i++) = static_cast<double>(w);

					return result;
				};

				virtual size_t Order() override
				{
					return static_cast<size_t>(this->_order);
				}; 

			private:
				libMesh::UniquePtr<libMesh::QBase> _quadrature_rule;
				libMesh::Order _order;
		};

} //namespace LibmeshBinary

#endif //__LIBMESH_QUADRATURE_H
