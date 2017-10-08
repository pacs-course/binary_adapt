#ifndef __BASIS__HH
#define __BASIS__HH

#include <memory> //unique_ptr
#include <stdexcept> //range_error

#include "AbstractFactory.h"
#include "TypeEnumerations.h"
#include "Point.h"


namespace FiniteElements
{
	using namespace std;

//	template <size_t d, class iterator>
//		struct Filler
//		{
//			static void FillIndexes(iterator, size_t);
//		};

//	template <class iterator>
//		struct Filler<1, iterator>
//		{
//			static void FillIndexes(iterator, size_t);
//		};


	//TODO: maybe check the object to be effectively a basis (linearly independent and maximal)
	//have to be inserted in the construction or after it

	template <size_t dim> //working in |R ^ dim
		class AbstractBasis
		{
			protected:
				/* For degree 0 I always have that the basis is given by 1 element */
				AbstractBasis() : _size_map(0, 1){};
				/* To be called by assignment operator in derived classes */
				void Copy(const AbstractBasis<dim>& basis);

			public:
				virtual double Evaluate(size_t ind, const Geometry::Point<dim>& p) = 0;
				virtual vector<double> EvaluateBasis(size_t degree, const Geometry::Point<dim>& p) = 0;

				/* Size is (degree + dim)! / degree! dim! */
				virtual size_t ComputeSize (const size_t& degree);

			protected:
				/* _size_map[d] stores the size of an AbstractBasis<dim> of degree d */
				vector<size_t> _size_map;
		};

	template <size_t dim>
		class TensorialBasis : public AbstractBasis<dim>
		{
			protected:
				TensorialBasis() : AbstractBasis<dim>(){};
				void Copy(const TensorialBasis<dim>& basis);

			public:
				/* This function implements the tensorization */
				virtual double Evaluate(size_t ind, const Geometry::Point<dim>& p) override;
				virtual vector<double> EvaluateBasis(size_t degree, const Geometry::Point<dim>& p) override;

			protected:
				virtual double OneDEvaluation(size_t index, double x)const = 0;

				virtual void UpdateSize(size_t);
				virtual void InitializeIndexes(size_t* index, size_t degree, size_t d = dim);
				void PrintIndexes();

			protected:
				vector<array<size_t, dim>> _tensorial_indexes;
		};

	template <size_t dim>
		void AbstractBasis<dim>::Copy(const AbstractBasis<dim>& basis)
		{
			this->_size_map = basis._size_map;
		};

	template <size_t dim>
		void TensorialBasis<dim>::Copy(const TensorialBasis<dim>& basis)
		{
			AbstractBasis<dim>::Copy(basis);
			this->_tensorial_indexes = basis._tensorial_indexes;
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
				for(size_t current_degree = start_degree; current_degree <= degree; ++current_degree)
				{
					typename vector<array<size_t, dim>>::iterator iter = this->_tensorial_indexes.begin() + current_length;
//					Filler<dim, typename vector<array<size_t, dim>>::iterator>::FillIndexes(iter, current_degree);
					InitializeIndexes(&current_length, current_degree);
				}
			}
		};

	template <size_t dim>
		vector<double> TensorialBasis<dim>::EvaluateBasis(size_t degree, const Geometry::Point<dim>& p)
		{
			auto size = this->ComputeSize(degree);
			UpdateSize(size);
			vector<double> result;
			for (size_t i(0); i < size; ++i)
				result.push_back(Evaluate(i, p));

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


	/* recursive construction, here I set the dim-d component of the index tuple in position *index
		the update of the *index parameter will be done when d=1,
		that is after that the last element of the tuple has been set */
	template <size_t dim>
		void TensorialBasis<dim>::InitializeIndexes(size_t* index, size_t degree, size_t d)
		{
#ifdef MYDEBUG
			clog << "Sono in InitializeIndexes" << endl;

			clog << "Gli indici attuali sono : " << endl;
			PrintIndexes();
			clog << "Index: " << *index << endl;
			clog << "Degree: " << degree << endl;
			clog << "Dim: " << d << endl; 
#endif //MYDEBUG

			/* end of recursion, here I update the index parameter*/
			if (d == 1)
			{
				(this->_tensorial_indexes)[(*index)++][0] = degree;

#ifdef MYDEBUG
				cout << "Ho settato l'indice " << degree << " in posizione " << *index - 1 << endl;
				cout << "I nuovi indici sono : " << endl;
				PrintIndexes();
#endif //MYDEBUG
			}
			else
			{
				for(size_t current_degree = 0; current_degree <= degree; ++current_degree)
				{
					this->_tensorial_indexes[(*index)][d-1] = current_degree;
					auto residual_degree = degree - current_degree;
					InitializeIndexes (index, residual_degree, d - 1);
				}
			}
		};


//	template <size_t d, class iterator>
//		void Filler<d, iterator>::FillIndexes(iterator iter, size_t degree)
//		{
//			for (size_t ind = 0; ind <= degree; ++ind)
//			{
//				(*iter)[d - 1] = ind;
//				auto residual = degree - ind;
//				Filler<d-1, iterator>::FillIndexes(iter, residual);
//			}
//		};

//	template <class iterator>
//		void Filler<1, iterator>::FillIndexes (iterator iter, size_t degree)
//		{
//			(*iter)[0] = degree;
//			++iter;
//		};


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

};//namespace FiniteElements

#endif //__BASIS__HH


