#include "libmesh/mesh.h"

namespace BinaryTree
{
	template <size_t DIM>
		using function_ptr = shared_ptr<function<double(Point<DIM>)>>;


///*
//	It contains s attribute and relatively to-access-method needed by the algorithm
//	This is meant to be the type of the master (ghost) node of the binary tree, where I don't have a geometry and so I don't have to compute errors, but only to store informations for the node to be refined
//*/
	class BinaryNode
	{

		public:
			BinaryNode() : _s_element	(this),
								_E				(numeric_limits<double>::max()),
								_E_tilde		(numeric_limits<double>::max()),
								_q				(numeric_limits<double>::max())
			{};

			virtual BinaryNode* s() const
			{
				return _s_element;
			};

			virtual void s(BinaryNode* delta)
			{
				_s_element = delta;
			};

			virtual double E() const
			{
				return _E;
			};
			virtual void E (double val)
			{
				_E = val;
			};

			virtual double E_tilde () const
			{
				return _E_tilde;
			};
			virtual void E_tilde (double val)
			{
				_E_tilde = val;
			};

			virtual double q() const
			{
				return _q;
			};
			virtual void q(double val)
			{
				_q = val;
			};

			virtual BinaryNode* left() = 0;
			virtual BinaryNode* right() = 0;
			virtual BinaryNode* dad() = 0;

			virtual double projectionError() = 0;
			virtual void refine(libMesh::MeshRefinement &mesh_refinement) = 0;

			virtual void	pLevel(size_t val) = 0;
			virtual size_t pLevel() const = 0;

		protected:
			virtual void updateProjectionError() = 0;

		protected:
			BinaryNode* _s_element;
			double _E;
			double _E_tilde;
			double _q;
	};

	struct BinarityMap
	{
		static bool check_binarity(libMesh::MeshBase& mesh);

		//TODO: introduce support for meshes containing different dimension elements (i.e. both hexes and quads in the same mesh)
		template <size_t DIM>
			static void make_binary(libMesh::MeshBase& mesh, function_ptr<DIM> f_ptr)
			{
				/*to silence warning by the compiler*/
				(void) mesh;
				(void) f_ptr;
				/* In the default case (unexpected DIM) it throws an exception; concrete behaviour for able-to-handle DIM is defined in the specializations */
				throw invalid_argument("Cannot handle a mesh of dimension " + to_string(DIM));
			};

		static BinaryNode* as_binary(libMesh::Elem* el);

/*
		A map which tells me if an object of libMesh::Meshbase has been binarized
		The compulsory way to binarize an element is by the call of BinarityMap::make_binary<DIM> (mesh_to_be_binarized, f)
*/
		static map<libMesh::MeshBase*, bool> _boolean_map;
	};

	template <>
		void BinarityMap::make_binary<1> (libMesh::MeshBase&, function_ptr<1>);

	template <>
		void BinarityMap::make_binary<2> (libMesh::MeshBase&, function_ptr<2>);

	template <>
		void BinarityMap::make_binary<3> (libMesh::MeshBase&, function_ptr<3>);

} //namespace BinaryTree

