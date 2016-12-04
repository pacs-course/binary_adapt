#ifndef __POLINOMIAL_H
#define __POLINOMIAL_H

#include <vector>
#include <exception> //runtime_error
#include <algorithm> //max
#include <type_traits> //enable_if


#include "Point.h"

using namespace Geometry;
using namespace std;

namespace FiniteElements
{
	template <size_t dim>
		class Polinomial
		{
			public:
	 
				Polinomial() : _degree(0), _degreeUpdated(true)
				{
					_oneDFactors.fill(Polinomial<1>());
				};

				Polinomial<dim> (const Polinomial<dim>& p)
				{
					*this = p;
				}

				Polinomial<dim>& operator = (const Polinomial<dim>& p)
				{
					if (&p != this)
					{
						for (size_t i = 0; i < dim; ++i)
							_oneDFactors[i] = p._oneDFactors[i];
						_degree = p._degree;
						_degreeUpdated = p._degreeUpdated;
					}
					return *this;
				}

				//polinomial product

				Polinomial<dim> operator * (const Polinomial<dim>& p)
				{
					Polinomial<dim> result ();
					for (size_t i = 0; i < dim; ++i)
						result.setFactor(i, _oneDFactors[i] * p[i]);

					return result;
				};

				const Polinomial<1>& operator[] (size_t ind) const
				{
					return _oneDFactors[ind];
				};

				Polinomial<1>& operator[] (size_t ind)
				{
					return _oneDFactors[ind];
				};

				typename array<Polinomial<1>, dim>::iterator begin()
				{
					return _oneDFactors.begin();
				};

				typename array<Polinomial<1>, dim>::iterator end()
				{
					return _oneDFactors.end();
				};

				typename array<Polinomial<1>, dim>::const_iterator begin() const
				{
					return _oneDFactors.begin();
				};

				typename array<Polinomial<1>, dim>::const_iterator end() const
				{
					return _oneDFactors.end();
				};

	/*
				I need the enable_if version since I need to fully specialize the Polinomial class with dim = 1
				so I cannot specialize this single method without specializing the full class
	*/
				template <size_t n, typename enable_if< (n > 1), int>::type = 0>
					Polinomial<dim + n> tensor (const Polinomial<n>& p) const
					{
						Polinomial<dim + n> result;
						size_t i(0);
						for (const auto& pol1 : _oneDFactors)
							result.setFactor(i++, pol1);
	
						for (const auto& pol2 : p)
							result.setFactor(i++, pol2);

						return result;
					}
			
				template <size_t n, typename enable_if< (n == 1), int>::type = 0>
					Polinomial<dim + n> tensor (const Polinomial<n>& p) const
					{
						Polinomial<dim + n> result;
						size_t i(0);
						for (const auto& pol1 : _oneDFactors)
							result.setFactor(i++, pol1);
	
						result.setFactor(i, p);
						return result;
					}
			

				//polinomial evaluation
				double operator() (Point<dim> const& x) const
				{
					double tot = 1;
					for (size_t i = 0; i < dim; ++i)
						tot *= _oneDFactors[i](x[i]);
					return tot;
				};


				size_t degree() const
				{
					if (!_degreeUpdated)
						updateDegree();
					return _degree;
				};

				void print(bool verbose = true) const
				{
					if (verbose)
					{
						cout << "(";
						switch(dim)
						{
							case(1):
								_oneDFactors[0].print(verbose, "x", false);
								break;

							case(2):
								_oneDFactors[0].print(verbose, "x", false);
								cout << ") * (";
								_oneDFactors[1].print(verbose, "y", false);
								break;

							case(3):
								_oneDFactors[0].print(verbose, "x", false);
								cout << ") * (";
								_oneDFactors[1].print(verbose, "y", false);
								cout << ") * (";
								_oneDFactors[2].print(verbose, "z", false);
								break;

							default:
								size_t i(0);
								for(const auto& pol : _oneDFactors)
								{
									pol.print(verbose, "x" + to_string(++i), false);
									if (i != _oneDFactors.size())
										cout << ") * (";
								}
								break;
						}
						cout << ")" << endl;	
					}
					else
					{
						cout << "Polinomio " << dim << "D di fattori:" << endl;
						for (const auto& pol : _oneDFactors)
							pol.print(verbose);
						cout << endl;
					}
				};

			public:
				void setFactor(size_t ind, const Polinomial<1>& f)
				{
					_oneDFactors[ind] = f;
					_degreeUpdated = false;
				};

				bool null()
				{
					for (const auto& pol : _oneDFactors)
						if (pol.empty())
							return true;

					return false;
				}

			private:
				void updateDegree()
				{
					size_t d(0);
					for (const auto& pol : _oneDFactors)
						d += pol.degree();

					_degree = d;
					_degreeUpdated = true;
				}

	/*
	multivariate polinomial of dim variables is stored as a product of univariate polinomial; these are stored in a dim sized array
	*/
				array<Polinomial<1>, dim> _oneDFactors;

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
				//destructor
				~Polinomial() = default;
				Polinomial (const Polinomial<1>& p);
				Polinomial<1>& operator = (const Polinomial<1>& p);

				//1D polinomial product by scalar
				Polinomial<1> operator * (const double&) const;
				//1D polinomial division by scalar
				Polinomial<1> operator / (const double&) const;
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
				Polinomial<1>& operator /= (const double&);

			
				//methods to access coefficient value of x^ind term
				double& 			operator[] 	(size_t ind)		;
				const double& 	operator[] 	(size_t ind) const;
				double  			get 			(size_t ind) const;

				//evaluate polinomial in input point
				double operator() (Point<1> const&) const;
				double operator() (double const& ) 	const;

				//polinomial degree
				size_t degree() const;

				bool null() const;

			template <size_t new_dim>
				typename enable_if< !(new_dim < 1), Polinomial<new_dim> >::type
					extendVariables (size_t my_position = 0)
					{
						vector<double> coeff (degree(),0);
						coeff[0] = 1;
 						Polinomial<1> constantOne(coeff);
						Polinomial<new_dim> result;
						for (size_t i = 0; i < new_dim; ++i)
							i != my_position ? result.setFactor(i, constantOne) : result.setFactor(i, *this);
					
						return result;
					};

			template <size_t N, typename enable_if< (N > 1), int>::type = 0>
				Polinomial<1 + N> tensor (const Polinomial<N>& p) const
				{
					Polinomial<1 + N> result;
					size_t i(0);
					result.setFactor (i++, *this);

					for (const auto& pol2 : p)
						result.setFactor (i++, pol2);

					return result;
				}
		
			template <size_t N, typename enable_if< (N == 1), int>::type = 0>
				Polinomial<1 + N> tensor (const Polinomial<N>& p) const
				{
					Polinomial<1 + N> result;
					result.setFactor(0, *this);
					result.setFactor(1, p);

					return result;
				}

			vector<double>::iterator begin();
			vector<double>::iterator end();
			vector<double>::const_iterator begin() const;
			vector<double>::const_iterator end() const;

			void print(bool verbose = true, string = "x", bool new_line = true)	const;

			protected:
				void degree(size_t val);
			private:
				vector<double> _coeff;
		};

} //namespace FiniteElements
#endif //__POLINOMIAL_H

