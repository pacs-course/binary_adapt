#ifndef __ELEMENT_H
#define __ELEMENT_H

#include "Maps.h"
#include "ReferenceElement.h"

#include <face_tri3.h> //libMesh::Tri3
#include <edge_edge2.h> //libMesh::Edge2

#include <memory> //std::shared_ptr, std::unique_ptr
#include <utility> //std::move
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
				AbstractFElement() : _pLevel(0), _initialized(false){};

				virtual ~AbstractFElement()
				{
#ifdef DESTRUCTOR_ALERT
				cout << "Distruggo AbstractFElement" << endl;
#endif //DESTRUCTOR_ALERT
				};

				virtual void init()
				{
					if(!_initialized)
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
					}
				};

/*
				methods depending on libmesh geometry, cannot be defined at this level
*/
				virtual nodes_ptr get_nodes() = 0;
				virtual elementType type()const = 0;

				virtual basisType FEType() override
				{
					return FETYPE;
				};


				//TODO: verify if it's convenient to store the quadrature nodes and weights as an attribute of the element
				virtual QuadPointVec<DIM> getQuadPoints()const
				{
					check_initialization();

					QuadPointVec<DIM> points = _refFElement->getQuadPoints();

					for (auto& p : points)
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
					return 	_refFElement->evaluateBasisFunction(ind, _map->computeInverse(point))
								/								//to normalize the basis I divide by the constant jacobian^1/2 of the map
								sqrt(_map->Jacobian())
								;
				};


//				virtual vector<double> evaluateBasis(const Point<DIM>& point)const
//				{
//					check_initialization();

//					vector<double> result;
//					for (size_t i(0); i < basisSize(); ++i)
//						result.push_back(evaluateBasisFunction(i, point));

//					return result;
//				};

				virtual size_t	basisSize(size_t degree)const
				{
					check_initialization();
					return _refFElement->basisSize(degree);
				};

				virtual size_t basisSize()const
				{
					check_initialization();
					return _refFElement->basisSize(_pLevel);
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
/*
				It is how much is incremented the underlying basis degree w.r.t the default basis degree (which is 0 for Legendre, that is approximation by a constant)
				By default is 0; 
*/
				size_t _pLevel;
/*
				A flag which controls if the init method has been called
*/
				bool _initialized;
/*
				The reference standard element associated to the type of the object
				It is meant to be a singleton, so it's stored as a shared_ptr
*/
				shared_ptr<StdBananaFElement<DIM, FETYPE>> _refFElement;

			private:
/*
				The affine map through which I can pass from the reference element to the current domain of the object
*/
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

			elementType type() const
			{
				return LibmeshInvalidType;
			};
			nodes_ptr get_nodes()
			{
				return nullptr;
			};

			void refine(libMesh::MeshRefinement& mr)
			{
				//to silence warning by the compiler
				(void)mr;
				throw logic_error("Trying to refine an invalid geometry");
			};

			unsigned int dim () const
			{
				return 0;
			};
	};

	//TODO: check if the virtual inheritance could be dangerous in this case
	template <size_t DIM, basisType FETYPE = INVALID, class LibmeshGeometry = InvalidGeometry>
		class FElement : public AbstractFElement<DIM, FETYPE>, public LibmeshGeometry
		{
			public:
				//TODO:	the default constructor makes damages when LibmeshGeometry() is called
				//			add the parameters needed by the libMesh::Elem constructor
				FElement()
				{
					cout << "WARNING: I'm calling the LibmeshGeometry constructor without parameters" << endl;
				};

				FElement(unique_ptr<LibmeshGeometry> element) : LibmeshGeometry(*element), _geom(move(element))
				{
#ifdef MYDEBUG
					cout << "Sono nel costruttore di FElement" << endl;
					std::cout << "CONTROLLO NODI" << std::endl;
					auto el_nodes = _geom->get_nodes();
					std::cout << "primo nodo :" << (*(el_nodes[0]))(0) << std::endl;
					std::cout << "secondo nodo :" << (*(el_nodes[1]))(0) << std::endl;

					cout << "Controllo i nodi di FElement" << endl;
					std::cout << "CONTROLLO NODI" << std::endl;
					el_nodes = _geom->get_nodes();
					std::cout << "primo nodo :" << (*(el_nodes[0]))(0) << std::endl;
					std::cout << "secondo nodo :" << (*(el_nodes[1]))(0) << std::endl;
#endif //MYDEBUG
				};

				~FElement(){};
				virtual elementType type() const override
				{
					return LibmeshGeometry::type();
				};
				virtual nodes_ptr get_nodes() override
				{
					return LibmeshGeometry::get_nodes();
				};

				virtual unsigned int dim () const override
				{
#ifdef MYDEBUG
					cout << "WELL DONE! libMesh method overwritten!" << endl;
#endif //MYDEBUG
					return LibmeshGeometry::dim();
				};

			private:
/*
				This attribute will not be used, it's presente only to avoid memory leakage.
				Since the LibmeshGeometry class does not implement neither a smart copy constructor, nor a smart assignment operator, I store here the pointer to the element I want my stuff to be added; since I store it as a unique_ptr, I take the ownership of it, so there's no way that the original libmesh object is accessed directly from the outside.
				This trick guarantees me that, when destroying my FElement, also the underlying object will be destroyed.
*/
				unique_ptr<LibmeshGeometry> _geom;
		};

};//namespace BinaryTree

#endif //__ELEMENT_H
