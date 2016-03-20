#ifndef __POLY__HH
#define __POLY__HH

#include <vector>
#include <exception> //runtime_error
#include <algorithm> //max
#include <type_traits> //enable_if

//#include "Tensorize.h"
#include "Point.h"

using namespace Geometry;
using namespace std;


template <size_t DIM>
	class Polinomial
	{
		public:
 
			Polinomial() : _degree(0), _degreeUpdated(true)
			{
				_oneDFactors.fill(Polinomial<1>());
			};

			Polinomial<DIM> (const Polinomial<DIM>& p)
			{
				*this = p;
			}

			Polinomial<DIM>& operator = (const Polinomial<DIM>& p)
			{
				if (&p != this)
				{
					for (size_t i = 0; i < DIM; ++i)
						_oneDFactors[i] = p._oneDFactors[i];
					_degree = p._degree;
					_degreeUpdated = p._degreeUpdated;
				}
				return *this;
			}

			//polinomial product

			Polinomial<DIM> operator * (const Polinomial<DIM>& p)
			{
				Polinomial<DIM> result ();
				for (size_t i = 0; i < DIM; ++i)
					result.setFactor(i, _oneDFactors[i] * p[i]);

				return result;
			};

			Polinomial<1>& operator[] (size_t ind)
			{
				return _oneDFactors[ind];
			};

			typename array<Polinomial<1>, DIM>::iterator begin()
			{
				return _oneDFactors.begin();
			};

			typename array<Polinomial<1>, DIM>::iterator end()
			{
				return _oneDFactors.end();
			};

			template <size_t N, typename enable_if< (N > 1), int>::type = 0>
				Polinomial<DIM + N> tensor (const Polinomial<N>& p)
				{
					Polinomial<DIM + N> result;
					size_t i(0);
					for (auto pol1 : _oneDFactors)
						result.setFactor(i++, pol1);
	
					for (auto pol2 : p)
						result.setFactor(i++, pol2);

					return result;
				}
			
			template <size_t N, typename enable_if< (N == 1), int>::type = 0>
				Polinomial<DIM + N> tensor (const Polinomial<N>& p)
				{
					Polinomial<DIM + N> result;
					size_t i(0);
					for (auto pol1 : _oneDFactors)
						result.setFactor(i++, pol1);
	
					result.setFactor(i, p);
					return result;
				}
			

			//polinomial evaluation
			double operator() (Point<DIM> const& x) const
			{
				double tot = 1;
				for (size_t i = 0; i < DIM; ++i)
					tot *= _oneDFactors[i](x[i]);
				return tot;
			};


			size_t degree() const
			{
				if (!_degreeUpdated)
					updateDegree();
				return _degree;
			};

			void print() const
			{
				cout << "Polinomio " << DIM << "D di fattori:" << endl;
				for (auto pol : _oneDFactors)
					pol.print();
				cout << endl;
			};

		public:
			void setFactor(size_t ind, Polinomial<1> const& f)
			{
				_oneDFactors[ind] = f;
				_degreeUpdated = false;
			};

		private:
			void updateDegree()
			{
				size_t d(0);
				for (auto pol : _oneDFactors)
					d += pol.degree();

				_degree = d;
				_degreeUpdated = true;
			}

			bool null()
			{
				for (auto pol : _oneDFactors)
					if (pol.empty())
						return true;

				return false;
			}

/*
multivariate polinomial of DIM variables is stored as a product of univariate polinomial; these are stored in a DIM sized array
*/
			array<Polinomial<1>, DIM> _oneDFactors;

/*
degree evaluation is computed at runtime every time _degreeUpdated is false
every method that modifies polinomial coefficients must also set _degreeUpdated=false
*/
			size_t _degree;
			bool _degreeUpdated;
	};


	template <>
		class Polinomial<1>
		{
			public:
				Polinomial ();
				//constructor to initialize coefficients size
				Polinomial (size_t);
				//constructor to set degree and constant coefficients
				Polinomial (size_t, double);
				//constructor to initialize coefficients value
				Polinomial (const vector<double>&);

				Polinomial (const Polinomial<1>& p);
				Polinomial<1>& operator = (const Polinomial<1>& p);

				//1D polinomial product
				Polinomial<1> operator * (const double&) const;
				//1D polinomial product
				Polinomial<1> operator * (const Polinomial<1>&) const;
				//1D polinomial sum
				Polinomial<1> operator + (const Polinomial<1>&) const;
				//1D polinomial difference
				Polinomial<1> operator - (const Polinomial<1>&) const;

				Polinomial<1>& operator -= (const Polinomial<1>&);
				Polinomial<1>& operator += (const Polinomial<1>&);
				Polinomial<1>& operator *= (const Polinomial<1>&);
				Polinomial<1>& operator *= (const double&);

				
				//methods to access coefficient value of x^ind term
				double& operator[] (size_t ind);
				double  get (size_t ind) const;

				//evaluate polinomial in input point
				double operator() (Point<1> const&) const;
				double operator() (double const& ) 	const;

				//polinomial degree
				size_t degree() const;

				bool null() const;

			template <size_t NEW_DIM>
				typename enable_if< !(NEW_DIM < 1), Polinomial<NEW_DIM> >::type
					extendVariables (size_t my_position = 0)
					{
						vector<double> coeff (degree(),0);
						coeff[0] = 1;
 						Polinomial<1> constantOne(coeff);
						Polinomial<NEW_DIM> result;
						for (size_t i = 0; i < NEW_DIM; ++i)
							i != my_position ? result.setFactor(i, constantOne) : result.setFactor(i, *this);
						
						return result;
					};

			template <size_t N, typename enable_if< (N > 1), int>::type = 0>
				Polinomial<1 + N> tensor (const Polinomial<N>& p)
				{
					Polinomial<1 + N> result;
					size_t i(0);
					result.setFactor (i++, *this);
	
					for (auto pol2 : p._oneDFactors)
						result.setFactor (i++, pol2);

					return result;
				}
			
			template <size_t N, typename enable_if< (N == 1), int>::type = 0>
				Polinomial<1 + N> tensor (const Polinomial<N>& p)
				{
					Polinomial<1 + N> result;
					result.setFactor(0, *this);
					result.setFactor(1, p);

					return result;
				}

			vector<double>::iterator begin();
			vector<double>::iterator end();

			void print() const;

			protected:
				void degree(size_t val);
			private:
				vector<double> _coeff;
		};


#endif //__POLY__HH
