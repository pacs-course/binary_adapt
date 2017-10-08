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

	template <size_t dim>
	using EigMatrix = Eigen::Matrix <double, static_cast<int>(dim), static_cast<int>(dim)>;

	template <size_t dim>
		class Point;
	template <size_t dim>
		class Matrix;

	template <size_t dim>
	using VecType = Point<dim>;

	template <size_t dim>
	using MatrixType = Matrix<dim>;

	using ColumnVector = Eigen::Matrix <double, Eigen::Dynamic, 1>;

	/* A class that holds {dim}D points, equally vectors
		When used as vectors, they are meant as column vector
		The Point class is derived from Eigen one for efficiency reasons
	*/
	template <size_t dim>
		class Point
		{
			public:
				using iterator = double*;
				using const_iterator = const double*;

				iterator begin();
				iterator end();
				const_iterator begin()const;
				const_iterator end()const;

				Point(double value = 0.0);

				Point(const initializer_list<double>& coor);

				/* copy constructor */
				Point (const Point& x);

				/* assignment operator */
				Point& operator= (const Point& x);

				/* for a 1D point I implement the implicit cast to double */
				template <size_t dummy = dim, typename enable_if< (dummy == 1), int>::type = 0>
				operator double() const;

				/* explicit cast to std::array */
				explicit operator array<double, dim> () const;

				/* Product by scalar */
				Point<dim> operator * (double alfa) const;

				Point<dim>& operator *= (double alfa);

				/* Vector difference */
				Point<dim> operator - (const Point<dim>& x) const;

				/* Vector sum */
				Point<dim> operator + (const Point<dim>& x) const;

				double& operator[] (size_t ind);

				const double& operator[] (size_t ind)const;

				//tensor product
				template <size_t N>
					Point<dim + N> Tensor (const Point<N>& p2) const;

				/* Modulus of the vector */
				double Abs() const;

				/* Euclidean distance from input vector */
				double distance (Point<dim> const& x) const;

				//friend to enable cout << p
				friend ostream& operator<< (ostream& output, const Point<dim>& p)
				{
					output << "(" << p[0];
					for (size_t i = 1; i < dim; ++i)
						output << "," << p[i];
					output << ")";
					return output;
				};

				void Print(bool newline = true) const;

				template <size_t N>
				friend Point<N> operator* (const Matrix<N>&, const Point<N>&);

			protected:
				/* constructor from EigVector type */
				Point(const EigVector<dim>& v) : _vec(v){};
			protected:
				EigVector<dim> _vec;
		};


	template <size_t dim>
		class Matrix
		{
			public:
				/* constructor */
				Matrix(){};

				/* copy constructor */
				Matrix(const Matrix&);

				/* assignment operator */
				Matrix& operator= (const Matrix&);

				double& operator() (size_t, size_t);

				const double& operator() (size_t, size_t) const;

				template <size_t dummy = dim, typename enable_if< (dummy == 1), int>::type = 0>
				double& operator() (size_t);

				template <size_t dummy = dim, typename enable_if< (dummy == 1), int>::type = 0>
				const double& operator() (size_t) const;

				double Determinant() const;
				Matrix Inverse()const;

				template <size_t N>
				friend Point<N> operator* (const Matrix<N>&, const Point<N>&);

			protected:
				/* constructor from EigMatrix */
				Matrix(const EigMatrix<dim>& m) : _mat(m){};
			protected:
				EigMatrix<dim> _mat;
		};

	template <size_t dim>	
		typename Point<dim>::iterator Point<dim>::begin()
		{
			return _vec.data();
		};

	template <size_t dim>	
		typename Point<dim>::iterator Point<dim>::end()
		{
			return _vec.data() + dim;
		};

	template <size_t dim>	
		typename Point<dim>::const_iterator Point<dim>::begin()const
		{
			return _vec.data();
		};

	template <size_t dim>	
		typename Point<dim>::const_iterator Point<dim>::end()const
		{
			return _vec.data() + dim;
		};

	template <size_t dim>	
		Point<dim>::Point(double value)
		{
			for (size_t i = 0; i < dim; ++i)
				(this->_vec)[i] = value;
		};

	template <size_t dim>	
		Point<dim>::Point(const initializer_list<double>& coor)
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
					(this->_vec)[i++] = val;
			}
		};

	template <size_t dim>	
		Point<dim>::Point (const Point<dim>& x)
		{
			*this = x;
		};

	template <size_t dim>	
		Point<dim>& Point<dim>::operator= (const Point<dim>& x)
		{
			if (&x != this)
				this->_vec = x._vec;

			return *this;
		};

	template <size_t dim>	
		template <size_t dummy, typename enable_if< (dummy == 1), int>::type>
		Point<dim>::operator double() const
		{
			return (this->_vec)[0];
		};

	template <size_t dim>	
		Point<dim>::operator array<double, dim> () const
		{
			array<double, dim> result;
			for (size_t i = 0; i < dim; ++i)
				result[i] = (this->_vec)[i];
			return result;
		};

	template <size_t dim>	
		Point<dim> Point<dim>::operator * (double alfa) const
		{
			return Point<dim>(this->_vec * alfa);
		};

	template <size_t dim>
		Point<dim>& Point<dim>::operator *= (double alfa)
		{
			this->_vec *= alfa;
			return *this;
		};

	template <size_t dim>	
		Point<dim> Point<dim>::operator - (const Point<dim>& x) const
		{
			return Point<dim>(this->_vec - x._vec);
		};

	template <size_t dim>	
		Point<dim> Point<dim>::operator + (const Point<dim>& x) const
		{
			return Point<dim>(this->_vec + x._vec);
		};


	template <size_t dim>	
		double& Point<dim>::operator[] (size_t ind)
		{
			return (this->_vec)(ind);
		};

	template <size_t dim>	
		const double& Point<dim>::operator[] (size_t ind)const
		{
			return (this->_vec)(ind);
		};

	template <size_t dim>	
		template <size_t N>
			Point<dim + N> Point<dim>::Tensor (const Point<N>& p2) const
			{
				Point<dim + N> result;

				for (size_t i = 0; i < dim; ++i)
					result[i] = (*this)[i];

				for (size_t j = 0; j < N; ++j)
					result[dim + j] = p2[j];

				return result;
			};

	template <size_t dim>	
		double Point<dim>::Abs() const
		{
			double result = (this->_vec).dot(this->_vec);
			return sqrt(result);
		};

	template <size_t dim>	
		double Point<dim>::distance(Point<dim> const& x) const
		{
			return (*this - x).Abs();
		};


	template <size_t dim>	
		void Point<dim>::Print(bool newline) const
		{
			cout << *this;
			if(newline)
				cout << endl;
		};

		/* copy constructor */
	template <size_t dim>	
		Matrix<dim>::Matrix(const Matrix<dim>& m)
		{
			*this = m;
		};

		/* assignment operator */
	template <size_t dim>	
		Matrix<dim>& Matrix<dim>::operator= (const Matrix<dim>& m)
		{
			if (&m != this)
			{
				this->_mat = m._mat;
			}
			return *this;
		};

	template <size_t dim>	
		double& Matrix<dim>::operator() (size_t i, size_t j)
		{
			return (this->_mat)(i,j);
		};

	template <size_t dim>	
		const double& Matrix<dim>::operator() (size_t i, size_t j) const
		{
			return (this->_mat)(i,j);
		};

	template <size_t dim>	
		template <size_t dummy, typename enable_if< (dummy == 1), int>::type>
		double& Matrix<dim>::operator() (size_t i)
		{
			return (this->_mat)(i);
		};

	template <size_t dim>
		template <size_t dummy, typename enable_if< (dummy == 1), int>::type>
		const double& Matrix<dim>::operator() (size_t i) const
		{
			return (this->_mat)(i);
		};

	template <size_t dim>
		double Matrix<dim>::Determinant() const
		{
			return this->_mat.determinant();
		};

	template <size_t dim>
		Matrix<dim> Matrix<dim>::Inverse() const
		{
			return Matrix<dim>(this->_mat.inverse());
		};

	template <size_t dim>
		Point<dim> operator* (const Matrix<dim>& A, const Point<dim>& b)
		{
			Point<dim> result;
			result._vec = A._mat * b._vec;
			return result;
		};
}
#endif //__POINT__HH
