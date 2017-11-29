#ifndef __BASIS__HH
#define __BASIS__HH

#include <memory> //unique_ptr
#include <stdexcept> //range_error

#include "AbstractFactory.h"
#include "TypeEnumerations.h"
#include "LinearAlgebra.h"


namespace FiniteElements
{
	using namespace std;

	template <size_t dim>
	using IndexVector = vector<array<size_t, dim>>;

	template <size_t dim, size_t d = dim>
		struct Filler
		{
/**
			Recursive computation of tensorial indexes.
			Here it is set the dim-D component of the index tuple in position *iter;
			the update of the *iter parameter will be done by the partial specialization for d=1,
			that is after that the last element of the tuple has been set.
			FillIndexes function cannot be set as TensorialBasis method because
			std c++ does not allow partial specialization of member functions.
**/
			static void FillIndexes(typename IndexVector<dim>::iterator& iter, size_t degree)
			{
				for(size_t current_degree = 0; current_degree <= degree; ++current_degree)
				{
					(*iter)[d-1] = current_degree;
					Filler<dim, d-1>::FillIndexes (iter, degree - current_degree);
				}
			};
		};

	template <size_t dim>
		struct Filler<dim, 1>
		{
			static void FillIndexes(typename IndexVector<dim>::iterator& iter, size_t degree)
			{
				(*iter)[0] = degree;
				++iter;			
			};
		};


	//TODO: maybe check the object to be effectively a basis (linearly independent and maximal)
	//have to be inserted in the construction or after it

/**
	Abstract class for polynomial basis.
	A basis is meant as an ordered set of functions.
	A basis object should provide a method to compute one of the basis function
	at a certain point, or each function until certain degree.
**/
	template <size_t dim> //working in |R ^ dim
		class AbstractBasis
		{
			protected:
/**
				constructor.
				For degree 0 the basis is given by 1 element
**/
				AbstractBasis();

/**
				Do the copy of attributes.
				To be called by assignment operator in derived classes.
**/
				void Copy(const AbstractBasis<dim>&);

			public:
/**
				default destructor
**/
				virtual ~AbstractBasis(){};

/**
				Evaluate one function at fixed point.
				Input parameters:
					- index of the function to be evaluated;
					- evaluation point.

**/
				virtual double Evaluate(size_t, const Geometry::Point<dim>&) = 0;
/**
				Evaluate functions until input degree at fixed point.
				Input parameters:
					- the degree of the basis: all basis functions with degree not higher than input
						will be evaluated;
					- evaluation point.

**/
				virtual Geometry::Vector EvaluateBasis(size_t degree, const Geometry::Point<dim>& p) = 0;

/**
				Size of a basis of input degree.
				It returns the number of basis functions with degree not higher than input value.

				 			(degree + dim)!
				Size = 	______________

							 degree! dim!
**/
				virtual size_t ComputeSize (const size_t&);

			protected:
/**
				_size_map[d] stores the size of an AbstractBasis<dim> of degree d
**/
				vector<size_t> _size_map;
		};

/**
	Abstract class for basis obtained by tensorization of 1D counterpart.
**/
	template <size_t dim>
		class TensorialBasis : public AbstractBasis<dim>
		{
			protected:
/**
				default constructor
**/
				TensorialBasis();

/**
				Do the copy of attributes.
				To be called by assignment operator in derived classes.
**/
				void Copy(const TensorialBasis<dim>&);

			public:
/**
				default destructor
**/
				virtual ~TensorialBasis(){};

/**
				Tensorization implementation.
				This function implements the tensorization of the OneDEvaluation() output. 
**/
				virtual double Evaluate(size_t, const Geometry::Point<dim>&) override;
/**
				As for Evaluate(), but it evaluates all basis function until input degree
**/
				virtual Geometry::Vector EvaluateBasis(size_t, const Geometry::Point<dim>&) override;

/**
				Get the tensorial indices.
**/
				array<size_t, dim> GetIndexes(size_t);

			protected:
/**
				The 1D function which output will be tensorized.
**/
				virtual double OneDEvaluation(size_t, double) const = 0;
/**
				Update the _tensorial_indexes if not already computed.
				It has to be called before performing the basis evaluation.
**/
				virtual void UpdateSize(size_t);

/**
				Print the _tensorial_indexes to standard output.
**/
				void PrintIndexes();

			protected:
/**
				The vector of indexes obtained by tensorization.
				_tensorial_indexes[i] gives a array of indexes:
				the i-th basis function is meant to be the tensorial product
				of functions of the 1D basis with those indexes.
				Ex: dim = 2
					_tensorial_indexes[i] = (j,k)
					the i-th basis function is the product of
					the j-th 1D function (meant as a function of "x") times
					the k-th 1D function (meant as a function of "y")
**/
				IndexVector<dim> _tensorial_indexes;
		};

	template <size_t dim>
		AbstractBasis<dim>::AbstractBasis() : _size_map(1,1)
		{};

	template <size_t dim>
		void AbstractBasis<dim>::Copy(const AbstractBasis<dim>& basis)
		{
			this->_size_map = basis._size_map;
		};

	template <size_t dim>
		size_t AbstractBasis<dim>::ComputeSize (const size_t& degree)
		{
			try
			{
				return this->_size_map.at(degree);
			}
			catch(std::out_of_range&)
			{
				//The value for degree = 0 is inserted by the constructor
				if (degree > 0)
				{
					size_t old_size = this->_size_map.size();
					this->_size_map.resize(degree + 1);
/*
					(deg + dim)!		(deg - 1 + dim)!		(deg + dim)
					____________  	=	________________	*	___________

					 deg! dim!			(deg - 1)! dim!			 deg
*/
					for (size_t deg = old_size; deg <= degree; ++deg)
						this->_size_map[deg] = (this->_size_map)[deg - 1] * (deg + dim) / deg;
				}
			}
			return (this->_size_map)[degree];
		};

	template <size_t dim>
		TensorialBasis<dim>::TensorialBasis() : AbstractBasis<dim>()
		{};

	template <size_t dim>
		void TensorialBasis<dim>::Copy(const TensorialBasis<dim>& basis)
		{
			AbstractBasis<dim>::Copy(basis);
			this->_tensorial_indexes = basis._tensorial_indexes;
		};

	template <size_t dim>
		void TensorialBasis<dim>::UpdateSize(size_t new_length)
		{
			size_t current_length = this->_tensorial_indexes.size();

			if (new_length > current_length)
			{
				size_t degree(0);

				while(this->ComputeSize(degree) <= current_length)
					++degree;
				size_t start_degree = degree;

				while(this->ComputeSize(degree) < new_length)
					++degree;

				this->_tensorial_indexes.resize(this->ComputeSize(degree));
				auto previous_end = this->_tensorial_indexes.begin() + current_length;
				for(size_t current_degree = start_degree; current_degree <= degree; ++current_degree)
				{
					Filler<dim>::FillIndexes(previous_end, current_degree);
				}
			}
		};

	template <size_t dim>
		Geometry::Vector TensorialBasis<dim>::EvaluateBasis(size_t degree, const Geometry::Point<dim>& p)
		{
			auto size = this->ComputeSize(degree);
			UpdateSize(size);
			Geometry::Vector result(size);
			for (size_t i = 0; i < size; ++i)
				result[i] = Evaluate(i, p);

			return result;
		};

	template <size_t dim>
		double TensorialBasis<dim>::Evaluate(size_t ind, const Geometry::Point<dim>& p)
		{
			UpdateSize(ind + 1);

			auto ind_iter = this->_tensorial_indexes[ind].begin();
			auto point_iter = p.begin();

			double result(1);

			for (size_t i(0); i < dim; ++i)
				result *= OneDEvaluation(*(ind_iter++), *(point_iter++));

			return result;
		};


	template <size_t dim>
		void TensorialBasis<dim>::PrintIndexes()
		{
			for (auto ind_tuple : this->_tensorial_indexes)
			{
				for (auto ind : ind_tuple)
					cout << ind << " ";
				cout << endl;
			}			
		};

	template <size_t dim>
		array<size_t, dim> TensorialBasis<dim>::GetIndexes(size_t ind)
		{
			UpdateSize(ind + 1);

			return (this->_tensorial_indexes)[ind];
		};

};//namespace FiniteElements

#endif //__BASIS__HH


