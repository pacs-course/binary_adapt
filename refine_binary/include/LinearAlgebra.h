#ifndef __POINT__HH
#define __POINT__HH

#include <iostream> //cout
#include <stdexcept> //invalid_argument
#include <string> //string
#include <array> //array
#include <math.h> //sqrt, abs
#include <vector> //vector

#include <Eigen/Dense>
/**
	Geometric structures needed the library objects
**/
namespace Geometry
{
	using namespace std;

	/**
		Type for eigen column vector. v	Eigen::DontAlign option used since fixed-size eigen object is used as class member;
		see http://eigen.tuxfamily.org/dox-devel/group__TopicStructHavingEigenMembers.html
	**/
	template <size_t dim>
	using EigVector = Eigen::Matrix <double,
									 static_cast<int> (dim),
									 1,
									 Eigen::DontAlign>;

	/**
		Type for dim x dim square matrix.
		Eigen::DontAlign option used since fixed-size eigen object is used as class member;
		see http://eigen.tuxfamily.org/dox-devel/group__TopicStructHavingEigenMembers.html
	**/
	template <size_t dim>
	using EigMatrix = Eigen::Matrix <double,
									 static_cast<int> (dim),
									 static_cast<int> (dim),
									 Eigen::DontAlign>;

	/**
		Type for eigen semidynamic matrix.
		Eigen::DontAlign option used since fixed-size eigen object is used as class member;
		see http://eigen.tuxfamily.org/dox-devel/group__TopicStructHavingEigenMembers.html
	**/
	template <size_t dim>
	using EigSemiDynamicMat = Eigen::Matrix <double,
											 static_cast<int> (dim),
											 Eigen::Dynamic>;

	/**
		Type for eigen fully dynamic matrix.
	**/
	using EigDynamicMat = Eigen::MatrixXd;
	/**
		Type for eigen dynamic vector
	**/
	using EigDynamicVec = Eigen::VectorXd;


	template <size_t dim>
	class Point;

	template <size_t dim>
	class Matrix;

	template <size_t dim>
	class VectorPoint;

	class DynamicVector;
	class DynamicMatrix;

	/**
		Fixed size vector.
		Used to represent points of dimensionality dim.
	**/
	template <size_t dim>
	using VecType = Point<dim>;

	/**
		Type for dim x dim square matrix.
		Used by Maps to store info to make the map Evaluate()
	**/
	template <size_t dim>
	using MatrixType = Matrix<dim>;

	/**
		Type for vector of points.
		Used to store vertexes of geometries by Maps::Init() and DimensionedNode::Nodes().
	**/
	template <size_t dim>
	using NodesVector = VectorPoint<dim>;

	using ColumnVector = DynamicVector;
	/**
		Vector of mutable length.
		Used to store quadrature weights.
	**/
	using Vector = ColumnVector;

	/**
		Class for vectors of unfixed size.
		This is a wrapper class for eigen dynamic vector.
		Operations are defined in terms of the underlying eigen vector counterparts.
	**/
	class DynamicVector
	{
	  public:
		/**
			A part of the vector
		**/
		using BlockType = EigDynamicVec::ConstSegmentReturnType;

		/**
			default constructor
		**/
		DynamicVector();
		/**
			constructor with vector length
		**/
		DynamicVector (size_t);
		/**
			constructor from standard vector
		**/
		DynamicVector (const vector<double>&);
	  protected:
		/**
			constructor from eigen type
		**/
		DynamicVector (const EigDynamicVec& v);

	  public:
		/**
			default destructor
		**/
		virtual ~DynamicVector();
		/**
			copy constructor
		**/
		DynamicVector (const DynamicVector&);
		/**
			assignment operator
		**/
		DynamicVector& operator= (const DynamicVector&);

		/**
			access vector element
		**/
		double& operator[] (size_t);
		/**
			const access to vector element
		**/
		const double& operator[] (size_t) const;

		/**
			Scalar product with another vector.
			Vectors must have the same size.
		**/
		double Dot (const DynamicVector&) const;
		/**
			Scalar product with part of another vector.
			Must have the same size.
		**/
		double Dot (const BlockType&) const;
		/**
			Term by term product.
		**/
		DynamicVector CWiseProduct (const DynamicVector&) const;

		/**
			Length of the vector
		**/
		size_t Size() const;
		/**
			Set new length of the vector
		**/
		void Resize (size_t);

		/**
			The first part of the vector until input index.
		**/
		BlockType Head (size_t) const;

		friend DynamicVector operator* (const DynamicMatrix&, const DynamicVector&);

		friend class DynamicMatrix;

	  protected:
		/**
			eigen vector storage.
		**/
		EigDynamicVec _vec;
	};

	/**
		Class for matrix with both rows and colums dynamically allocated.
		This is a wrapper class for eigen fully dynamic matrix.
		Operations are defined in terms of the underlying eigen matrix counterparts.
	**/
	class DynamicMatrix
	{
	  public:
		/**
			default constructor
		**/
		DynamicMatrix();
		/**
			constructor with matrix size
		**/
		DynamicMatrix (size_t, size_t);
		/**
			default destructor
		**/
		virtual ~DynamicMatrix();

		/**
			Set vector as matrix column.
		**/
		void SetCol (size_t, const DynamicVector&);

		friend DynamicVector operator* (const DynamicMatrix&, const DynamicVector&);

	  protected:
		/**
			eigen matrix storage
		**/
		EigDynamicMat _mat;
	};

	DynamicVector operator* (const DynamicMatrix&, const DynamicVector&);

	/**
		Class for points in R^dim.
		Equally it could be meant as a column vector of length = dim.
	**/
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
		/**
			constructor
		**/
		Point (double value = 0.0);
		/**
			default destructor
		**/
		virtual ~Point() {};
		/**
			constructor from initializer list
		**/
		Point (const initializer_list<double>&);

	  protected:
		/**
			constructor from EigVector type
		**/
		Point (const EigVector<dim>&);

	  public:
		/**
			copy constructor
		**/
		Point (const Point&);
		/**
			assignment operator
		**/
		Point& operator= (const Point&);

		/**
			Implicit cast to double for 1D points
		**/
		template <size_t dummy = dim,
				  typename enable_if< (dummy == 1), int>::type = 0>
		operator double() const;

		/**
			explicit cast to std::array
		**/
		explicit operator array<double, dim> () const;

		/**
			Product by scalar
		**/
		Point<dim> operator * (double) const;
		/**
			Product by scalar
		**/
		Point<dim>& operator *= (double);

		/**
			Vectors difference
		**/
		Point<dim> operator - (const Point<dim>&) const;
		/**
			Vectors sum
		**/
		Point<dim> operator + (const Point<dim>&) const;

		/**
			Access to point component
		**/
		double& operator[] (size_t);
		/**
						const access to point component
		**/
		const double& operator[] (size_t) const;

		/**
						Tensor product.
						(x) tensor (y) = (x,y)
		**/
		template <size_t N>
		Point < dim + N > Tensor (const Point<N>&) const;

		/**
			Modulus of the vector
		**/
		double Abs() const;
		/**
			Euclidean distance between vectors
		**/
		double distance (Point<dim> const&) const;

		/**
			stream operator overload.
			friend to enable i.e. cout << point.
		**/
		friend ostream& operator<< (ostream& output, const Point<dim>& p)
		{
			output << "(" << p[0];
			for (size_t i = 1; i < dim; ++i)
				output << "," << p[i];
			output << ")";
			return output;
		};

		/**
			Print point to standard output.
			If boolean input is true, a new line is added at the end of the output.
		**/
		void Print (bool newline = true) const;

		template <size_t N>
		friend Point<N> operator* (const Matrix<N>&, const Point<N>&);

		template <size_t N>
		friend VectorPoint<N> operator+ (const VectorPoint<N>&, const Point<N>&);

		template <size_t N>
		friend VectorPoint<N> operator- (const VectorPoint<N>&, const Point<N>&);

		friend class VectorPoint<dim>;

	  protected:
		/**
			eigen fixed-size vector storage
		**/
		EigVector<dim> _vec;
	};

	/**
		Class for a matrix meant as a vector of dim sized columns
	**/
	template <size_t dim>
	class VectorPoint
	{
	  public:
		/**
			default constructor
		**/
		VectorPoint() : _mat() {};
		/**
			default destructor
		**/
		virtual ~VectorPoint() {};

		/**
			constructor with columns number
		**/
		VectorPoint (size_t c) : _mat (dim, c) {};

	  protected:
		/**
			constructor from eigen type
		**/
		VectorPoint (const EigSemiDynamicMat<dim>& m) : _mat (m) {};

	  public:
		/**
			copy constructor
		**/
		VectorPoint (const VectorPoint&);
		/**
			assignment operator
		**/
		VectorPoint& operator= (const VectorPoint&);

		/**
			Get a copy of a column
		**/
		Point<dim> operator[] (size_t) const;

		/**
			Set a column
		**/
		void Insert (size_t, const Point<dim>&);

		/**
			The number of columns
		**/
		size_t Size() const;

		template <size_t N>
		friend VectorPoint<N> operator* (const Matrix<N>&, const VectorPoint<N>&);

		template <size_t N>
		friend VectorPoint<N> operator+ (const VectorPoint<N>&, const Point<N>&);

		template <size_t N>
		friend VectorPoint<N> operator- (const VectorPoint<N>&, const Point<N>&);

	  protected:
		/**
			eigen matrix storage
		**/
		EigSemiDynamicMat<dim> _mat;
	};

	/**
		Class for matrices in R^(dim x dim)
	**/
	template <size_t dim>
	class Matrix
	{
	  public:
		/**
			default constructor
		**/
		Matrix() : _mat() {};
		/**
			default destructor
		**/
		virtual ~Matrix() {};

	  protected:
		/**
			constructor from eigen type
		**/
		Matrix (const EigMatrix<dim>& m) : _mat (m) {};

	  public:
		/**
			copy constructor
		**/
		Matrix (const Matrix&);
		/**
			assignment operator
		**/
		Matrix& operator= (const Matrix&);

		/**
			access element of the matrix
		**/
		double& operator() (size_t, size_t);
		/**
			const access to element of the matrix
		**/
		const double& operator() (size_t, size_t) const;

		/**
			matrix determinant
		**/
		double Determinant() const;
		/**
			inverse matrix
		**/
		Matrix Inverse()const;

		template <size_t N>
		friend Point<N> operator* (const Matrix<N>&, const Point<N>&);

		template <size_t N>
		friend VectorPoint<N> operator* (const Matrix<N>&, const VectorPoint<N>&);

	  protected:
		/**
			eigen matrix storage
		**/
		EigMatrix<dim> _mat;
	};

	/**
		Matrix times vector product
	**/
	template <size_t dim>
	Point<dim> operator* (const Matrix<dim>& A, const Point<dim>& b)
	{
		EigVector<dim> p = A._mat * b._vec;
		return Point<dim> (p);
	};

	/**
		Matrix times matrix product
	**/
	template <size_t dim>
	VectorPoint<dim> operator* (const Matrix<dim>& A, const VectorPoint<dim>& B)
	{
		EigSemiDynamicMat<dim> M = A._mat * B._mat;
		return VectorPoint<dim> (M);
	};

	/**
		Add vector to every column
	**/
	template <size_t dim>
	VectorPoint<dim> operator+ (const VectorPoint<dim>& A, const Point<dim>& b)
	{
		VectorPoint<dim> result (A);
		for (size_t i = 0; i < result.Size(); ++i)
			result._mat.col (i) += b._vec;

		return result;
	};

	/**
		Subtract vector from every column
	**/
	template <size_t dim>
	VectorPoint<dim> operator- (const VectorPoint<dim>& A, const Point<dim>& b)
	{
		VectorPoint<dim> result (A);
		for (size_t i = 0; i < result.Size(); ++i)
			result._mat.col (i) -= b._vec;

		return result;
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
	Point<dim>::Point (double value)
	{
		for (size_t i = 0; i < dim; ++i)
			(this->_vec)[i] = value;
	};

	template <size_t dim>
	Point<dim>::Point (const initializer_list<double>& coor)
	{
		auto l = coor.size();
		if (l != dim)
		{
			throw invalid_argument
				("Invalid number of elements in Geometry::Point construction");
		}
		else
		{
			size_t i (0);
			for (const auto& val : coor)
				(this->_vec)[i++] = val;
		}
	};

	template <size_t dim>
	Point<dim>::Point (const EigVector<dim>& v) : _vec (v)
	{};

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
		return Point<dim> (this->_vec * alfa);
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
		return Point<dim> (this->_vec - x._vec);
	};

	template <size_t dim>
	Point<dim> Point<dim>::operator + (const Point<dim>& x) const
	{
		return Point<dim> (this->_vec + x._vec);
	};


	template <size_t dim>
	double& Point<dim>::operator[] (size_t ind)
	{
		return (this->_vec) (ind);
	};

	template <size_t dim>
	const double& Point<dim>::operator[] (size_t ind)const
	{
		return (this->_vec) (ind);
	};

	template <size_t dim>
	template <size_t N>
	Point < dim + N > Point<dim>::Tensor (const Point<N>& p2) const
	{
		Point < dim + N > result;

		for (size_t i = 0; i < dim; ++i)
			result[i] = (*this)[i];

		for (size_t j = 0; j < N; ++j)
			result[dim + j] = p2[j];

		return result;
	};

	template <size_t dim>
	double Point<dim>::Abs() const
	{
		double result = (this->_vec).dot (this->_vec);
		return sqrt (result);
	};

	template <size_t dim>
	double Point<dim>::distance (Point<dim> const& x) const
	{
		return (*this - x).Abs();
	};


	template <size_t dim>
	void Point<dim>::Print (bool newline) const
	{
		cout << *this;
		if (newline)
			cout << endl;
	};

	template <size_t dim>
	Matrix<dim>::Matrix (const Matrix<dim>& m)
	{
		*this = m;
	};

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
		return (this->_mat) (i, j);
	};

	template <size_t dim>
	const double& Matrix<dim>::operator() (size_t i, size_t j) const
	{
		return (this->_mat) (i, j);
	};

	template <size_t dim>
	double Matrix<dim>::Determinant() const
	{
		return this->_mat.determinant();
	};

	template <size_t dim>
	Matrix<dim> Matrix<dim>::Inverse() const
	{
		return Matrix<dim> (this->_mat.inverse());
	};

	template <size_t dim>
	VectorPoint<dim>::VectorPoint (const VectorPoint<dim>& m)
	{
		*this = m;
	};

	template <size_t dim>
	VectorPoint<dim>& VectorPoint<dim>::operator= (const VectorPoint<dim>& m)
	{
		if (&m != this)
		{
			this->_mat = m._mat;
		}
		return *this;
	};

	template <size_t dim>
	size_t VectorPoint<dim>::Size() const
	{
		return this->_mat.cols();
	};

	template <size_t dim>
	Point<dim> VectorPoint<dim>::operator[] (size_t c) const
	{
		return Point<dim> (_mat.col (c));
	};

	template <size_t dim>
	void VectorPoint<dim>::Insert (size_t c, const Point<dim>& p)
	{
		size_t l = _mat.size();
		if (c >= l)
		{
#ifdef TRY_IT
			/*	 Make automatically the resize instead of throwing the exception
			    TODO: test it */
			cerr << "Warning: inserting column in VectorPoint at index " << c <<
				 ", but size is " << l << endl;
			_mat.conservativeResize (dim, c + 1);
#ifdef VERBOSE
			cerr << "***Resizing VectorPoint***" << endl;
			cerr << "New size: " << _mat.cols() << endl;
#endif //VERBOSE
#else //TRY_IT
			throw logic_error ("Trying to insert column in "
							   + to_string (c)
							   + " position, but VectorPoint size is "
							   + to_string (_mat.cols()));
#endif //TRY_IT
		}
		_mat.col (c) = p._vec;
	};

} //namespace Geometry
#endif //__POINT__HH
