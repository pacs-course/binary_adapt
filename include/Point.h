#ifndef __POINT__HH
#define __POINT__HH

#include <iostream> //cout
#include <stdexcept> //invalid_argument
#include <string> //string
#include <array> //array

#include <math.h> //sqrt, abs

namespace Geometry
{
	using namespace std;
	// A class that holds {dim}D points, equally vectors

	//TODO: derive Point class from an Eigen type instead of std::array
	template <size_t dim>
		class Point : public array<double,dim>
		{
			public:
			Point()
			{
				this->fill(0.0);
			}

			Point(const initializer_list<double>& coor)
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

			Point(const array<double, dim>& coor) : array<double, dim> (coor){};

			//copy constructor
			Point (Point const& x)
			{
				(*this) = x;
			};

			//assignment operator
			Point& operator = (Point const& x)
			{
				if (&x != this)
					copy(x.begin(), x.end(), this->begin());
				return (*this);
			};

			// Returns coordinates in a array<double>.
			array<double,dim> get() const
			{
				return *this;
			};

			// Product by scalar
			Point<dim> operator * (double& alfa) const
			{
				array<double, dim> temp;
				size_t i(0);
				for (const auto& iter : (*this))
					temp[i++] = alfa * iter;
				return Point<dim>(temp);
			};

			// Vector difference
			Point<dim> operator - (Point<dim> const& x) const
			{
				array<double, dim> temp;
				for (size_t i=0; i < dim; ++i)
					temp[i] = (*this)[i] - x[i];
				return Point<dim>(temp);
			};

			// Vector sum
			Point<dim> operator + (Point<dim> const& x) const
			{
				array<double, dim> temp;
				for (size_t i=0; i < dim; ++i)
					temp[i] = x[i] + (*this)[i];
				return Point<dim>(temp);
			};

			//tensor product
			template <size_t N>
				Point<dim + N> tensor (const Point<N>& p2) const
				{
					array<double, dim + N> coor;
					size_t i(0);
					for (auto val : *this)
						coor[i++] = val;

					for (auto val2 : p2.get())
						coor[i++] = val2;

					return coor;
				}

			// Modulus of the vector
			double abs() const
			{
				double result(0);
				for (const auto& iter : (*this))
					result += iter * iter;
				return sqrt(result);
			};

			// Euclidean Distance from input vector
			double distance (Point<dim> const& x) const
			{
				return (*(this) - x).abs();
			};

			void print(bool newline = true) const
			{
				cout << "(" << (*this)[0];
				for (size_t i = 1; i < dim; ++i)
					cout << "," << (*this)[i];
				cout << ")";
				if(newline)
					cout << endl;
			}
		};

	template <size_t dim>
		using Vector = Point<dim>;
}
#endif //__POINT__HH
