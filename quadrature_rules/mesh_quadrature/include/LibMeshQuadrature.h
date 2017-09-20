#ifndef __LIBMESH_QUADRATURE_H
#define __LIBMESH_QUADRATURE_H

#include <quadrature.h> //libMesh::QBase

#include "Quadrature.h" //ElementType

#include "LibMeshHelper.h" //ConvertPoint, ConvertType

#include "getpot.h"

namespace LibmeshBinary
{
	template <size_t dim, Geometry::ElementType Type>
		class LibmeshQuadratureRule : public Geometry::QuadratureRuleInterface<dim>
		{
			public:
				LibmeshQuadratureRule()
				{
					string thisfile = __FILE__;
					string conf_file = thisfile.substr(0, thisfile.find_last_of('/')) + "/mesh_quadrature.conf";

					GetPot cl(conf_file.c_str());
					string order = to_string(dim) + "D/order";

					this->_order = cl(order.c_str(), 0);
					if (!this->_order)
						throw runtime_error("Unable to read the configuration file in LibmeshQuadratureRule");

					libMesh::Order mesh_order = static_cast<libMesh::Order>(this->_order);
					if (this->_order > 43)
					{
						mesh_order = libMesh::FORTYTHIRD;
						cerr << "Trying to construct a libMesh quadrature with order " << this->_order << endl;
						cerr << "The maximum order available is 43" << endl;
						cerr << "A quadrature rule of order 43 will be constructed" << endl;
					}
					_quadrature_rule = libMesh::QBase::build(libMesh::QGAUSS, dim, mesh_order);
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

			private:
				libMesh::UniquePtr<libMesh::QBase> _quadrature_rule;
		};

} //namespace LibmeshBinary

#endif //__LIBMESH_QUADRATURE_H
