#ifndef __POINT__HH
#define __POINT__HH

#include <iostream> //cout
#include <stdexcept> //invalid_argument
#include <string> //string
#include <array> //array

namespace Geometry
{
	using namespace std;
	// A class that holds {DIM}D points, equally vectors
	
	template <size_t DIM>
		class Point : public array<double,DIM>
		{
			public:
			Point()
			{
				this->fill(0.0);
			}

			Point(const initializer_list<double>& coor)
			{
				if (coor.size() != DIM)
				{
					string ss("Invalid number of elements in Geometry::Point construction");
					throw invalid_argument(ss);
				}
				else
				{
					size_t i(0);
					for (const auto& val : coor)
						(*this)[i++] = val;
				}
			};

			Point(const array<double, DIM>& coor) : array<double, DIM> (coor){};

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
			array<double,DIM> get() const
			{
				return *this;
			};

			// Product by scalar
			Point<DIM> operator * (double& alfa) const
			{
				array<double, DIM> temp;
				size_t i(0);
				for (const auto& iter : (*this))
					temp[i++] = alfa * iter;
				return Point<DIM>(temp);
			};

			// Vector difference
			Point<DIM> operator - (Point<DIM> const& x) const
			{
				array<double, DIM> temp;
				for (size_t i=0; i < DIM; ++i)
					temp[i] = (*this)[i] - x[i];
				return Point<DIM>(temp);
			};

			// Vector sum
			Point<DIM> operator + (Point<DIM> const& x) const
			{
				array<double, DIM> temp;
				for (size_t i=0; i < DIM; ++i)
					temp[i] = x[i] + (*this)[i];
				return Point<DIM>(temp);
			};

			//tensor product
			template <size_t N>
				Point<DIM + N> tensor (const Point<N>& p2) const
				{
					array<double, DIM + N> coor;
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
			double distance (Point<DIM> const& x) const
			{
				return (*(this) - x).abs();
			};

			void print() const
			{
				cout << "(" << (*this)[0];
				for (size_t i = 1; i < DIM; ++i)
					cout << "," << (*this)[i];
				cout << ")" << endl;
			}
		};

	template <size_t DIM>
		using Vector = Point<DIM>;
}
#endif //__POINT__HH
