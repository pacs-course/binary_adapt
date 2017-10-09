#include "LibMeshQuadrature.h"

using namespace std;

namespace LibmeshBinary
{
	ModifiedTriangleRule::ModifiedTriangleRule() : LibmeshQuadratureRule<2, Geometry::TriangleType>()
	{
		Geometry::NodesVector<2> vertices(3);
		vertices[0][0] = 0;
		vertices[0][1] = 0;
		vertices[1][0] = 1;
		vertices[1][1] = 0;
		vertices[2][0] = 0;
		vertices[2][1] = 1;
		_std_map.Init(vertices);
	};

	Geometry::QuadPointVec<2> ModifiedTriangleRule::GetPoints()
	{
		auto points = LibmeshQuadratureRule<2, Geometry::TriangleType>::GetPoints();
		return this->_std_map.ComputeInverse(points);
	};

	Geometry::QuadWeightVec ModifiedTriangleRule::GetWeights()
	{
		auto weights = LibmeshQuadratureRule<2, Geometry::TriangleType>::GetWeights(); 
		//The jacobian of the inverse map is 1 / J, since the map is affine
		for (size_t i = 0; i < weights.Size(); ++i)
			weights[i] /= this->_std_map.Jacobian();

		return weights;
	};

} //namespace LibmeshBinary
