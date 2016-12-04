#ifndef __STD_F_ELEMENT_H
#define __STD_F_ELEMENT_H

#include "ReferenceElement.h"
#include "BinaryTreeHelper.h"
#include "AbstractSpace.h"

using namespace std;
using namespace Geometry;

namespace FiniteElements
{
/*
	This is a hat class for StdFElement which represents the return type of the std FElements factory
	It must not depend on the ElementType template parameter, since it is known at runtime
	Moreover it must guarantee the AbstractSpace_Interface interface
*/
	template <size_t dim, BasisType FeType = InvalidFeType>
		class StdBananaFElement : public AbstractSpace_Interface<dim>, public AbstractElement<dim>
		{
			public:
				virtual ~StdBananaFElement(){};
/*
				AbstractSpace_Interface methods
*/
				virtual double	EvaluateBasisFunction (size_t ind, const Point<dim>& point)const = 0;
				virtual size_t	BasisSize (size_t)const = 0;
				virtual BasisType GetFeType ()const = 0;
/*
				AbstractElement methods
*/
				virtual ElementType GetType() const = 0;
				virtual QuadPointVec<dim> GetQuadPoints()const = 0;
				virtual QuadWeightVec GetQuadWeights()const = 0;
		};


	template <size_t dim, BasisType FeType = InvalidFeType>
		class StdFIperCube : public StdBananaFElement<dim, FeType>
		{
			public:
#ifdef SINGLETON_ENABLED
				static shared_ptr<StdFIperCube> Instance()
				{
					static shared_ptr<StdFIperCube> sic (new StdFIperCube);
					return sic;
				};
/*
			It is a Singleton
*/
			protected:

				StdFIperCube& operator = (const StdFIperCube&) = delete;
				StdFIperCube (const StdFIperCube&) = delete;

				/* If I do not initialize at nullptr the compiler fails trying to call the protected default constructor of the attributes */
				StdFIperCube () : _basis(nullptr), _std_geometry(nullptr)
				{
					_std_geometry = StdIperCube<dim>::Instance();
					_basis = PolinomialBasis<dim, FeType>::Instance();
#else
				StdFIperCube()
				{
					_std_geometry = HelperFunctions::Builders<StdIperCube<dim>>::BuildSingleton ();
					_basis = HelperFunctions::Builders<PolinomialBasis<dim, FeType>>::BuildSingleton ();
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

				virtual BasisType GetFeType()const
				{
					return FeType;
				};

				//TODO: optimize storing already evaluated points
				virtual double EvaluateBasisFunction (size_t ind, const Point<dim>& point)const
				{
					return this->_basis->Evaluate(ind, point);
				};

				virtual size_t BasisSize(size_t degree)const
				{
					return this->_basis->size(degree);
				};

				virtual void PrintBasis(size_t index) const
				{
					this->_basis->print(index);
				};

				virtual void PrintBasis() const
				{
					this->_basis->print();
				};

				virtual ElementType GetType()const
				{
					return this->_std_geometry->GetType();
				};
				virtual QuadPointVec<dim> GetQuadPoints()const
				{
					return this->_std_geometry->GetQuadPoints();
				};
				virtual QuadWeightVec GetQuadWeights()const
				{
					return this->_std_geometry->GetQuadWeights();
				};

			protected:
				shared_ptr<PolinomialBasis<dim, FeType>> _basis;
				shared_ptr<StdIperCube<dim> > _std_geometry;
		};

	template <size_t dim, ElementType Type = InvalidElementType, BasisType FeType = InvalidFeType>
		class StdFElement : public StdBananaFElement<dim, FeType>
		{
			public:
#ifdef SINGLETON_ENABLED
				static shared_ptr<StdFElement> Instance()
				{
					static shared_ptr<StdFElement> ss(new StdFElement);
					return ss;
				};
			//It is a Singleton
			protected:
				StdFElement& operator = (const StdFElement&) = delete;
				StdFElement					(const StdFElement&) = delete;

				/* If I do not initialize at nullptr the compiler fails trying to call the protected default constructor of the attributes */
				StdFElement() : _std_cube(nullptr), _std_geometry(nullptr)
				{
					_std_geometry = StdElement<dim, Type>::Instance();
					_std_cube = StdFIperCube<dim, FeType>::Instance();
#else //SINGLETON_ENABLED
				StdFElement()
				{
					_std_geometry = HelperFunctions::Builders<StdElement<dim, Type> >::BuildSingleton ();
					_std_cube = HelperFunctions::Builders<StdFIperCube<dim, FeType> >::BuildSingleton ();
#endif //SINGLETON_ENABLED

					auto& std_map_factory(StdMapFactory<dim>::Instance());
					_ipercube_map = std_map_factory.create(Type);
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
				virtual BasisType GetFeType()const
				{
					return FeType;
				};

				//TODO: optimize storing already evaluated points
				virtual double EvaluateBasisFunction (size_t ind, const Point<dim>& point)const
				{
					return this->_std_cube->EvaluateBasisFunction(ind, MapBackward(point));
				};

				virtual size_t BasisSize(size_t degree)const
				{
					return this->_std_cube->BasisSize(degree);
				};

				virtual ElementType GetType()const
				{
					return this->_std_geometry->GetType();
				};
				virtual QuadPointVec<dim> GetQuadPoints()const
				{
					return this->_std_geometry->GetQuadPoints();
				};
				virtual QuadWeightVec GetQuadWeights()const
				{
					return this->_std_geometry->GetQuadWeights();
				};

/*
				Public methods to use the _ipercube_map attribute
*/
				virtual Point<dim> MapBackward(const Point<dim>& p)const
				{
					return this->_ipercube_map->ComputeInverse(p);
				};

				virtual Point<dim> MapForward(const Point<dim>& p)const
				{
					return this->_ipercube_map->Evaluate(p);
				};

				virtual double Jacobian(const Point<dim>& p)const
				{
					return this->_ipercube_map->EvaluateJacobian(p);
				};

			protected:
				shared_ptr<StdFIperCube<dim, FeType>>	_std_cube;
				shared_ptr<StdElement<dim, Type>> _std_geometry;
				unique_ptr<Map<dim>>	_ipercube_map;
		};

} //namespace FiniteElements

#endif //__STD_F_ELEMENT_H
