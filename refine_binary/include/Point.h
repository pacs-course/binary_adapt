#ifndef __POINT__HH
#define __POINT__HH

#include <iostream> //cout
#include <stdexcept> //invalid_argument
#include <string> //string
#include <array> //array
#include <math.h> //sqrt, abs

#include <Eigen/Dense>

namespace Geometry
{
	using namespace std;

	template <size_t dim>
	using EigVector = Eigen::Matrix<double, static_cast<int>(dim), 1>;

	/* A class that holds {dim}D points, equally vectors
		When used as vectors, they are meant as column vector
		The Point class is derived from Eigen one for efficiency reasons
	*/
	template <size_t dim>
		class Point : public EigVector<dim>
		{
			public:
				using iterator = double*;
				using const_iterator = const double*;

				iterator begin()
				{
					return this->data();
				};

				iterator end()
				{
					return this->data() + dim;
				};

				const_iterator begin()const
				{
					return this->data();
				};

				const_iterator end()const
				{
					return this->data() + dim;
				};

				Point(double value = 0.0) : EigVector<dim>()
				{
					for (size_t i = 0; i < dim; ++i)
						(*this)[i] = value;
				};

				Point(const initializer_list<double>& coor) : EigVector<dim>()
				{
					auto l = coor.size();
					if (l != dim)
					{
						throw invalid_argument("Invalid number of elements in Geometry::Point construction");
					}
					else
					{
						size_t i(0);
						for (const auto& val : coor)
							(*this)[i++] = val;
					}
				};

				/* copy constructor */
				Point (const Point& x) : EigVector<dim>(x){};

				/* copy constructor */
				Point (const EigVector<dim>& x) : EigVector<dim>(x){};

				/* assignment operator */
				Point& operator= (const Point& x)
				{
					EigVector<dim>::operator= (x);
					return (*this);
				};


				/* for a 1D point I implement the implicit cast to double */
				template <size_t dummy = dim, typename enable_if< (dummy == 1), int>::type = 0>
				operator double() const
				{
					return (*this)[0];
				};

				/* implicit cast to std::array */
				explicit operator array<double, dim> () const
				{
					array<double, dim> result;
					for (size_t i = 0; i < dim; ++i)
						result[i] = (*this)[i];
					return result;
				};

				/* Product by scalar */
				Point<dim> operator * (double alfa) const
				{
					return Point<dim>(EigVector<dim>::operator* (alfa));
				};

//				/* Vector difference */
				Point<dim> operator - (const Point<dim>& x) const
				{
					return Point<dim>(EigVector<dim>::operator- (x));
				};

				// Vector sum
				Point<dim> operator + (const Point<dim>& x) const
				{
					return Point<dim>(EigVector<dim>::operator+ (x));
				};


				//tensor product
				template <size_t N>
					Point<dim + N> Tensor (const Point<N>& p2) const
					{
						Point<dim + N> result;

						for (size_t i = 0; i < dim; ++i)
							result[i] = (*this)[i];

						for (size_t j = 0; j < N; ++j)
							result[dim + j] = p2[j];

						return result;
					}

				// Modulus of the vector
				double Abs() const
				{
					double result = this->dot(*this);
					return sqrt(result);
				};

				// Euclidean Distance from input vector
				double distance (Point<dim> const& x) const
				{
					return (Point<dim>(EigVector<dim>::operator- (x))).Abs();
				};

				//friend to enable cout << p
				friend ostream& operator<< (ostream& output, const Point<dim>& p)
				{
					output << "(" << p[0];
					for (size_t i = 1; i < dim; ++i)
						output << "," << p[i];
					output << ")";
					return output;
				}

				void Print(bool newline = true) const
				{
					cout << *this;
					if(newline)
						cout << endl;
				}
		};


	//It must be castable to Point<dim>
	template <size_t dim>
	using VecType = Point<dim>;

	template <size_t dim>
	using MatrixType = Eigen::Matrix <double, static_cast<int>(dim), static_cast<int>(dim)>;

	using ColumnVector = Eigen::Matrix <double, Eigen::Dynamic, 1>;
}
#endif //__POINT__HH
