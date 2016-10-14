#ifndef __ELEMENT_H
#define __ELEMENT_H

#include "Maps.h"
#include "ReferenceElement.h"

#include <face_tri3.h> //libMesh::Tri3
#include <edge_edge2.h> //libMesh::Edge2

#include <memory> //shared_ptr

using namespace std;
using namespace Maps;
using namespace Geometry;
using namespace Basis;

namespace BinaryTree
{
	template <size_t DIM, basisType FETYPE>
		using StdFElementFactory	= GenericFactory::SingletonFactory	<StdBananaFElement<DIM, FETYPE>, elementType>;

	template <size_t DIM>
		using AffineMapFactory		= GenericFactory::ObjectFactory		<AffineMap<DIM>					, elementType>;


	template <size_t DIM, basisType FETYPE = INVALID>
		class	AbstractFElement : public AbstractElement<DIM>, public AbstractSpace<DIM>
		{
			public:
/*
					_map and _refFElement initialization depends on the pure virtual method type() return, so it cannot be done during object construction; so this operation is done by init method, which call is compulsory before using the object
*/
				AbstractFElement() : _initialized(false){};

				virtual ~AbstractFElement()
				{
#ifdef DESTRUCTOR_ALERT
				cout << "Distruggo AbstractFElement" << endl;
#endif //DESTRUCTOR_ALERT
				};

				virtual void init()
				{
#ifdef MYDEBUG
					cout << "Provo a inizializzare AbstractFElement" << endl;
#endif //MYDEBUG
				//TODO: create helper function to combine factory instantiation and call to the object creation
					auto& felem_factory(StdFElementFactory<DIM, FETYPE>::Instance());
#ifdef MYDEBUG
					cout << "cerco in StdFElementFactory<" << DIM << ", " << FETYPE <<"> con chiave " << type() << endl;
#endif //MYDEBUG
					_refFElement = felem_factory.create(type());
#ifdef MYDEBUG
					cout << "refFElement creato" << endl;
#endif //MYDEBUG
					auto& map_factory(AffineMapFactory<DIM>::Instance());
					_map = move(map_factory.create(type()));
#ifdef MYDEBUG
					cout << "mappa creata" << endl;
#endif //MYDEBUG
					_map -> init(get_nodes());
					_initialized = true;
#ifdef MYDEBUG
					cout << "AbstractFElement inizializzato" << endl;
#endif //MYDEBUG
				};

/*
				methods depending on libmesh geometry, cannot be defined at this level
*/
				virtual nodes_ptr get_nodes() = 0;
				virtual elementType type() = 0;

				virtual basisType FEType() override
				{
					return FETYPE;
				};


				//TODO: verify if it's convenient to store the quadrature nodes and weights as an attribute of the element
				virtual QuadPointVec<DIM> getQuadPoints()const
				{
					check_initialization();

					QuadPointVec<DIM> points = _refFElement->getQuadPoints();

					for (auto p : points)
						p = _map->evaluate(p);

					return points;
				};

				virtual QuadWeightVec getQuadWeights()const
				{
					check_initialization();

					QuadWeightVec weights = _refFElement->getQuadWeights();
/*
					I'm taking advantage from the fact that _map is affine, so it has an evaluateJacobian() method
					not dependent on the point of evaluation
*/
					for (ptrdiff_t i(0); i < weights.size(); ++i)
						weights[i] = weights[i] * _map->Jacobian();

					return weights;
				};

				//TODO: optimize storing already evaluated points
				virtual double evaluateBasisFunction (size_t ind, const Point<DIM>& point)const
				{
					check_initialization();
					return _refFElement->evaluateBasisFunction(ind, _map->computeInverse(point));
				};

				virtual vector<double> evaluateBasis (const Point<DIM>& point)const
				{
					check_initialization();
					return _refFElement->evaluateBasis ( _map->computeInverse(point) );
				};

				virtual size_t	basisSize()const
				{
					check_initialization();
					return _refFElement->basisSize();
				};

				virtual size_t pLevel() const
				{
					return _pLevel;
				};
				virtual void pLevel(size_t newP)
				{
					_pLevel = newP;
				};

			protected:

				AffineMap<DIM>& useMap()const
				{
					check_initialization();
					return *_map;
				};

				virtual void check_initialization()const
				{
					if (!_initialized)
						throw runtime_error("Trying to use uninitialized element");
				};

			protected:
				size_t _pLevel;
				bool _initialized;
				shared_ptr<StdBananaFElement<DIM, FETYPE>> _refFElement;

			private:
				unique_ptr<AffineMap<DIM>>	_map;
		};

	using LibmeshInterval = libMesh::Edge2;
	using LibmeshTriangle = libMesh::Tri3;

	class InvalidGeometry
	{
		public:
			InvalidGeometry()
			{
				throw logic_error("Trying to use an invalid geometry");
			};
			~InvalidGeometry(){};

			elementType type()
			{
				return LibmeshInvalidType;
			};
			nodes_ptr get_nodes()
			{
				return nullptr;
			};
	};

	//TODO: check if the virtual inheritance could be dangerous in this case
	template <size_t DIM, basisType FETYPE = INVALID, class LibmeshGeometry = InvalidGeometry>
		class FElement : public AbstractFElement<DIM, FETYPE>, public LibmeshGeometry
		{
			public:
				FElement(){};
				~FElement(){};
				virtual elementType type()override
				{
					return LibmeshGeometry::type();
				};
				virtual nodes_ptr get_nodes()override
				{
					return LibmeshGeometry::get_nodes();
				};
		};

};//namespace BinaryTree

#endif //__ELEMENT_H
