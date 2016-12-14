#include "LibMeshBinaryElements.h"

using namespace std;

namespace LibmeshBinary
{
	/* initializing the static attribute
		I have to create the new node at the half of the longest side of the triangle

		nodes:
			2
			|\
			| \
			|  \
			|	 \
			|____\
			0		1

		sides:
			|\
			| \
		2	|  \ 1
			|	 \
			|____\
				0
	*/
	const float Triangle::_binary_embedding_matrices[3][2][3][3] = 
	{
		//if the longest is the number 0
		{
			// embedding matrix for child 0
			{
			// 0    1    2
			{1.0, 0.0, 0.0}, // 0
			{0.5, 0.5, 0.0}, // 1
			{0.0, 0.0, 1.0}  // 2
			},

			// embedding matrix for child 1
			{
			// 0    1    2
			{0.5, 0.5, 0.0}, // 0
			{0.0, 1.0, 0.0}, // 1
			{0.0, 0.0, 1.0}  // 2
			}
		},

		//if the longest is the number 1
		{
			// embedding matrix for child 0
			{
			// 0    1    2
			{1.0, 0.0, 0.0}, // 0
			{0.0, 1.0, 0.0}, // 1
			{0.0, 0.5, 0.5}  // 2
			},

			// embedding matrix for child 1
			{
			// 0    1    2
			{1.0, 0.0, 0.0}, // 0
			{0.0, 0.5, 0.5}, // 1
			{0.0, 0.0, 1.0}  // 2
			}
		},

		//if the longest is the number 2
		{
			// embedding matrix for child 0
			{
			// 0    1    2
			{1.0, 0.0, 0.0}, // 0
			{0.0, 1.0, 0.0}, // 1
			{0.5, 0.0, 0.5}  // 2
			},

			// embedding matrix for child 1
			{
			// 0    1    2
			{0.5, 0.0, 0.5}, // 0
			{0.0, 1.0, 0.0}, // 1
			{0.0, 0.0, 1.0}  // 2
			}
		}
	};

	Triangle::Triangle (	unique_ptr<LibmeshTriangleClass> el_ptr,
								BinaryTree::FunctionPtr<2> f,
								libMesh::MeshRefinement& mesh_refinement) :

								BinaryTreeElement<2, LegendreType, LibmeshTriangleClass>(move(el_ptr), f, mesh_refinement )
	{
		auto nodes = this->_f_element->GetNodes();
		vector<double> side_lengths(0,3);

		side_lengths[0] = nodes[1].distance(nodes[0]);
		side_lengths[1] = nodes[2].distance(nodes[1]);
		side_lengths[2] = nodes[0].distance(nodes[2]);

		double longest_length(0);
		for (size_t i = 0; i < side_lengths.size(); ++i)
		{
			if (side_lengths[i] > longest_length)
				this->_longest_side = i;
		}
	};

};
