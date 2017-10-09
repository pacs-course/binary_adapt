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

} //namespace Geometry
