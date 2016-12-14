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
					//TODO: use GetPot to configure quadrature options
					auto ptr = libMesh::QBase::build(libMesh::QGAUSS, dim, libMesh::FORTYTHIRD);
					_quadrature_rule = unique_ptr<libMesh::QBase>(ptr.release());
					_quadrature_rule->init(ConvertType<Type>());
				};

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

			private:
				unique_ptr<libMesh::QBase> _quadrature_rule;
		};

} //namespace LibmeshBinary

#endif //__LIBMESH_QUADRATURE_H
