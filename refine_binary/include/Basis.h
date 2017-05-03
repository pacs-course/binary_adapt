#ifndef __BASIS__HH
#define __BASIS__HH

#include <memory> //unique_ptr
#include <stdexcept> //range_error

#include "AbstractFactory.h"
#include "TypeEnumerations.h"
#include "Point.h"

using namespace std;

namespace FiniteElements
{
	//TODO: maybe checks on the object to be effectively a basis (linearly independent and maximal)
	//have to be inserted in the construction or after it

	template <size_t dim> //working in |R ^ dim
		class AbstractBasis
		{
			protected:
				/* For degree 0 I always have that the basis is given by 1 element */
				AbstractBasis() : _size_map(0, 1){};
			public:
				/* This function implements the tensorization */
				virtual double Evaluate(size_t ind, const Geometry::Point<dim>& p);

				virtual vector<double> EvaluateBasis(size_t degree, const Geometry::Point<dim>& p);

				//size is (degree + dim)! / degree! dim!
				virtual size_t ComputeSize (const size_t& degree);			

			protected:
				virtual double OneDEvaluation(size_t index, double x)const = 0;

				virtual void InitializeIndexes(size_t* index, size_t degree, size_t d = dim);

				virtual void UpdateSize(size_t);

				void PrintIndexes();
			protected:
				/* _size_map[d] stores the size of an AbstractBasis<dim> of degree d */
				vector<size_t> _size_map;	

				vector<array<size_t, dim>> _tensorial_indexes;
		};

	template <size_t dim>
		vector<double> AbstractBasis<dim>::EvaluateBasis(size_t degree, const Geometry::Point<dim>& p)
		{
			UpdateSize(ComputeSize(degree));
			vector<double> result;
			for (size_t i(0); i < ComputeSize(degree); ++i)
				result.push_back(Evaluate(i, p));

			return result;
		};

	template <size_t dim>
		double AbstractBasis<dim>::Evaluate(size_t ind, const Geometry::Point<dim>& p)
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
		void AbstractBasis<dim>::UpdateSize(size_t new_length)
		{
			size_t current_length = this->_tensorial_indexes.size();

			if (new_length > current_length)
			{
				size_t degree(0);

				while(ComputeSize(degree) <= current_length)
					++degree;
				size_t start_degree = degree;

				while(ComputeSize(degree) < new_length)
					++degree;

				this->_tensorial_indexes.resize(ComputeSize(degree));
				for(size_t current_degree = start_degree; current_degree <= degree; ++current_degree)
					InitializeIndexes(&current_length, current_degree);
			}
		};

	/* recursive construction, here I set the dim-d component of the index tuple in position *index
		the update of the *index parameter will be done when d=1,
		that is after that the last element of the tuple has been set */
	template <size_t dim>
		void AbstractBasis<dim>::InitializeIndexes(size_t* index, size_t degree, size_t d)
		{
#ifdef MYDEBUG
			cout << "Sono in InitializeIndexes" << endl;

			cout << "Gli indici attuali sono : " << endl;
			PrintIndexes();
			cout << "Voglio il grado " << degree << " in posizione " << *index << endl; 
#endif //MYDEBUG

			/* end of recursion, here I update the index parameter*/
			if (d == 1)
			{
				(this->_tensorial_indexes)[(*index)++][dim-1] = degree;

#ifdef MYDEBUG
				cout << "Ho settato l'indice " << degree << " in posizione " << *index - 1 << endl;

				cout << "I nuovi indici sono : " << endl;
				PrintIndexes();
#endif //MYDEBUG
			}
			else
			{ 
				for(size_t current_degree = degree; current_degree + 1; --degree)
				{
					this->_tensorial_indexes[(*index)][dim-d] = current_degree;
					InitializeIndexes (index, degree - current_degree, d - 1);
				}
			}
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
				size_t old_size = this->_size_map.size();
				this->_size_map.resize(degree + 1);

				for (size_t deg = old_size; deg <= degree; ++deg)
				{
					if (dim == 1)
						this->_size_map[deg] = deg + 1;
					else
					{
						size_t dim_fact(1), degree_plus_dim(deg + 1);
						for (size_t i(2); i <= dim; ++i)
						{
							dim_fact *= i;
							degree_plus_dim *= (deg + i);
						}

						this->_size_map[deg] = degree_plus_dim / dim_fact;
					}
				}
			}
			return this->_size_map[degree];
		};

	template <size_t dim>
		void AbstractBasis<dim>::PrintIndexes()
		{
			for (auto ind_tuple : this->_tensorial_indexes)
			{
				for (auto ind : ind_tuple)
					cout << ind << " ";
				cout << endl;
			}			
		};

};//namespace FiniteElements

#endif //__BASIS__HH


