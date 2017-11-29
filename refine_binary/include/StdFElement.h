#ifndef __STD_F_ELEMENT_H
#define __STD_F_ELEMENT_H

#include "StdFElementInterface.h"
#include "BinaryTreeHelper.h"
#include "ConcreteFactories.h"


namespace FiniteElements
{
	using namespace std;

/**
	Class for standard finite element with ipercube geometry.
	It is the element that effectively stores the basis.
	To satisfy the AbstractElement interface it stores a standard geometry.
	It is meant to be a singleton, std elements factory guarantees that no more than one copy is constructed.
	ComputeNormSquared() method can be optimized by partial specialization of FeType template parameter;
	since partial specialization of single methods is not allowed, in AlmostStdFIperCube class it is left
	pure virtual, in order to avoid the specialization of the whole class.
	It will be defined in proper StdFIperCube derived class.
**/
	template <size_t dim, BasisType FeType = InvalidFeType>
		class AlmostStdFIperCube : public StdFElementInterface<dim, FeType>
		{
			public:
/**
				constructor.
				It calls the basis factory to initialize _basis attribute.
**/
				AlmostStdFIperCube();

			public:
/**
				default destructor
**/
				virtual ~AlmostStdFIperCube();

/**
				Get the basis type identifier
**/
				virtual BasisType GetFeType() const;


/**
				Evaluate _basis function.
**/
				virtual double EvaluateBasisFunction (size_t ind, const Geometry::Point<dim>&) const;

/**
				Evaluate _basis functions untill certain degree.
**/
				virtual Geometry::Vector EvaluateBasis (size_t degree, const Geometry::Point<dim>&) const;

/**
				Number of _basis functions corresponding to input degree
**/
				virtual size_t BasisSize(size_t) const;

/**
				The geometric type identifier
**/
				virtual Geometry::ElementType GetType() const;

/**
				The quadrature points of standard ipercube
**/
				virtual Geometry::QuadPointVec<dim> GetQuadPoints() const;

/**
				The quadrature weights of standard ipercube
**/
				virtual Geometry::QuadWeightVec GetQuadWeights() const;

/**
				The quadrature order of exactness of standard ipercube
**/
				virtual size_t QuadratureOrder() const;

			protected:
/**
				The basis functions.
				In multiple dimensionality they are obtained by tensorization of 1D counterparts.
**/
				unique_ptr<TensorialBasis<dim>> _basis;
/**
				The standard ipercube.
**/
				shared_ptr<Geometry::StdIperCube<dim> > _std_geometry;
		};

/**
		Concrete class implementing the standard ipercube.
		It implements methods of AlmostStdFIperCube class which can take advantage
		of a partial specialization of the template FeType.
		This pattern is needed since standard c++ forbids partial specialization of template class methods.
**/
	template <size_t dim, BasisType FeType = InvalidFeType>
		class StdFIperCube : public AlmostStdFIperCube<dim, FeType>
		{
			public:
/**
				default constructor
**/
				StdFIperCube();

/**
				default destructor
**/
				virtual ~StdFIperCube();

			protected:
/**
				In a general setting the norm is the L^2 norm.
**/
				virtual double ComputeNormSquared(size_t) override;
		};

/**
		Partial specialization of standard ipercube for Legendre basis.
		In case of Legendre basis I know a priori the value of the basis norm,
		so I don't need to do the explicit integration.
**/
	template <size_t dim>
		class StdFIperCube<dim, LegendreType> : public AlmostStdFIperCube<dim, LegendreType>
		{
			public:
/**
				default constructor
**/
				StdFIperCube();

/**
				default destructor
**/
				virtual ~StdFIperCube();

			protected:
/**
				L^2 norm depends only on function index.
				Explicit L^2 integral is not needed.
				See:
					Quarteroni, "Numerical Models for Differential Problems", 2nd edition; p 229
				for details
**/
				virtual double ComputeNormSquared(size_t) override;
		};

/**
	Class for reference finite element with geometry different from ipercube.
	As for StdFIperCube class, AbstracElement interface is guaranteed storing
	a standard geometry object.
	Basis methods are defined in terms of the standard ipercube counterparts.
	So standard ipercube is stored, and map to pass from ipercube to standard geometry is provided.
**/
	template <size_t dim, Geometry::ElementType Type = Geometry::InvalidElementType, BasisType FeType = InvalidFeType>
		class StdFElement : public StdFElementInterface<dim, FeType>
		{
			public:
/**
				constructor.
				map from StdIperCube is given by StdMapFactory.
				standard geometries are stored as singleton. 
**/
				StdFElement();

			public:
/**
				default destructor
**/
				virtual ~StdFElement();

/**
				Get basis type identifier
**/
				virtual BasisType GetFeType() const;

/**
				Evaluation of basis function at input point.
				The evaluation of the i^th function at point p is defined as
				the value of the i^th basis function of the standard ipercube,
				computed at the point obtained mapping p to the ipercube,
				that is applying to p the inverse map of _ipercube_map.
**/
				virtual double EvaluateBasisFunction (size_t ind, const Geometry::Point<dim>& point) const;

/**
				As for EvaluateBasisFunction(), but it evaluates every basis function of degree not greater than input one.
**/
				virtual Geometry::Vector EvaluateBasis (size_t degree, const Geometry::Point<dim>& point) const;

/**
				The size of a basis with degree equal to input parameter
**/
				virtual size_t BasisSize(size_t) const;

/**
				The geometric type identifier of the standard geometry
**/
				virtual Geometry::ElementType GetType() const;

/**
				The quadrature nodes of the standard geometry
**/
				virtual Geometry::QuadPointVec<dim> GetQuadPoints() const;

/**
				The quadrature weights of the standard geometry
**/
				virtual Geometry::QuadWeightVec GetQuadWeights() const;

/**
				The quadrature order of underlying geometry quadrature rule
**/
				virtual size_t QuadratureOrder() const;

				/* Public methods to use the _ipercube_map attribute */


/**
				The inverse map of _ipercube_map
**/
				virtual Geometry::Point<dim> MapBackward(const Geometry::Point<dim>&)const;

/**
				Get the _ipercube_map evaluation
**/
				virtual Geometry::Point<dim> MapForward(const Geometry::Point<dim>&) const;

/**
				Get the _ipercube_map Jacobian evaluation
**/
				virtual double Jacobian(const Geometry::Point<dim>&) const;

			protected:
/**
				i-th basis function norm computation.
				In this general setting, it simply computes the L^2 norm of the required basis function
**/
				virtual double ComputeNormSquared(size_t) override;

/**
				Optimized basis functions norm computation.
**/
				virtual void UpdateNorms() override;

			protected:
/**
				Pointer to reference ipercube.
				Standard ipercube is stored as a singleton.
**/
				shared_ptr<StdFIperCube<dim, FeType>> _std_cube;

/**
				Pointer to standard geometry.
				Standard geometry is stored as a singleton.
				"Geometric" methods of StdFElement class simply recall
				the _std_geometry counterparts.
**/
				shared_ptr<Geometry::StdElement<dim, Type>> _std_geometry;
/**
				Map from reference ipercube to standard geometry
**/
				unique_ptr<Geometry::Map<dim>> _ipercube_map;
		};

	template <size_t dim, BasisType FeType>
		AlmostStdFIperCube<dim, FeType>::AlmostStdFIperCube()
		{
			_std_geometry = Helpers::Builders<Geometry::StdIperCube<dim>>::BuildSingleton ();

			auto& basis_factory (GenericFactory::TensorialBasisFactory<dim>::Instance());
			_basis = move(basis_factory.create(FeType));
		};

	template <size_t dim, BasisType FeType>
		AlmostStdFIperCube<dim, FeType>::~AlmostStdFIperCube()
		{};

	template <size_t dim, BasisType FeType>
		BasisType AlmostStdFIperCube<dim, FeType>::GetFeType()const
		{
			return FeType;
		};

	//TODO: optimize storing already evaluated points
	template <size_t dim, BasisType FeType>
		double AlmostStdFIperCube<dim, FeType>::EvaluateBasisFunction (size_t ind, const Geometry::Point<dim>& point) const
		{
			return this->_basis->Evaluate(ind, point);
		};

	template <size_t dim, BasisType FeType>
		Geometry::Vector AlmostStdFIperCube<dim, FeType>::EvaluateBasis (size_t degree, const Geometry::Point<dim>& point) const
		{
			return this->_basis->EvaluateBasis(degree, point);
		};

	template <size_t dim, BasisType FeType>
		size_t AlmostStdFIperCube<dim, FeType>::BasisSize(size_t degree) const
		{
			return this->_basis->ComputeSize(degree);
		};

	template <size_t dim, BasisType FeType>
		Geometry::ElementType AlmostStdFIperCube<dim, FeType>::GetType() const
		{
			return this->_std_geometry->GetType();
		};

	template <size_t dim, BasisType FeType>
		Geometry::QuadPointVec<dim> AlmostStdFIperCube<dim, FeType>::GetQuadPoints() const
		{
			return this->_std_geometry->GetQuadPoints();
		};

	template <size_t dim, BasisType FeType>
		Geometry::QuadWeightVec AlmostStdFIperCube<dim, FeType>::GetQuadWeights() const
		{
			return this->_std_geometry->GetQuadWeights();
		};

	template <size_t dim, BasisType FeType>
		size_t AlmostStdFIperCube<dim, FeType>::QuadratureOrder() const
		{
			return this->_std_geometry->QuadratureOrder();
		};

	template <size_t dim, BasisType FeType>
		StdFIperCube<dim, FeType>::StdFIperCube() : AlmostStdFIperCube<dim, FeType>()
		{};

	template <size_t dim, BasisType FeType>
		StdFIperCube<dim, FeType>::~StdFIperCube()
		{};

	template <size_t dim, BasisType FeType>
		double StdFIperCube<dim, FeType>::ComputeNormSquared(size_t ind)
		{
			return this->L2Prod	(
											[ind, this]
											(const Geometry::Point<dim>& p)
											{return this->EvaluateBasisFunction(ind, p);},
											[ind, this]
											(const Geometry::Point<dim>& p)
											{return this->EvaluateBasisFunction(ind, p);}
										);
		};

	template <size_t dim>
		StdFIperCube<dim, LegendreType>::StdFIperCube() : AlmostStdFIperCube<dim, LegendreType>()
		{};

	template <size_t dim>
		StdFIperCube<dim, LegendreType>::~StdFIperCube()
		{};

	template <size_t dim>
		double StdFIperCube<dim, LegendreType>::ComputeNormSquared(size_t ind)
		{
			auto indexes = this->_basis->GetIndexes(ind);
			double inverse_norm = 1;
			for (auto k : indexes)
				inverse_norm *= static_cast<double>(k) + 0.5;

			return 1 / inverse_norm;
		};

	template <size_t dim, Geometry::ElementType Type, BasisType FeType>
		StdFElement<dim, Type, FeType>::StdFElement()
		{
			_std_geometry = Helpers::Builders<Geometry::StdElement<dim, Type> >::BuildSingleton ();
			_std_cube = Helpers::Builders<StdFIperCube<dim, FeType> >::BuildSingleton ();

			auto& std_map_factory(GenericFactory::StdMapFactory<dim>::Instance());
			_ipercube_map = std_map_factory.create(Type);
		};

	template <size_t dim, Geometry::ElementType Type, BasisType FeType>
		StdFElement<dim, Type, FeType>::~StdFElement()
		{};

	template <size_t dim, Geometry::ElementType Type, BasisType FeType>
		BasisType StdFElement<dim, Type, FeType>::GetFeType() const
		{
			return FeType;
		};

	//TODO: optimize storing already evaluated points
	template <size_t dim, Geometry::ElementType Type, BasisType FeType>
		double StdFElement<dim, Type, FeType>::EvaluateBasisFunction (size_t ind, const Geometry::Point<dim>& point)const
		{
			return this->_std_cube->EvaluateBasisFunction(ind, MapBackward(point));
		};

	template <size_t dim, Geometry::ElementType Type, BasisType FeType>
		Geometry::Vector StdFElement<dim, Type, FeType>::EvaluateBasis (size_t degree, const Geometry::Point<dim>& point)const
		{
			return this->_std_cube->EvaluateBasis(degree, MapBackward(point));
		};

	template <size_t dim, Geometry::ElementType Type, BasisType FeType>
		size_t StdFElement<dim, Type, FeType>::BasisSize(size_t degree) const
		{
			return this->_std_cube->BasisSize(degree);
		};

	template <size_t dim, Geometry::ElementType Type, BasisType FeType>
		Geometry::ElementType StdFElement<dim, Type, FeType>::GetType() const
		{
			return this->_std_geometry->GetType();
		};

	template <size_t dim, Geometry::ElementType Type, BasisType FeType>
		Geometry::QuadPointVec<dim> StdFElement<dim, Type, FeType>::GetQuadPoints() const
		{
			return this->_std_geometry->GetQuadPoints();
		};

	template <size_t dim, Geometry::ElementType Type, BasisType FeType>
		Geometry::QuadWeightVec StdFElement<dim, Type, FeType>::GetQuadWeights() const
		{
			return this->_std_geometry->GetQuadWeights();
		};

	template <size_t dim, Geometry::ElementType Type, BasisType FeType>
		size_t StdFElement<dim, Type, FeType>::QuadratureOrder() const
		{
			return this->_std_geometry->QuadratureOrder();
		};

	template <size_t dim, Geometry::ElementType Type, BasisType FeType>
		Geometry::Point<dim> StdFElement<dim, Type, FeType>::MapBackward(const Geometry::Point<dim>& p) const
		{
			return this->_ipercube_map->ComputeInverse(p);
		};

	template <size_t dim, Geometry::ElementType Type, BasisType FeType>
		Geometry::Point<dim> StdFElement<dim, Type, FeType>::MapForward(const Geometry::Point<dim>& p) const
		{
			return this->_ipercube_map->Evaluate(p);
		};

	template <size_t dim, Geometry::ElementType Type, BasisType FeType>
		double StdFElement<dim, Type, FeType>::Jacobian(const Geometry::Point<dim>& p) const
		{
			return this->_ipercube_map->EvaluateJacobian(p);
		};

	template <size_t dim, Geometry::ElementType Type, BasisType FeType>
		double StdFElement<dim, Type, FeType>::ComputeNormSquared(size_t ind)
		{
			return this->L2Prod	(
											[ind, this]
											(const Geometry::Point<dim>& p)
											{return this->EvaluateBasisFunction(ind, p);},
											[ind, this]
											(const Geometry::Point<dim>& p)
											{return this->EvaluateBasisFunction(ind, p);}
										);
		};

	template <size_t dim, Geometry::ElementType Type, BasisType FeType>
		void StdFElement<dim, Type, FeType>::UpdateNorms()
		{
			size_t l = (this->_norm_values).Size();
			size_t new_l = this->BasisSize(this->_max_degree);

			if (l < new_l)
			{
#ifdef VERBOSE
				clog << "Resizing the basis norm values vector" << endl;
#endif //VERBOSE
				size_t degree = 0;
				while (BasisSize(degree) < new_l)
					++degree;

				this->_norm_values = this->MultiIntegrate	(
																			[degree, this]
																			(const Geometry::Point<dim>& p)
																			{
																				auto vals = this->EvaluateBasis(degree, p);
																				return vals.CWiseProduct(vals);
																			}
																		);
			}
		};

} //namespace FiniteElements

#endif //__STD_F_ELEMENT_H
