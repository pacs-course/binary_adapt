#ifndef __REFERENCE_H
#define __REFERENCE_H

#include "AbstractEl.h"
#include "Maps.h"

#include <point.h> //libMesh::Point
#include <quadrature.h> //libMesh::QBase

#include <memory> //make_unique
#include <utility> //move

using namespace std;
using namespace Maps;

namespace BinaryTree
{
/*
	This is a hat class for StdElements which represents the return type of the std elements factory
	It must not depend on the elementType template parameter
	Moreover it must guarantee the AbstractElement interface
*/

	template <size_t DIM>
		class StdBananaElement : public AbstractElement<DIM>
		{
			public:
				StdBananaElement(){};
				virtual ~StdBananaElement(){};
				virtual elementType type() = 0;
				virtual QuadPointVec<DIM> getQuadPoints()const = 0;
				virtual QuadWeightVec getQuadWeights()const = 0;
		};

	template <size_t DIM, elementType TYPE = libMesh::INVALID_ELEM>
		class StdElement : public StdBananaElement<DIM>
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
#ifdef MYDEBUG
				cout << "Distruggo StdElement" << endl;
#endif //MYDEBUG
				};

				virtual elementType type()
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
	Simmetrically, for the std space class, as for the std element one
*/
	template <size_t DIM>
		class StdBananaSpace : public AbstractSpace<DIM>
		{
			public:
				StdBananaSpace(){};
				virtual ~StdBananaSpace(){};
				virtual double				evaluateBasisFunction(size_t ind, const Point<DIM>& point)const = 0;
				virtual vector<double>	evaluateBasis			(				 const Point<DIM>& point)const = 0;
				virtual size_t	basisSize()const = 0;
				virtual size_t basisDegree()const = 0;
				virtual void basisDegree(size_t) = 0;

				virtual Basis::basisType FEType() = 0;
		};

//	template <size_t DIM>
//		using MapFactory	= GenericFactory::ObjectFactory		<Map<DIM>, elementType>;

	template <size_t DIM, Basis::basisType FETYPE = Basis::INVALID>
		class StdIperCube : public StdBananaSpace<DIM>
		{
#ifdef SINGLETON_ENABLED
/*
			It is a Singleton
*/
			protected:
				StdIperCube()
				{
					_basis = Basis::PolinomialBasis<DIM, FETYPE>::Instance();
				};

				StdIperCube& operator = (const StdIperCube&) = delete;
				StdIperCube					(const StdIperCube&) = delete;
			public:
				static shared_ptr<StdIperCube> Instance()
				{
					static shared_ptr<StdIperCube> sic = shared_ptr<StdIperCube>(new StdIperCube);
					return sic;
				};
#else
			public:
				StdIperCube()
				{
					_basis = make_shared<Basis::PolinomialBasis<DIM, FETYPE> >();
				};
#endif //SINGLETON_ENABLED

				virtual ~StdIperCube()
				{
					//TODO
#ifdef MYDEBUG
				cout << "Distruggo IperCube" << endl;
#endif //MYDEBUG
				};

				virtual Basis::basisType FEType()
				{
					return FETYPE;
				};

				//TODO: optimize storing already evaluated points
				virtual double evaluateBasisFunction (size_t ind, const Point<DIM>& point)const
				{
					return (*_basis)[ind](point);
				};

				virtual vector<double> evaluateBasis(const Point<DIM>& point)const
				{
					vector<double> result;
					for (auto fun = _basis->begin(); fun != _basis->end(); ++fun)
						result.push_back((*fun)(point));

					return result;
				};

				virtual size_t basisDegree()const
				{
					return _basis->degree();
				};

				virtual void basisDegree(size_t deg)
				{
					_basis->degree(deg);
				};

				virtual size_t basisSize()const
				{
					return _basis->size();
				};

			protected:
				shared_ptr<Basis::PolinomialBasis<DIM, FETYPE>> _basis;
		};

	template <size_t DIM, Basis::basisType FETYPE = Basis::INVALID>
		class StdSpace : public StdBananaSpace<DIM>
		{
#ifdef SINGLETON_ENABLED
			//It is a Singleton
			protected:
				StdSpace()
				{
					_stdCube = StdIperCube<DIM, FETYPE>::Instance();
					//TODO: initialize _ipercubeMap through the call to a factory
				};

				StdSpace& operator = (const StdSpace&) = delete;
				StdSpace					(const StdSpace&) = delete;

			public:
				static shared_ptr<StdSpace> Instance()
				{
					static shared_ptr<StdSpace> ss = shared_ptr<StdSpace>(new StdSpace);
					return ss;
				};
#else
			public:
				StdSpace()
				{
					_stdCube = make_shared<StdIperCube<DIM, FETYPE> >();
					//TODO: initialize _ipercubeMap through the call to a factory
				};

#endif //SINGLETON_ENABLED

				virtual ~StdSpace()
				{
					//TODO
#ifdef MYDEBUG
				cout << "Distruggo StdSpace" << endl;
#endif //MYDEBUG
				};
				virtual Basis::basisType FEType()
				{
					return FETYPE;
				};

				//TODO: optimize storing already evaluated points
				virtual double evaluateBasisFunction (size_t ind, const Point<DIM>& point)const
				{
					return _stdCube->evaluateBasisFunction(ind, _ipercubeMap->computeInverse(point));
				};

				virtual vector<double> evaluateBasis (const Point<DIM>& point)const
				{
					return _stdCube->evaluateBasis(_ipercubeMap->computeInverse(point));
				};

				virtual size_t basisSize()const
				{
					return _stdCube->basisSize();
				};

				virtual size_t basisDegree()const
				{
					return _stdCube->basisDegree();
				};

				virtual void basisDegree(size_t d)
				{
					_stdCube->basisDegree(d);
				};

			protected:
				unique_ptr<Map<DIM>>							_ipercubeMap;
				shared_ptr<StdIperCube<DIM, FETYPE>>	_stdCube;
		};

};//namespace BinaryTree
#endif //__REFERENCE_H
