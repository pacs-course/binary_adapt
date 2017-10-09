#ifndef __LIBMESH_QUADRATURE_H
#define __LIBMESH_QUADRATURE_H


#include "Quadrature.h" //ElementType
#include "LibMeshHelper.h" //ConvertType
#include "Maps.h" //TriMap

#include <quadrature.h> //libMesh::QBase
#include "getpot.h"

namespace LibmeshBinary
{
	template <size_t dim, Geometry::ElementType Type>
		class LibmeshQuadratureRule : public Geometry::QuadratureRuleInterface<dim>
		{
			public:
				LibmeshQuadratureRule()
				{
					std::string thisfile = __FILE__;
					std::string conf_file = thisfile.substr(0, thisfile.find_last_of('/')) + "/mesh_quadrature.conf";
#ifdef DEBUG
					std::cerr << "LibmeshQuadratureRule: opening configuration file " << conf_file << std::endl;
#endif //DEBUG
					GetPot cl(conf_file.c_str());
					std::string order = "order";

					this->_order = cl(order.c_str(), 0);
					if (!this->_order)
						throw std::runtime_error("Unable to read the configuration file in LibmeshQuadratureRule");

					libMesh::Order mesh_order = static_cast<libMesh::Order>(this->_order);
					if (this->_order > 43)
					{
						mesh_order = libMesh::FORTYTHIRD;
						std::cerr << "Trying to construct a libMesh quadrature with order " << this->_order << std::endl;
						std::cerr << "The maximum order available is 43" << std::endl;
						std::cerr << "A quadrature rule of order 43 will be constructed" << std::endl;
					}
					_quadrature_rule = libMesh::QBase::build(libMesh::QGAUSS, dim, mesh_order);
					_quadrature_rule->init(ConvertType<Type>());
				};

				virtual ~LibmeshQuadratureRule(){};

				virtual Geometry::QuadPointVec<dim> GetPoints() override
				{
					std::vector<libMesh::Point> std_points = _quadrature_rule->get_points();
					size_t length = std_points.size();
					Geometry::QuadPointVec<dim> result(length);
					
					for (size_t i = 0; i < length; ++i)
					{
						auto p = ConvertPoint<dim>(std_points[i]);
						result.Insert(i,p);
					}

					return result;
				};

				virtual Geometry::QuadWeightVec GetWeights() override
				{
					std::vector<libMesh::Real> std_weights = _quadrature_rule->get_weights();
					Geometry::QuadWeightVec result(std_weights.size());
					size_t i(0);
					for (auto w : std_weights)
						result[i++] = static_cast<double>(w);

					return result;
				};

			private:
				libMesh::UniquePtr<libMesh::QBase> _quadrature_rule;
		};

/*
	It may happen that the library gives quadrature nodes on different standard geometry
	For triangles nodes are given in
	co{(0,0),(1,0),(0,1)},
	while my std element (on which the basis is orthogonal) is
	co{(-1,-1),(1,-1),(-1,1)})
*/
	class ModifiedTriangleRule : public LibmeshQuadratureRule<2, Geometry::TriangleType>
	{
		public:
			ModifiedTriangleRule();
			virtual Geometry::QuadPointVec<2> GetPoints() override;
			virtual Geometry::QuadWeightVec GetWeights() override;

		private:
			Geometry::TriMap _std_map;
	};


} //namespace LibmeshBinary

#endif //__LIBMESH_QUADRATURE_H
