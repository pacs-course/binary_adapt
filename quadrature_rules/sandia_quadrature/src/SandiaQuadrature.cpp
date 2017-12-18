#include "SandiaQuadrature.h"

namespace SandiaQuadrature
{
	template <>
	Geometry::QuadPointVec<1> TensorizePoints<1> (
		const Geometry::QuadPointVec<1>& one_d_p)
	{
		return one_d_p;
	};

	template <>
	Geometry::QuadWeightVec TensorizeWeights<1> (
		const Geometry::QuadWeightVec& one_d_w)
	{
		return one_d_w;
	};
} //namespace SandiaQuadrature
