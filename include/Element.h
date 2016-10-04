#ifndef __ELEMENT_H
#define __ELEMENT_H

#include "Maps.h"
#include "ReferenceElement.h"

#include <memory> //shared_ptr

using namespace std;
using namespace Maps;
using namespace Geometry;

namespace BinaryTree
{	
	template <size_t DIM>
		using StdElementFactory	= GenericFactory::SingletonFactory	<StdBananaElement<DIM>	, elementType	>;

	template <size_t DIM>
		using StdSpaceFactory	= GenericFactory::SingletonFactory	<StdBananaSpace<DIM>		, Basis::basisType>;

	template <size_t DIM>
		using AffineMapFactory	= GenericFactory::ObjectFactory		<AffineMap<DIM>			, elementType		>;

	template <size_t DIM>
		class Element : public AbstractElement<DIM>
		{
			protected:
				Element() : _initialized(false)
				{
/*
					Map is defined in derived class constructor, since I need the libMesh geometry
*/
					//TODO 
					// it must call a factory for the reference element and a factory for the map
					// the procedure must guarantee that the map is affine to the correspondent libMesh reference element,
					// on which the Qbase rule, stored in _refElement, is defined
				};

			public:
				virtual ~Element()
				{
#ifdef MYDEBUG
				cout << "Distruggo Element" << endl;
#endif //MYDEBUG
				};

				virtual elementType type() = 0;
				virtual nodes_ptr get_nodes() = 0;

				virtual void init()
				{
				//TODO: create helper function to combine factory instantiation and call to the object creation
					auto& el_factory(StdElementFactory<DIM>::Instance());
					_refElement = el_factory.create(type());
					auto& map_factory(AffineMapFactory<DIM>::Instance());
					_map = move(map_factory.create(type()));
					_map -> init(get_nodes());
					_initialized = true;
				};

				//TODO: verify if it's convenient to store the quadrature nodes and weights as an attribute of the element
				virtual QuadPointVec<DIM> getQuadPoints()const
				{
					check_initialization();

					QuadPointVec<DIM> points = _refElement->getQuadPoints();

					for (auto p : points)
						p = _map->evaluate(p);

					return points;
				};

				virtual QuadWeightVec getQuadWeights()const
				{
					check_initialization();

					QuadWeightVec weights = _refElement->getQuadWeights();
/*
					I'm taking advantage from the fact that _map is affine, so it has an evaluateJacobian() method
					not dependent on the point of evaluation
*/
					for (size_t i(0); i < weights.size(); ++i)
						weights[i] = weights[i] * _map->Jacobian();

					return weights;
				};

			protected:
				AffineMap<DIM>& useMap()const
				{
					check_initialization();
					return *_map;
				};

				bool _initialized;

				virtual void check_initialization()const
				{
					if (!_initialized)
						throw runtime_error("Trying to use uninitialized element");
				};
			private:
				unique_ptr<AffineMap<DIM>>	_map;
				shared_ptr<StdBananaElement<DIM>> _refElement;
		};

	template <size_t DIM>
		class	FElement : public Element<DIM>, public AbstractSpace<DIM>
		{
			public:
				FElement()
				{			};

				virtual ~FElement()
				{
#ifdef MYDEBUG
				cout << "Distruggo FElement" << endl;
#endif //MYDEBUG
				};

				virtual elementType type() = 0;
				virtual nodes_ptr get_nodes() = 0;

				virtual void init() override
				{
					auto& space_factory(StdSpaceFactory<DIM>::Instance());
					_refSpace = space_factory.create(FEType());
					Element<DIM>::init();
				};

				//TODO: verify if it could be better to make the basisType a template parameter of the FElement
				virtual Basis::basisType FEType() override
				{
					//TODO: use GetPot to select basisType at runtime
					return Basis::LEGENDRE;
				};

				//TODO: optimize storing already evaluated points
				virtual double evaluateBasisFunction (size_t ind, const Point<DIM>& point)const
				{
					this->check_initialization();
					return _refSpace->evaluateBasisFunction(ind, this->useMap().computeInverse(point));
				};

				virtual vector<double> evaluateBasis (const Point<DIM>& point)const
				{
					this->check_initialization();
					return _refSpace->evaluateBasis ( this->useMap().computeInverse(point) );
				};

				virtual size_t	basisSize()const
				{
					this->check_initialization();
					return _refSpace->basisSize();
				};

			protected:
				shared_ptr<StdBananaSpace<DIM>> _refSpace;
		};


};//namespace BinaryTree

#endif //__ELEMENT_H
