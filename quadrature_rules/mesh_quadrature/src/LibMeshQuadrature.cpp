#include "LibMeshQuadrature.h"

using namespace std;

namespace LibmeshBinary
{
	ModifiedTriangleRule::ModifiedTriangleRule() : LibmeshQuadratureRule<2, Geometry::TriangleType>()
	{
		Geometry::NodesVector<2> vertices(3);
		Geometry::Point<2> v1({0,0});
		Geometry::Point<2> v2({1,0});
		Geometry::Point<2> v3({0,1});
		vertices.Insert(0, v1);
		vertices.Insert(1, v2);
		vertices.Insert(2, v3);
		_std_map.Init(vertices);
	};

	ModifiedTriangleRule::~ModifiedTriangleRule()
	{};

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
