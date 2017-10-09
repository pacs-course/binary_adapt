#ifndef __ABSTRACT_F_ELEMENT_H
#define __ABSTRACT_F_ELEMENT_H

#include "ConcreteFactories.h"
#include "AbstractSpace.h"

#include <memory> //std::shared_ptr, std::unique_ptr
#include <utility> //std::move


namespace FiniteElements
{
	using namespace std;

	template <size_t dim, BasisType FeType = InvalidFeType>
		class	AbstractFElement : public Geometry::AbstractElement<dim>, public AbstractSpace_Interface<dim>
		{
			public:
/*
				_map and _ref_felement initializations depend on the pure virtual method GetType() return, so it cannot be done during object construction; this operation is done by Init() method, whose call is compulsory before using the object
*/
				AbstractFElement() : _p_level(0), _initialized(false), _ref_felement(nullptr){};

				virtual ~AbstractFElement()
				{
#ifdef DESTRUCTOR_ALERT
				clog << "Distruggo AbstractFElement" << endl;
#endif //DESTRUCTOR_ALERT
				};

				virtual void Init()
				{
					if(!(this->_initialized))
					{
#ifdef MYDEBUG
						clog << "Provo a inizializzare AbstractFElement" << endl;
#endif //MYDEBUG
						//TODO: create helper function to combine factory instantiation and call to the object creation
						auto& felem_factory(GenericFactory::StdFElementFactory <dim, FeType> ::Instance());
#ifdef MYDEBUG
						clog << "cerco in StdFElementFactory<" << dim << ", " << FeType <<"> con chiave " << this->GetType() << endl;
						clog << "indirizzo factory: " << &felem_factory << endl;
#endif //MYDEBUG
						this->_ref_felement = felem_factory.create(this->GetType());

#ifdef MYDEBUG
						clog << "refFElement creato" << endl;
#endif //MYDEBUG
						auto& map_factory(GenericFactory::AffineMapFactory<dim>::Instance());
						this->_map = move(map_factory.create(this->GetType()));
#ifdef MYDEBUG
						clog << "mappa creata" << endl;
#endif //MYDEBUG
						this->_map -> Init(GetNodes());
						this->_initialized = true;
#ifdef MYDEBUG
						clog << "AbstractFElement inizializzato" << endl;
#endif //MYDEBUG
					}
				};


				virtual BasisType GetFeType()const override
				{
					return FeType;
				};

/*
				methods depending on geometry, cannot be defined at this level
*/
				virtual Geometry::NodesVector<dim> GetNodes() = 0;

				virtual Geometry::ElementType GetType()const = 0;

				//TODO: verify if it's convenient to store the quadrature nodes and weights as an attribute of the element
				virtual Geometry::QuadPointVec<dim> GetQuadPoints()const
				{
					CheckInitialization();

					auto points = _ref_felement->GetQuadPoints();

					return _map->Evaluate(points);
				};

				virtual Geometry::QuadWeightVec GetQuadWeights()const
				{
					CheckInitialization();

					Geometry::QuadWeightVec weights = _ref_felement->GetQuadWeights();
/*
					I'm taking advantage from the fact that _map is affine, so it has an evaluateJacobian() method
					not dependent on the point of evaluation
*/
					for (size_t i = 0; i < weights.Size(); ++i)
						weights[i] *= _map->Jacobian();

					return weights;
				};

				virtual size_t QuadratureOrder()const
				{
					CheckInitialization();
					return _ref_felement->QuadratureOrder();
				};

				//TODO: optimize storing already evaluated points
				virtual double EvaluateBasisFunction (size_t ind, const Geometry::Point<dim>& point)const
				{
					CheckInitialization();
					return 	_ref_felement->EvaluateBasisFunction(ind, _map->ComputeInverse(point))
								/								//to normalize the basis I divide by the constant jacobian^1/2 of the map
								sqrt(_map->Jacobian())
								;
				};

				//TODO: optimize storing already evaluated points
				virtual vector<double> EvaluateBasis (const Geometry::Point<dim>& point)const
				{
					CheckInitialization();
					auto result = this->_ref_felement->EvaluateBasis(this->_p_level,_map->ComputeInverse(point));

					/* to normalize the basis I divide by the constant jacobian^1/2 of the map */
					for (auto& iter : result)
						iter /= sqrt(_map->Jacobian());

					return result;
				};

				virtual size_t	BasisSize(size_t degree)const
				{
					CheckInitialization();
					return _ref_felement->BasisSize(degree);
				};

				virtual size_t BasisSize()const
				{
					CheckInitialization();
					return _ref_felement->BasisSize(this->_p_level);
				};

				virtual size_t PLevel() const
				{
					return this->_p_level;
				};
				virtual void PLevel(size_t new_p)
				{
					this->_p_level = new_p;
				};

			protected:

				Geometry::AffineMap<dim>& UseMap()const
				{
					CheckInitialization();
					return *(this->_map);
				};

				virtual void CheckInitialization()const
				{
					if (!_initialized)
						throw runtime_error("Trying to use uninitialized element");
				};

			protected:
/*
				It is how much is incremented the underlying basis degree w.r.t the default basis degree (which is 0 for Legendre, that is approximation by a constant)
				By default is 0; 
*/
				size_t _p_level;
/*
				A flag which controls if the init method has been called
*/
				bool _initialized;
/*
				The reference standard element associated to the type of the object
				It is meant to be a singleton, so it's stored as a shared_ptr
*/
				shared_ptr<StdBananaFElement<dim, FeType>> _ref_felement;

			private:
/*
				The affine map through which I can pass from the reference element to the current domain of the object
*/
				unique_ptr<Geometry::AffineMap<dim>>	_map;
		};

};//namespace FiniteElements

#endif //__ABSTRACT_F_ELEMENT_H
