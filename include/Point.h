#ifndef __POINT__HH
#define __POINT__HH

namespace Geometry
{
	using namespace std;
	// A class that holds {DIM}D points, equally vectors
	 
	template <size_t DIM>
	 class Point : array<double,DIM>
	 {
	  public:
		// Constructor giving coordinates.

	//QUESTO NON FUNZIONA
		Point(initializer_list<double> coor) : array<double,DIM>(coor){};
		Point(array<double,DIM> coor) : array<double,DIM>(coor){};
		Point<DIM> operator () (Point<DIM> const& x) = default;
		Point<DIM> operator = (Point<DIM> const& x) = default;
	
		double& operator [] (size_t i)
		{
			return (*this)[i];
		};
		// Returns coordinates in a array<double>.
		array<double,DIM> get() const
		{
			return *this;
		};

		// Scalar multiplication is implemented as an external friend function
		Point<DIM> operator * (double& alfa)
		{
			array<double, DIM> temp();
			for (auto iter : x)
				temp[i] = alfa * iter;
			return Point<DIM>(temp);
		};

		// Subtraction is implemented as an external friend function
		Point<DIM> operator - (Point<DIM> const& x);
		{
			array<double, DIM> temp();
			for (size_t i=0; i < DIM; ++i)
				temp[i] = (*this)[i] - x[i];
			return Point<DIM>(temp);
		};

		// Addition is implemented as an external friend function
		Point<DIM> operator + (Point<DIM> const& x)
		{
			array<double, DIM> temp();
			for (size_t i=0; i < DIM; ++i)
				temp[i] = x[i] + (*this)[i];
			return Point<DIM>(temp);
		};
		// Modulus of the vector
		double abs()
		{
			double result(0);
			for (auto iter : (*this))
				result += iter * iter;
			return sqrt(result);
		};

		double distance (Point<DIM> const& x)
		{
			return abs (*(this) - x);
		};
	};

	template <size_t DIM>
		using Vector = Point<DIM>;
}
#endif //__POINT__HH
