#ifndef __POLINOMIAL_H
#define __POLINOMIAL_H

#include <vector>
#include <exception> //runtime_error
#include <algorithm> //max
#include <type_traits> //enable_if


#include "Point.h"


namespace FiniteElements
{
	using namespace Geometry;
	using namespace std;

	template <size_t dim>
		class Polinomial
		{
			public:
	 
				Polinomial() : _degree(0), _degree_updated(true)
				{
					_one_d_factors.fill(Polinomial<1>());
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
							_one_d_factors[i] = p._one_d_factors[i];
						_degree = p._degree;
						_degree_updated = p._degree_updated;
					}
					return *this;
				}

				//polinomial product
				Polinomial<dim> operator * (const Polinomial<dim>& p)
				{
					Polinomial<dim> result;
					for (size_t i = 0; i < dim; ++i)
						result.SetFactor(i, this->_one_d_factors[i] * p[i]);

					return result;
				};

				const Polinomial<1>& operator[] (size_t ind) const
				{
					return this->_one_d_factors[ind];
				};

				Polinomial<1>& operator[] (size_t ind)
				{
					return this->_one_d_factors[ind];
				};

				typename array<Polinomial<1>, dim>::iterator begin()
				{
					return this->_one_d_factors.begin();
				};

				typename array<Polinomial<1>, dim>::iterator end()
				{
					return this->_one_d_factors.end();
				};

				typename array<Polinomial<1>, dim>::const_iterator begin() const
				{
					return this->_one_d_factors.begin();
				};

				typename array<Polinomial<1>, dim>::const_iterator end() const
				{
					return this->_one_d_factors.end();
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
						for (const auto& pol1 : this->_one_d_factors)
							result.SetFactor(i++, pol1);
	
						for (const auto& pol2 : p)
							result.SetFactor(i++, pol2);

						return result;
					}
			
				template <size_t n, typename enable_if< (n == 1), int>::type = 0>
					Polinomial<dim + n> tensor (const Polinomial<n>& p) const
					{
						Polinomial<dim + n> result;
						size_t i(0);
						for (const auto& pol1 : _one_d_factors)
							result.SetFactor(i++, pol1);
	
						result.SetFactor(i, p);
						return result;
					}
			

				//polinomial evaluation
				double operator() (Point<dim> const& x) const
				{
					double tot = 1;
					for (size_t i = 0; i < dim; ++i)
						tot *= this->_one_d_factors[i](x[i]);
					return tot;
				};


				size_t Degree() const
				{
					if (!_degree_updated)
						UpdateDegree();
					return this->_degree;
				};

				void Print(bool verbose = true) const
				{
					if (verbose)
					{
						cout << "(";
						switch(dim)
						{
							case(1):
								this->_one_d_factors[0].Print(verbose, "x", false);
								break;

							case(2):
								this->_one_d_factors[0].Print(verbose, "x", false);
								cout << ") * (";
								this->_one_d_factors[1].Print(verbose, "y", false);
								break;

							case(3):
								this->_one_d_factors[0].Print(verbose, "x", false);
								cout << ") * (";
								this->_one_d_factors[1].Print(verbose, "y", false);
								cout << ") * (";
								this->_one_d_factors[2].Print(verbose, "z", false);
								break;

							default:
								size_t i(0);
								for(const auto& pol : _one_d_factors)
								{
									pol.Print(verbose, "x" + to_string(++i), false);
									if (i != this->_one_d_factors.size())
										cout << ") * (";
								}
								break;
						}
						cout << ")" << endl;	
					}
					else
					{
						cout << "Polinomio " << dim << "D di fattori:" << endl;
						for (const auto& pol : this->_one_d_factors)
							pol.Print(verbose);
						cout << endl;
					}
				};

			public:
				void SetFactor(size_t ind, const Polinomial<1>& f)
				{
					this->_one_d_factors[ind] = f;
					this->_degree_updated = false;
				};

				bool Null()
				{
					for (const auto& pol : this->_one_d_factors)
						if (pol.empty())
							return true;

					return false;
				}

			private:
				void UpdateDegree()
				{
					size_t d(0);
					for (const auto& pol : this->_one_d_factors)
						d += pol.Degree();

					this->_degree = d;
					this->_degree_updated = true;
				}

	/*
	multivariate polinomial of dim variables is stored as a product of univariate polinomial; these are stored in a dim sized array
	*/
				array<Polinomial<1>, dim> _one_d_factors;

	/*
	degree evaluation is computed at runtime every time _degree_updated is false
	every method that modifies polinomial coefficients must also set _degree_updated=false
	*/
				size_t _degree;
				bool _degree_updated;
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
				size_t Degree() const;

				bool Null() const;

			template <size_t new_dim>
				typename enable_if< !(new_dim < 1), Polinomial<new_dim> >::type
				ExtendVariables (size_t my_position = 0)
				{
					vector<double> coeff (Degree(),0);
					coeff[0] = 1;
					Polinomial<1> constant_one(coeff);
					Polinomial<new_dim> result;
					for (size_t i = 0; i < new_dim; ++i)
						i != my_position ? result.SetFactor(i, constant_one) : result.SetFactor(i, *this);
			
					return result;
				};

			template <size_t N, typename enable_if< (N > 1), int>::type = 0>
				Polinomial<1 + N> tensor (const Polinomial<N>& p) const
				{
					Polinomial<1 + N> result;
					size_t i(0);
					result.SetFactor (i++, *this);

					for (const auto& pol2 : p)
						result.SetFactor (i++, pol2);

					return result;
				};
	
			template <size_t N, typename enable_if< (N == 1), int>::type = 0>
				Polinomial<1 + N> tensor (const Polinomial<N>& p) const
				{
					Polinomial<1 + N> result;
					result.SetFactor(0, *this);
					result.SetFactor(1, p);

					return result;
				};

			vector<double>::iterator begin();
			vector<double>::iterator end();
			vector<double>::const_iterator begin() const;
			vector<double>::const_iterator end() const;

			void Print(bool verbose = true, string = "x", bool new_line = true)	const;

			protected:
				void Degree(size_t val);
			private:
				vector<double> _coeff;
		};

} //namespace FiniteElements
#endif //__POLINOMIAL_H

