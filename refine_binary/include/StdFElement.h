#ifndef __STD_F_ELEMENT_H
#define __STD_F_ELEMENT_H

#include "StdFElementInterface.h"
#include "BinaryTreeHelper.h"
#include "ConcreteFactories.h"


namespace FiniteElements
{
	using namespace std;

	template <size_t dim, BasisType FeType = InvalidFeType>
		class AlmostStdFIperCube : public StdFElementInterface<dim, FeType>
		{
			public:
				AlmostStdFIperCube()
				{
#ifdef DESTRUCTOR_ALERT
					clog << "Costruisco AlmostStdFIperCube" << endl;
#endif //DESTRUCTOR_ALERT

					_std_geometry = Helpers::Builders<Geometry::StdIperCube<dim>>::BuildSingleton ();

					auto& basis_factory (GenericFactory::TensorialBasisFactory<dim>::Instance());
					_basis = move(basis_factory.create(FeType));

#ifdef DESTRUCTOR_ALERT
					clog << "Esco dal costruttore di AlmostStdFIperCube" << endl;
#endif //DESTRUCTOR_ALERT
				};

			public:
				virtual ~AlmostStdFIperCube()
				{
#ifdef DESTRUCTOR_ALERT
					clog << "Distruggo AlmostStdFIperCube" << endl;
#endif //DESTRUCTOR_ALERT
				};

				virtual BasisType GetFeType()const
				{
					return FeType;
				};

				//TODO: optimize storing already evaluated points
				virtual double EvaluateBasisFunction (size_t ind, const Geometry::Point<dim>& point)const
				{
					return this->_basis->Evaluate(ind, point);
				};

				virtual Geometry::Vector EvaluateBasis (size_t degree, const Geometry::Point<dim>& point)const
				{
					return this->_basis->EvaluateBasis(degree, point);
				};

				virtual size_t BasisSize(size_t degree)const
				{
					return this->_basis->ComputeSize(degree);
				};

				virtual Geometry::ElementType GetType()const
				{
					return this->_std_geometry->GetType();
				};
				virtual Geometry::QuadPointVec<dim> GetQuadPoints()const
				{
					return this->_std_geometry->GetQuadPoints();
				};
				virtual Geometry::QuadWeightVec GetQuadWeights()const
				{
					return this->_std_geometry->GetQuadWeights();
				};
				virtual size_t QuadratureOrder()const
				{
					return this->_std_geometry->QuadratureOrder();
				};

			protected:
				unique_ptr<TensorialBasis<dim>> _basis;
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
				StdFIperCube() : AlmostStdFIperCube<dim, FeType>(){};

				virtual ~StdFIperCube()
				{
#ifdef DESTRUCTOR_ALERT
					clog << "Destructing StdFIperCube" << endl;
#endif //DESTRUCTOR_ALERT
				};

			protected:
				virtual double ComputeNormSquared(size_t ind) override
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
				StdFIperCube() : AlmostStdFIperCube<dim, LegendreType>(){};

				virtual ~StdFIperCube()
				{
#ifdef DESTRUCTOR_ALERT
					clog << "Destructing StdFIperCube" << endl;
#endif //DESTRUCTOR_ALERT
				};

			protected:
				virtual double ComputeNormSquared(size_t ind) override
				{
					auto indexes = this->_basis->GetIndexes(ind);
					double inverse_norm = 1;
					for (auto k : indexes)
						inverse_norm *= static_cast<double>(k) + 0.5;

					return 1 / inverse_norm;
				};
		};

	template <size_t dim, Geometry::ElementType Type = Geometry::InvalidElementType, BasisType FeType = InvalidFeType>
		class StdFElement : public StdFElementInterface<dim, FeType>
		{
			public:
				StdFElement()
				{
					_std_geometry = Helpers::Builders<Geometry::StdElement<dim, Type> >::BuildSingleton ();
					_std_cube = Helpers::Builders<StdFIperCube<dim, FeType> >::BuildSingleton ();

					auto& std_map_factory(GenericFactory::StdMapFactory<dim>::Instance());
					_ipercube_map = std_map_factory.create(Type);

					//add here other stuff the constructor is expected to do
				};

			public:
				virtual ~StdFElement()
				{
#ifdef DESTRUCTOR_ALERT
					clog << "Distruggo StdFElement" << endl;
#endif //DESTRUCTOR_ALERT
				};

				virtual BasisType GetFeType()const
				{
					return FeType;
				};

				//TODO: optimize storing already evaluated points
				virtual double EvaluateBasisFunction (size_t ind, const Geometry::Point<dim>& point)const
				{
					return this->_std_cube->EvaluateBasisFunction(ind, MapBackward(point));
				};

				virtual Geometry::Vector EvaluateBasis (size_t degree, const Geometry::Point<dim>& point)const
				{
					return this->_std_cube->EvaluateBasis(degree, MapBackward(point));
				};

				virtual size_t BasisSize(size_t degree)const
				{
					return this->_std_cube->BasisSize(degree);
				};

				virtual Geometry::ElementType GetType()const
				{
					return this->_std_geometry->GetType();
				};
				virtual Geometry::QuadPointVec<dim> GetQuadPoints()const
				{
					return this->_std_geometry->GetQuadPoints();
				};
				virtual Geometry::QuadWeightVec GetQuadWeights()const
				{
					return this->_std_geometry->GetQuadWeights();
				};

				virtual size_t QuadratureOrder()const
				{
					return this->_std_geometry->QuadratureOrder();
				};

/*
				Public methods to use the _ipercube_map attribute
*/
				virtual Geometry::Point<dim> MapBackward(const Geometry::Point<dim>& p)const
				{
					return this->_ipercube_map->ComputeInverse(p);
				};

				virtual Geometry::Point<dim> MapForward(const Geometry::Point<dim>& p)const
				{
					return this->_ipercube_map->Evaluate(p);
				};

				virtual double Jacobian(const Geometry::Point<dim>& p)const
				{
					return this->_ipercube_map->EvaluateJacobian(p);
				};

			protected:
				virtual double ComputeNormSquared(size_t ind) override
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

				virtual void UpdateNorms(size_t ind) override
				{
					size_t l = (this->_norm_values).Size();
					size_t new_l = ind+1;

					if (l < new_l)
					{
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

			protected:
				shared_ptr<StdFIperCube<dim, FeType>>			_std_cube;
				shared_ptr<Geometry::StdElement<dim, Type>>	_std_geometry;
				unique_ptr<Geometry::Map<dim>>					_ipercube_map;
		};

} //namespace FiniteElements

#endif //__STD_F_ELEMENT_H
