#include "Maps.h"
#include "BinaryTreeHelper.h"

namespace Geometry
{
	void IntervalMap::Init(const NodesVector<1>& vert)
	{
		double a = vert[0][0];
		double b = vert[1][0];

		this->_mat(0) = (b - a) / 2;
		this->_trasl[0] = (a + b) / 2;
		this->_jacobian = this->_mat(0);
		this->_inverse(0) = 1 / this->_mat(0);
	};

	void TriMap::Init(const NodesVector<2>& vert)
	{
		double x1 = vert[0][0];
		double y1 = vert[0][1];
		double x2 = vert[1][0];
		double y2 = vert[1][1];
		double x3 = vert[2][0];
		double y3 = vert[2][1];

		this->_mat(0,0) = (x2 - x1) / 2;
		this->_mat(0,1) = (x3 - x1) / 2;
		this->_mat(1,0) = (y2 - y1) / 2;
		this->_mat(1,1) = (y3 - y1) / 2;

		this->_jacobian = this->_mat.Determinant();

		this->_trasl[0] = (x2 + x3) / 2;
		this->_trasl[1] = (y2 + y3) / 2;

		//TODO: optimize it
		this->_inverse = this->_mat.Inverse();
	};

	Point<2> StdTriMap::Evaluate (const Point<2>& p)const
	{
		Point<2> result;
		double csi = p[0];
		double eta = p[1];
		result[0] = (1 + csi) * (1 - eta) / 2 - 1;
		result[1] = eta;
		return result;
	};

	Point<2> StdTriMap::ComputeInverse (const Point<2>& p)const
	{
		Point<2> result;
		double x = p[0];
		double y = p[1];
		result[0] = 2 * ( 1 + x) / (1 - y) - 1;
		result[1] = y;
		return result;
	};

	double StdTriMap::EvaluateJacobian (const Point<2>& p)const
	{
		return (1 - p[1]) / 2;
	};

} //namespace Geometry


