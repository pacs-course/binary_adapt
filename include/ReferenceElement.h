#ifndef __REFERENCE_H
#define __REFERENCE_H

#include "AbstractEl.h"
#include "Maps.h"

#include <point.h> //libMesh::Point
#include <quadrature.h> //libMesh::QBase

#include <memory> //make_shared
#include <utility> //move

using namespace std;
using namespace Maps;
using namespace Basis;

namespace BinaryTree
{
/*
	renaming libMesh enumerators for the use in my program
	since in libMesh there are multiple types describing the same geometry, in this way a potential switch is easier, since it must be done only ones here
*/
#define LibmeshInvalidType libMesh::INVALID_ELEM
#define LibmeshIntervalType libMesh::EDGE2
#define LibmeshSquareType libMesh::QUAD4
#define LibmeshCubeType libMesh::HEX8

	template <size_t DIM>
		using StdMapFactory	= GenericFactory::ObjectFactory <Map<DIM>, elementType>;

	template <size_t DIM, elementType TYPE = LibmeshInvalidType>
		class StdElement : public AbstractElement<DIM>
		{
			public:
#ifdef SINGLETON_ENABLED
				static shared_ptr<StdElement> Instance()
				{
					static shared_ptr<StdElement> el = shared_ptr<StdElement>(new StdElement);					
					return el;
				};
/*
			It is a Singleton
*/
				StdElement& operator =	(const StdElement&) = delete;
				StdElement					(const StdElement&) = delete;

			protected:
#endif //SINGLETON_ENABLED
				StdElement()
				{
					//TODO: use GetPot to configure quadrature options
					auto ptr = libMesh::QBase::build(libMesh::QGAUSS, DIM, libMesh::FORTYTHIRD);
					_quadratureRule = unique_ptr<libMesh::QBase>(ptr.release());
					_quadratureRule->init(TYPE);
				};

			public:
				virtual ~StdElement()
				{
					//TODO
#ifdef DESTRUCTOR_ALERT
				cout << "Distruggo StdElement" << endl;
#endif //DESTRUCTOR_ALERT
				};

				virtual elementType type() const
				{
					return TYPE;
				};

				//TODO: verify if it's convenient to store the quadrature nodes and weights as an attribute of the element
				virtual QuadPointVec<DIM> getQuadPoints()const
				{
					//TODO: instead of convert here the libmesh::point, maybe it's convenient to define QuadPointVec in terms of the libmesh type 
					vector<libMesh::Point> stdpoints = _quadratureRule->get_points();
					QuadPointVec<DIM> result;
					for (auto lp : stdpoints)
						result.push_back(lp);

					return result;
				};

				virtual QuadWeightVec getQuadWeights()const
				{
					//TODO: instead of convert here the libmesh::Real, maybe it's convenient to define QuadWeightVec in terms of the libmesh type 
					vector<libMesh::Real> stdweights = _quadratureRule->get_weights();
					QuadWeightVec result(stdweights.size());
					size_t i(0);
					for (auto w : stdweights)
						result(i++) = w;

					return result;
				};

			protected:
				unique_ptr<libMesh::QBase> _quadratureRule;
		};


/*
	This is a hat class for StdFElement which represents the return type of the std FElements factory
	It must not depend on the elementType template parameter, since it is known at runtime
	Moreover it must guarantee the AbstractSpace interface
*/
	template <size_t DIM, basisType FETYPE = INVALID>
		class StdBananaFElement : public AbstractSpace<DIM>, public AbstractElement<DIM>
		{
			public:
				StdBananaFElement(){};
				virtual ~StdBananaFElement(){};
/*
				AbstractSpace methods
*/
				virtual double	evaluateBasisFunction(size_t ind, const Point<DIM>& point)const = 0;
				virtual size_t	basisSize	(size_t)const = 0;
//				virtual size_t basisDegree	()const = 0;
//				virtual void basisDegree	(size_t) = 0;
				virtual basisType FEType	() = 0;
/*
				AbstractElement methods
*/
				virtual elementType type() const = 0;
				virtual QuadPointVec<DIM> getQuadPoints()const = 0;
				virtual QuadWeightVec getQuadWeights()const = 0;
		};

/*
	I don't have a general definition of std ipercube geometry since I depend on the libMesh geometries, which are defined only in 1, 2 and 3 dimensions.
	In a more general setting, it's not a priori excluded the possibility to define my std ipercube, on which the quadrature nodes and weigths are constructed by tensorization of the 1d counterparts; this is similarly already done for basis functions.
	However, in this case the default StdIperCube is set as Invalid, so the respectively stdInterval, stdSquare, stdCube are defined through specialization on the DIM parameter.
*/
namespace detail
{
	template <size_t DIM>
	struct alias
	{
		using aliasIperCube = StdElement<DIM, LibmeshInvalidType	>;
	};

	template <>
	struct alias<1>
	{
		using aliasIperCube = StdElement<1,	LibmeshIntervalType	>;
	};

	template <>
	struct alias<2>
	{
		using aliasIperCube = StdElement<2, LibmeshSquareType		>;
	};

	template <>
	struct alias<3>
	{
		using aliasIperCube = StdElement<3,	LibmeshCubeType		>;
	};
}//namespace detail

	template <size_t DIM>
		using StdIperCube = typename detail::alias<DIM>::aliasIperCube;

	template <size_t DIM, basisType FETYPE = INVALID>
		class StdFIperCube : public StdBananaFElement<DIM, FETYPE>
		{
			public:
#ifdef SINGLETON_ENABLED
				static shared_ptr<StdFIperCube> Instance()
				{
					static shared_ptr<StdFIperCube> sic = shared_ptr<StdFIperCube>(new StdFIperCube);
					return sic;
				};
/*
			It is a Singleton
*/
			protected:

				StdFIperCube& operator = (const StdFIperCube&) = delete;
				StdFIperCube					(const StdFIperCube&) = delete;
				StdFIperCube() : _std_geometry()
				{
					_basis = PolinomialBasis<DIM, FETYPE>::Instance();
#else
				StdFIperCube() : _std_geometry()
				{
					//TODO: wrong!!! I'm not sharing anything! Use a factory!
					_basis = make_shared<PolinomialBasis<DIM, FETYPE> >();
#endif //SINGLETON_ENABLED
					//add here other stuff the constructor is expected to do
#ifdef DESTRUCTOR_ALERT
					cerr << "Costruisco StdFIperCube" << endl;
#endif //DESTRUCTOR_ALERT
				};

			public:
				virtual ~StdFIperCube()
				{
					//TODO
#ifdef DESTRUCTOR_ALERT
					cerr << "Distruggo StdFIperCube" << endl;
#endif //DESTRUCTOR_ALERT
				};

				virtual basisType FEType()
				{
					return FETYPE;
				};

				//TODO: optimize storing already evaluated points
				virtual double evaluateBasisFunction (size_t ind, const Point<DIM>& point)const
				{
					return _basis->evaluate(ind, point);
				};

//				virtual size_t basisDegree()const
//				{
//					return _basis->degree();
//				};

//				virtual void basisDegree(size_t deg)
//				{
//					_basis->degree(deg);
//				};

				virtual size_t basisSize(size_t degree)const
				{
					return _basis->size(degree);
				};

				virtual void printBasis(size_t index) const
				{
					_basis->print(index);
				};

				virtual void printBasis() const
				{
					_basis->print();
				};

				virtual elementType type()const
				{
					return _std_geometry.type();
				};
				virtual QuadPointVec<DIM> getQuadPoints()const
				{
					return _std_geometry.getQuadPoints();
				};
				virtual QuadWeightVec getQuadWeights()const
				{
					return _std_geometry.getQuadWeights();
				};

			protected:
				shared_ptr<PolinomialBasis<DIM, FETYPE>> _basis;
				StdIperCube<DIM> _std_geometry;
		};

	template <size_t DIM, elementType TYPE = LibmeshInvalidType, basisType FETYPE = INVALID>
		class StdFElement : public StdBananaFElement<DIM, FETYPE>
		{
			public:
#ifdef SINGLETON_ENABLED
				static shared_ptr<StdFElement> Instance()
				{
					static shared_ptr<StdFElement> ss = shared_ptr<StdFElement>(new StdFElement);
					return ss;
				};
			//It is a Singleton
			protected:
				StdFElement& operator = (const StdFElement&) = delete;
				StdFElement					(const StdFElement&) = delete;

				StdFElement() : _std_geometry()
				{
					_stdCube = StdFIperCube<DIM, FETYPE>::Instance();
#else //SINGLETON_ENABLED
				StdFElement() : _std_geometry()
				{
					//TODO: wrong!!! I'm not sharing anything! Use a factory!
					_stdCube = make_shared<StdFIperCube<DIM, FETYPE> >();
#endif //SINGLETON_ENABLED

					auto& std_map_factory(StdMapFactory<DIM>::Instance());
					_ipercubeMap = std_map_factory.create(TYPE);
					//add here other stuff the constructor is expected to do
				};

			public:
				virtual ~StdFElement()
				{
					//TODO
#ifdef DESTRUCTOR_ALERT
				cout << "Distruggo StdFElement" << endl;
#endif //DESTRUCTOR_ALERT
				};
				virtual basisType FEType()
				{
					return FETYPE;
				};

				//TODO: optimize storing already evaluated points
				virtual double evaluateBasisFunction (size_t ind, const Point<DIM>& point)const
				{
					return _stdCube->evaluateBasisFunction(ind, mapBackward(point));
				};

				virtual size_t basisSize(size_t degree)const
				{
					return _stdCube->basisSize(degree);
				};

//				virtual size_t basisDegree()const
//				{
//					return _stdCube->basisDegree();
//				};

//				virtual void basisDegree(size_t d)
//				{
//					_stdCube->basisDegree(d);
//				};

				virtual elementType type()const
				{
					return _std_geometry.type();
				};
				virtual QuadPointVec<DIM> getQuadPoints()const
				{
					return _std_geometry.getQuadPoints();
				};
				virtual QuadWeightVec getQuadWeights()const
				{
					return _std_geometry.getQuadWeights();
				};

/*
				Public methods to use the _ipercubeMap attribute
*/
				virtual Point<DIM> mapBackward(const Point<DIM>& p)const
				{
					return _ipercubeMap->computeInverse(p);
				};

				virtual Point<DIM> mapForward(const Point<DIM>& p)const
				{
					return _ipercubeMap->evaluate(p);
				};

				virtual double Jacobian(const Point<DIM>& p)const
				{
					return _ipercubeMap->evaluateJacobian(p);
				};

			protected:
				shared_ptr<StdFIperCube<DIM, FETYPE>>	_stdCube;
				StdElement<DIM, TYPE> _std_geometry;
				unique_ptr<Map<DIM>>	_ipercubeMap;
		};

};//namespace BinaryTree
#endif //__REFERENCE_H
