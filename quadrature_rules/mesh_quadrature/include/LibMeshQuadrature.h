#ifndef __LIBMESH_QUADRATURE_H
#define __LIBMESH_QUADRATURE_H


#include "Quadrature.h" //ElementType
#include "LibMeshHelper.h" //ConvertType
#include "Maps.h" //TriMap

#include "libmesh/quadrature.h" //libMesh::QBase
#include "libmesh/getpot.h"

namespace LibmeshBinary
{
	//TODO: implement other possible nodes other than gaussian

	/**
		Quadrature rule class for #Geometry::ElementType geometry.
		Nodes are Gaussian.
		Nodes and weights computation is based on external library
		<a href="https://libmesh.github.io/doxygen/classlibMesh_1_1QGauss.html">link text</a>.
	**/
	template <size_t dim, Geometry::ElementType Type>
	class LibmeshQuadratureRule : public Geometry::QuadratureRuleInterface<dim>
	{
	  public:
		/**
			constructor.
			It gets the quadrature parameters (i.e. the order) from
			mesh_quadrature/include/mesh_quadrature.conf configuration file.
			Then it initialize the libMesh rule.
		**/
		LibmeshQuadratureRule();
		/**
			default destructor
		**/
		virtual ~LibmeshQuadratureRule();

		/**
			Get quadrature nodes converting libMesh nodes of #_quadrature_rule
		**/
		virtual Geometry::QuadPointVec<dim> GetPoints() override;

		/**
			Get quadrature weights converting libMesh weights of #_quadrature_rule
		**/
		virtual Geometry::QuadWeightVec GetWeights() override;

	  private:
		libMesh::UniquePtr<libMesh::QBase> _quadrature_rule;
	};

	/**
		Specialization for triangular case.
		It may happen that the library gives quadrature nodes on different standard geometry
		For triangles nodes are given in
		co{(0,0),(1,0),(0,1)},
		while the standard element (on which the WarpedBasis is orthogonal) is
		co{(-1,-1),(1,-1),(-1,1)})
	**/
	class ModifiedTriangleRule :
		public LibmeshQuadratureRule<2, Geometry::TriangleType>
	{
	  public:
		/**
			constructor
		**/
		ModifiedTriangleRule();
		/**
			default destructor
		**/
		~ModifiedTriangleRule();

		virtual Geometry::QuadPointVec<2> GetPoints() override;
		virtual Geometry::QuadWeightVec	  GetWeights() override;

	  private:
		/**
			Map from standard element to libMesh reference element.
		**/
		Geometry::TriMap _std_map;
	};

	template <size_t dim, Geometry::ElementType Type>
	LibmeshQuadratureRule<dim, Type>::LibmeshQuadratureRule()
	{
		std::string thisfile = __FILE__;
		std::string conf_file = thisfile.substr (0, thisfile.find_last_of ('/'))
							  + "/mesh_quadrature.conf";
#ifdef DEBUG
		std::cerr << "LibmeshQuadratureRule: opening configuration file "
				  << conf_file
				  << std::endl;
#endif //DEBUG

		GetPot cl (conf_file.c_str());
		std::string order = "order";

		this->_order = cl (order.c_str(), 0);
		if (!this->_order)
			throw std::runtime_error (
				"Unable to read the configuration file in LibmeshQuadratureRule");

		libMesh::Order mesh_order = static_cast<libMesh::Order> (this->_order);

		if (this->_order > 43)
		{
			mesh_order = libMesh::FORTYTHIRD;
			std::cerr << "Trying to construct a libMesh quadrature with order "
					  << this->_order
					  << std::endl;
			std::cerr << "The maximum order available is 43"
					  << std::endl;
			std::cerr << "A quadrature rule of order 43 will be constructed"
					  << std::endl;
		}

		_quadrature_rule = libMesh::QBase::build (libMesh::QGAUSS, dim, mesh_order);
		_quadrature_rule->init (ConvertType<Type>());
	};

	template <size_t dim, Geometry::ElementType Type>
	LibmeshQuadratureRule<dim, Type>::~LibmeshQuadratureRule()
	{};

	template <size_t dim, Geometry::ElementType Type>
	Geometry::QuadPointVec<dim> LibmeshQuadratureRule<dim, Type>::GetPoints()
	{
		std::vector<libMesh::Point> std_points = _quadrature_rule->get_points();
		size_t length = std_points.size();
		Geometry::QuadPointVec<dim> result (length);

		for (size_t i = 0; i < length; ++i)
		{
			auto p = ConvertPoint<dim> (std_points[i]);
			result.Insert (i, p);
		}

		return result;
	};

	template <size_t dim, Geometry::ElementType Type>
	Geometry::QuadWeightVec LibmeshQuadratureRule<dim, Type>::GetWeights()
	{
		std::vector<libMesh::Real> std_weights = _quadrature_rule->get_weights();
		Geometry::QuadWeightVec result (std_weights.size());
		size_t i (0);
		for (auto w : std_weights)
			result[i++] = static_cast<double> (w);

		return result;
	};

} //namespace LibmeshBinary

#endif //__LIBMESH_QUADRATURE_H
