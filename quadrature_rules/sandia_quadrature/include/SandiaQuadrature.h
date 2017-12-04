#ifndef __SANDIA_QUADRATURE_H
#define __SANDIA_QUADRATURE_H

#include "sandia_rules.hpp" //jacobi_compute

#include "Quadrature.h" //ElementType

#ifdef LIBMESH_BUG_FIXED
#include <GetPot>
#else
#include "libmesh/getpot.h"
#endif

/**
	Quadrature rule implementation based on sandia_rules library
**/
namespace SandiaQuadrature
{
	using namespace std;

/**
	Compute the tensor product of each possible @p dim sized tuple of points of the vector.
	The computation is done recursively on template parameter @p dim
**/
	template <size_t dim>
		Geometry::QuadPointVec<dim> TensorizePoints (const Geometry::QuadPointVec<1>&);

/**
	Compute the term by term product of each possible @p dim sized tuple of points of the vector.
	The computation is done recursively on template parameter @p dim
**/
	template <size_t dim>
		Geometry::QuadWeightVec TensorizeWeights (const Geometry::QuadWeightVec&);


//TODO: implement other possible nodes other than gaussian
/**
	Quadrature rule class for the ipercube of @p dim dimensionality.
	Nodes are Gaussian.
	Nodes and weights computation is based on external library 
	<a href="https://people.sc.fsu.edu/~jburkardt/cpp_src/sandia_rules/sandia_rules.html">link text</a>.
**/
	template <size_t dim>
		class SandiaQuadratureRule : public Geometry::QuadratureRuleInterface<dim>
		{
			public:
/**
				constructor.
				It gets the quadrature parameters (i.e. the order) from 
				sandia_quadrature/include/sandia_quadrature.conf configuration file.
				The it computes nodes and weights through Init() method.
**/
				SandiaQuadratureRule();
/**
				constructor with needed exactness order
**/
				SandiaQuadratureRule(size_t);
/**
				default destructor
**/
				virtual ~SandiaQuadratureRule();

/**
				Get quadrature nodes
**/
				virtual Geometry::QuadPointVec<dim> GetPoints() override;
/**
				Get quadrature weigths
**/
				virtual Geometry::QuadWeightVec GetWeights() override;

			protected:
/**
				Compute nodes and weights
**/
				virtual void Init();

/**
				Compute the number of nodes needed to get input order
**/
				virtual size_t NodesNumber(size_t);

			private:
/**
				The number of nodes
**/
				size_t _n;
/**
				The vector of quadrature nodes
**/
				Geometry::QuadPointVec<dim> _points;
/**
				The vector of quadrature weights
**/
				Geometry::QuadWeightVec _weights;
		};

	template <size_t dim>
		SandiaQuadratureRule<dim>::SandiaQuadratureRule()
		{
			string thisfile = __FILE__;
			string conf_file = thisfile.substr(0, thisfile.find_last_of('/')) + "/sandia_quadrature.conf";
#ifdef DEBUG
			cerr << "SandiaQuadratureRule: opening configuration file " << conf_file << endl;
#endif //DEBUG
			GetPot cl(conf_file.c_str());
			string order = "order";

			this->_order = cl(order.c_str(), 0);
			if (!this->_order)
				throw runtime_error("Unable to read the configuration file in SandiaQuadratureRule");

			Init();
		};

	template <size_t dim>
		SandiaQuadratureRule<dim>::SandiaQuadratureRule(size_t order)
		{
			this->_order = order;
			Init();
		};

	template <size_t dim>
		SandiaQuadratureRule<dim>::~SandiaQuadratureRule()
		{};

	template <size_t dim>
		Geometry::QuadPointVec<dim> SandiaQuadratureRule<dim>::GetPoints()
		{
			return this->_points;
		};

	template <size_t dim>
		Geometry::QuadWeightVec SandiaQuadratureRule<dim>::GetWeights()
		{
			return this->_weights;
		};

	template <size_t dim>
		void SandiaQuadratureRule<dim>::Init()
		{
			_n = NodesNumber(this->_order);

			double x[_n];
			double w[_n];
			webbur::jacobi_compute (_n, 0.0, 0.0, x, w);

			//I convert x and w to my type
			Geometry::QuadPointVec<1> one_d_points(this->_n);
			Geometry::QuadWeightVec one_d_weights(this->_n);

			for (size_t i = 0; i < this->_n; ++i)
			{
				one_d_points.Insert(i, x[i]);
				one_d_weights[i] = w[i];
			}

			//I tensorize to get a dim-dimensional quadrature rule
			_points = TensorizePoints<dim>(one_d_points);
			_weights = TensorizeWeights<dim>(one_d_weights);
		};

	template <size_t dim>
		size_t SandiaQuadratureRule<dim>::NodesNumber(size_t order)
		{
			return ceil((static_cast<double>(order) + 1)/2);
		};

	template <size_t dim>
		Geometry::QuadPointVec<dim> TensorizePoints (const Geometry::QuadPointVec<1>& one_d_p)
		{
			Geometry::QuadPointVec<dim-1> temp = TensorizePoints<dim-1> (one_d_p);

			size_t l1 = temp.Size();
			size_t l2 = one_d_p.Size();
			Geometry::QuadPointVec<dim> result(l1*l2);

			size_t cont = 0;
			for (size_t i = 0; i < l1; ++i)
				for(size_t j = 0; j < l2; ++j)
				{
					auto p = temp[i].Tensor(one_d_p[j]);
					result.Insert(cont++, p);
				}
			return result;
		};

/*	End of the recursion	*/
	template <>
		Geometry::QuadPointVec<1> TensorizePoints<1> (const Geometry::QuadPointVec<1>&);

	template <size_t dim>
		Geometry::QuadWeightVec TensorizeWeights (const Geometry::QuadWeightVec& one_d_w)
		{
			Geometry::QuadWeightVec temp = TensorizeWeights<dim-1>(one_d_w);
			auto length1 = temp.Size();
			auto length2 = one_d_w.Size();

			Geometry::QuadWeightVec result(length1*length2);

			auto k = 0;
			for (size_t i = 0; i < length1; ++i)
				for(size_t j = 0; j < length2; ++j)
					result[k++] = temp[i]*one_d_w[j];
			return result;
		};

/*	End of recursion	*/
	template <>
		Geometry::QuadWeightVec TensorizeWeights<1> (const Geometry::QuadWeightVec& one_d_w);

} //namespace SandiaQuadrature

#endif //__SANDIA_QUADRATURE_H
