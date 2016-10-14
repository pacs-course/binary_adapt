#ifndef __POINT__HH
#define __POINT__HH

#include <iostream> //cout
#include <stdexcept> //invalid_argument
#include <string> //string
#include <array> //array

#include <point.h> //libmesh::point
namespace Geometry
{
	using namespace std;
	// A class that holds {DIM}D points, equally vectors

	//TODO: derive Point class from an Eigen type instead of std::array
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
				auto l = coor.size();
				if (l != DIM)
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

			Point(const array<double, DIM>& coor) : array<double, DIM> (coor){};

			//copy constructor
			Point (Point const& x)
			{
				(*this) = x;
			};

			//implicit constructor from libmesh::point
			//libmesh::point has 3 coordinates, so for DIM > 3 it does not make sense 
			template <size_t DUMMY = DIM, typename enable_if< (DUMMY < 4), size_t>::type = 0>
				Point(libMesh::Point& point)
				{
					size_t i(0);
					for (auto& iter : *this)
						iter = point(i++);
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

			double* data() const
			{
				return this->data();
			}

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
