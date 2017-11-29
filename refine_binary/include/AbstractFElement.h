#ifndef __ABSTRACT_F_ELEMENT_H
#define __ABSTRACT_F_ELEMENT_H

#include "ConcreteFactories.h"
#include "AbstractSpace.h"

#include <memory> //std::shared_ptr, std::unique_ptr
#include <utility> //std::move


namespace FiniteElements
{
	using namespace std;

/**
	Abstract class for a generic FElement.
	An FElement is intended as a geometric object
	(so is possible to compute integrals on it)
	with a basis defined on it.
	So it provides methods to compute integrals
	and to evaluate basis functions.
	This operations are made mapping the result
	of the reference element corresponding operation.
	Information on the geometric type are still missing,
	so the class is virtual.
**/
	template <size_t dim, BasisType FeType = InvalidFeType>
		class	AbstractFElement : public Geometry::AbstractElement<dim>, public AbstractSpaceInterface<dim>
		{
			public:
/**
				constructor.
				_map and _ref_felement initializations depend on the pure virtual method GetType() return,
				so they cannot be initialized during object construction;
				this operation is done by Init() method, whose call is compulsory before using the object
**/
				AbstractFElement();

/**
				default destructor
**/
				virtual ~AbstractFElement();

/**
				Initialize the element.
				It must be called after the constructor.
				It accesses std elements factory and maps factory to initialize attributes.
				If a class method is called before the element has been initialized,
				an exception is raised.
**/
				virtual void Init();

/**
				Get the basis type identifier
**/
				virtual BasisType GetFeType() const override;
/**
				Get the element vertices.
				Method which depend on the geometry, cannot be defined at this level.
**/
				virtual Geometry::NodesVector<dim> GetNodes() = 0;

/**
				Get the geometry type identifier.
				Method which depend on the geometry, cannot be defined at this level.
**/
				virtual Geometry::ElementType GetType() const = 0;

/**
				Get quadrature nodes.
				Quadrature nodes are computed mapping the reference element quadrature nodes.
**/
				virtual Geometry::QuadPointVec<dim> GetQuadPoints() const;

/**
				Get quadrature weights.
				Quadrature weights are computed multiplying the reference element quadrature nodes times the map jacobian.
**/
				virtual Geometry::QuadWeightVec GetQuadWeights() const;


/**
				Get quadrature order of exactness
**/
				virtual size_t QuadratureOrder() const;

/**
				Evaluate a basis function.
				Input parameters:
					- index of the basis function to be evaluated;
					- point where the basis function has to be evaluated;

				The evaluation is done computing the correspondent reference element basis function
				at inverse mapped input point. 
**/
				virtual double EvaluateBasisFunction (size_t, const Geometry::Point<dim>&) const;

/**
				Evaluate the whole basis at input point.
				The evaluation is done computing the reference element basis
				at inverse mapped input point. 
**/
				virtual Geometry::Vector EvaluateBasis (const Geometry::Point<dim>&) const;

/**
				Get the number of functions of a basis with degree equal to the input parameter.
**/
				virtual size_t	BasisSize(size_t) const;

/**
				Get the number of functions of the element basis.
**/
				virtual size_t BasisSize() const;

/**
				Get the element p level
**/
				virtual size_t PLevel() const;
/**
				Set the element p level
**/
				virtual void PLevel(size_t);

			public:
/**
				Norm of the basis function with input index.
				It returns the norm of the correspondent basis function of the reference element,
				rescaled by the constant jacobian of the affine map
**/
				virtual double BasisNormSquared(size_t) const;

			protected:
/**
				Get the map.
				Map returned by reference since it is a unique_ptr
**/
				const Geometry::AffineMap<dim>& UseMap() const;

/**
				Raise an exception if the element has not been initialized.
				Method to be called by every other method of the class before trying to use class attributes.
				It does not fail if the Init() method has been called before.
**/
				virtual void CheckInitialization() const;

			protected:
/**
				p refinement level.
				It is how much is incremented the underlying basis degree w.r.t the default basis degree
				(which is 0 for Legendre, that is approximation by a constant).
				By default p level is 0.
**/
				size_t _p_level;
/**
				A flag which controls if the init method has been called
**/
				bool _initialized;
/**
				The reference standard element associated to the type of the object.
				It is meant to be a singleton, so it's stored as a shared_ptr.
**/
				shared_ptr<StdFElementInterface<dim, FeType>> _ref_felement;

			private:
/**
				The affine map through which I can pass from the reference element to the current domain of the object
**/
				unique_ptr<Geometry::AffineMap<dim>> _map;
		};


	template <size_t dim, BasisType FeType>
		AbstractFElement<dim, FeType>::AbstractFElement() : _p_level(0), _initialized(false), _ref_felement(nullptr)
		{};

	template <size_t dim, BasisType FeType>
		AbstractFElement<dim, FeType>::~AbstractFElement()
		{};

	template <size_t dim, BasisType FeType>
		void AbstractFElement<dim, FeType>::Init()
		{
			if(!(this->_initialized))
			{
				//TODO: create helper function to combine factory instantiation and call to the object creation
				auto& felem_factory(GenericFactory::StdFElementFactory <dim, FeType> ::Instance());

				this->_ref_felement = felem_factory.create(this->GetType());

				auto& map_factory(GenericFactory::AffineMapFactory<dim>::Instance());
				this->_map = move(map_factory.create(this->GetType()));

				this->_map -> Init(GetNodes());
				this->_initialized = true;
			}
		};

	template <size_t dim, BasisType FeType>
		BasisType AbstractFElement<dim, FeType>::GetFeType() const
		{
			return FeType;
		};

	//TODO: verify if it's convenient to store the quadrature nodes and weights as an attribute of the element
	template <size_t dim, BasisType FeType>
		Geometry::QuadPointVec<dim> AbstractFElement<dim, FeType>::GetQuadPoints() const
		{
			CheckInitialization();

			auto points = _ref_felement->GetQuadPoints();

			return _map->Evaluate(points);
		};

	template <size_t dim, BasisType FeType>
		Geometry::QuadWeightVec AbstractFElement<dim, FeType>::GetQuadWeights() const
		{
			CheckInitialization();

			Geometry::QuadWeightVec weights = _ref_felement->GetQuadWeights();
/*
			I'm taking advantage from the fact that _map is affine, so it has an evaluateJacobian() method
			not dependent on the point of evaluation
*/
			auto jac = _map->Jacobian();
			for (size_t i = 0; i < weights.Size(); ++i)
				weights[i] *= jac;

			return weights;
		};

	template <size_t dim, BasisType FeType>
		size_t AbstractFElement<dim, FeType>::QuadratureOrder() const
		{
			CheckInitialization();
			return _ref_felement->QuadratureOrder();
		};

	//TODO: optimize storing already evaluated points
	template <size_t dim, BasisType FeType>
		double AbstractFElement<dim, FeType>::EvaluateBasisFunction (size_t ind, const Geometry::Point<dim>& point) const
		{
			CheckInitialization();
			return _ref_felement->EvaluateBasisFunction(ind, _map->ComputeInverse(point));
		};

	//TODO: optimize storing already evaluated points
	template <size_t dim, BasisType FeType>
		Geometry::Vector AbstractFElement<dim, FeType>::EvaluateBasis (const Geometry::Point<dim>& point) const
		{
			CheckInitialization();
			return this->_ref_felement->EvaluateBasis(this->_p_level, _map->ComputeInverse(point));
		};

	template <size_t dim, BasisType FeType>
		size_t AbstractFElement<dim, FeType>::BasisSize(size_t degree) const
		{
			CheckInitialization();
			return _ref_felement->BasisSize(degree);
		};

	template <size_t dim, BasisType FeType>
		size_t AbstractFElement<dim, FeType>::BasisSize() const
		{
			CheckInitialization();
			return _ref_felement->BasisSize(this->_p_level);
		};

	template <size_t dim, BasisType FeType>
		size_t AbstractFElement<dim, FeType>::PLevel() const
		{
			return this->_p_level;
		};

	template <size_t dim, BasisType FeType>
		void AbstractFElement<dim, FeType>::PLevel(size_t new_p)
		{
			this->_p_level = new_p;
		};

	template <size_t dim, BasisType FeType>
		double AbstractFElement<dim, FeType>::BasisNormSquared(size_t ind) const
		{
			double std_norm = this->_ref_felement->BasisNormSquared(ind);
			double J = this->_map->Jacobian();
			return std_norm * J;
		};

	template <size_t dim, BasisType FeType>
		const Geometry::AffineMap<dim>& AbstractFElement<dim, FeType>::UseMap() const
		{
			CheckInitialization();
			return *(this->_map);
		};

	template <size_t dim, BasisType FeType>
		void AbstractFElement<dim, FeType>::CheckInitialization() const
		{
			if (!_initialized)
				throw runtime_error("Trying to use uninitialized element");
		};

};//namespace FiniteElements

#endif //__ABSTRACT_F_ELEMENT_H
