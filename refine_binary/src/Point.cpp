#include "Point.h"

using namespace std;

namespace Geometry
{
	DynamicVector::DynamicVector() : _vec(){};

	DynamicVector::DynamicVector(size_t l) : _vec(l){};

	DynamicVector::DynamicVector(const DynamicVector& v)
	{
		*this = v;
	};

	DynamicVector::DynamicVector(const EigDynamicVec& v) : _vec(v){};
	DynamicVector::DynamicVector(const vector<double>& v) : _vec(v.size())
	{
		for (size_t i = 0; i < v.size(); ++i)
			_vec(i) = v[i];
	};

	DynamicVector& DynamicVector::operator= (const DynamicVector& v)
	{
		if (&v != this)
		{
			_vec = v._vec;
		}
		return *this;
	};

	double& DynamicVector::operator[] (size_t i)
	{
		return (this->_vec)(i);
	};

	const double& DynamicVector::operator[] (size_t i) const
	{
		return (this->_vec)(i);
	};

	double DynamicVector::Dot(const DynamicVector& v) const
	{
		return (this->_vec).dot(v._vec);
	};

	size_t DynamicVector::Size() const
	{
		return this->_vec.size();
	};

	void DynamicVector::Resize(size_t L)
	{
		this->_vec.conservativeResize(L);
	};

	DynamicMatrix::DynamicMatrix() : _mat(){};

	DynamicMatrix::DynamicMatrix(size_t i, size_t j) : _mat(i,j){};

	void DynamicMatrix::SetCol(size_t i, const DynamicVector& v)
	{
		this->_mat.col(i) = v._vec;
	};

	DynamicVector operator* (const DynamicMatrix& A, const DynamicVector& b)
	{
		return DynamicVector(A * b);
	};

} //namespace Geometry
