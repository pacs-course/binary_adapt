#ifndef __LIBMESH_BINARY_ELEMENT_H
#define __LIBMESH_BINARY_ELEMENT_H

#include "LibMeshFElement.h"
#include "AbstractBinaryElement.h"
#include "LibMeshBinarityMap.h"

#include <memory> //std::shared_ptr
#include <functional> //std::function

#include "libmesh/mesh_refinement.h"

namespace LibmeshBinary
{
	using namespace std;

	/**
		Concrete implementation of a binary tree element.
		It merges the functionalities given by the #BinaryTree::AbstractBinaryElement
		with the ones given by libMesh::Elem.
		The template parameter LibmeshGeometry defines the type of geometry underlying the element.
		In this way any libMesh geometry can be easily endowed with a binary tree element tools.
		It derives from a concrete libMesh::Elem implementation, so it is compatible with
		libMesh::Mesh methods, which are defined at an abstract level using only libMesh::Elem* pointers.
	**/
	template <size_t dim,
			  FiniteElements::BasisType FeType = FiniteElements::InvalidFeType,
			  class LibmeshGeometry = InvalidGeometry>
	class BinaryTreeElement :
		public LibmeshGeometry,
		public BinaryTree::AbstractBinaryElement<dim, FeType>
	{
	  public:
		/**
			constructor.
			Input parameters:
				-	a pointer to the libMesh element to be incorporated
				-	a pointer to the objective function of the algorithm to be projected
				-	a libMesh::MeshRefinement object
		**/
		BinaryTreeElement (unique_ptr<LibmeshGeometry>,
						   BinaryTree::FunctionPtr<dim>,
						   libMesh::MeshRefinement&);
		/**
			default destructor
		**/
		virtual ~BinaryTreeElement();
		/**
			Return NULL if element is a leaf
		**/
		virtual BinaryTree::BinaryNode* Left();
		/**
			Return NULL if element is a leaf
		**/
		virtual BinaryTree::BinaryNode* Right();
		/**
			Return NULL if node has no father
		**/
		virtual BinaryTree::BinaryNode* Dad();
		/**
			Node identifier
		**/
		virtual size_t NodeID() override;
		/**
			Overridden refinement method
		**/
		virtual void Bisect() override;
		/**
			Make the tree element an active mesh element
		**/
		virtual void Activate() override;
		/**
			Make the tree element inactive
		**/
		virtual void Deactivate() override;

	  private:
		/**
			libMesh refiner
		**/
		libMesh::MeshRefinement& _mesh_refinement;
	};

	//TODO: use GetPot to select FElement BasisType at runtime
	/**
		Binary interval based on libMesh geometry
	**/
	using Interval =
		BinaryTreeElement <	1,
							FiniteElements::LegendreType,
							LibmeshIntervalClass >;

	/**
		Binary triangle based on libMesh geometry.
		The override of the refinement algorithm is needed,
		since by default it splits itself into three children.
	**/
	class Triangle :
		public BinaryTreeElement < 2,
								   FiniteElements::WarpedType,
								   LibmeshTriangleClass>
	{
	  public:
		/**
			constructor
		**/
		Triangle (unique_ptr<LibmeshTriangleClass>,
				  BinaryTree::FunctionPtr<2>,
				  libMesh::MeshRefinement&);
		/**
			default destructor
		**/
		virtual ~Triangle();

		/**
			Embedding matrix implementing the bisection.
		**/
		virtual float embedding_matrix (const unsigned int,
										const unsigned int,
										const unsigned int) const override;
		/**
			Different embedding matrix for different longest side
		**/
		virtual unsigned int embedding_matrix_version () const override;

		/**
			Binary element has two children
		**/
		virtual unsigned int n_children () const override;

	  protected:
		/**
			The embedding matrices to be used for the binary bisection of the triangle.
			The one which will be effectively used depends on which is the longest side.
		**/
		static const float _binary_embedding_matrices[3][2][3][3];
		/**
			the index of the longest side
		**/
		size_t _longest_side;
	};


	template < size_t dim,
			   FiniteElements::BasisType FeType,
			   class LibmeshGeometry >
	BinaryTreeElement<dim, FeType, LibmeshGeometry>::BinaryTreeElement(
		unique_ptr<LibmeshGeometry> element_ptr,
		BinaryTree::FunctionPtr<dim> f,
		libMesh::MeshRefinement& mesh_refinement) :

			LibmeshGeometry (*element_ptr),
			BinaryTree::AbstractBinaryElement<dim, FeType>(
				f,
				new FElement <dim, FeType, LibmeshGeometry> (move (element_ptr))),
			_mesh_refinement (mesh_refinement)
	{};

	template < size_t dim,
			   FiniteElements::BasisType FeType,
			   class LibmeshGeometry >
	BinaryTreeElement<dim, FeType, LibmeshGeometry>::~BinaryTreeElement()
	{};

	template < size_t dim,
			   FiniteElements::BasisType FeType,
			   class LibmeshGeometry >
	BinaryTree::BinaryNode* BinaryTreeElement<dim, FeType, LibmeshGeometry>
	::Left()
	{
		if (this->has_children())
			return BinarityMap::AsBinary<dim> (this->child (0));

		return nullptr;
	};

	template < size_t dim,
			   FiniteElements::BasisType FeType,
			   class LibmeshGeometry>
	BinaryTree::BinaryNode* BinaryTreeElement<dim, FeType, LibmeshGeometry>
	::Right()
	{
		if (this->has_children())
			return BinarityMap::AsBinary<dim> (this->child (1));

		return nullptr;
	};

	template < size_t dim,
			   FiniteElements::BasisType FeType,
			   class LibmeshGeometry >
	BinaryTree::BinaryNode* BinaryTreeElement<dim, FeType, LibmeshGeometry>
	::Dad()
	{
		return BinarityMap::AsBinary<dim> (this->parent());
	};

	template < size_t dim,
			   FiniteElements::BasisType FeType,
			   class LibmeshGeometry >
	size_t BinaryTreeElement<dim, FeType, LibmeshGeometry>
	::NodeID()
	{
		return static_cast<size_t> (this->id());
	};

	template < size_t dim,
			   FiniteElements::BasisType FeType,
			   class LibmeshGeometry >
	void BinaryTreeElement<dim, FeType, LibmeshGeometry>
	::Bisect()
	{
		/* Firstly I check that the derived class
		   which implements the refine makes two children */
		if (this->n_children() != 2)
			throw logic_error ("This is not a binary element!");

		this->set_refinement_flag (libMesh::Elem::REFINE);

		libMesh::Elem::refine (this->_mesh_refinement);

		BinarityMap::MakeBinary<dim> (this->_mesh_refinement, this->_f);
	};

	template < size_t dim,
			   FiniteElements::BasisType FeType,
			   class LibmeshGeometry >
	void BinaryTreeElement<dim, FeType, LibmeshGeometry>
	::Activate()
	{
		/*  when I activate my binary node, I also change the libMesh::Elem::_p_level,
			so that the p-refinement can be see also in a libMesh framework */
		this->set_p_level (this->PLevel());

		if (! (this->active()))
			this->set_refinement_flag (libMesh::Elem::JUST_REFINED);
	};

	template < size_t dim,
			   FiniteElements::BasisType FeType,
			   class LibmeshGeometry >
	void BinaryTreeElement<dim, FeType, LibmeshGeometry>
	::Deactivate()
	{
		if (this->active())
			this->set_refinement_flag (libMesh::Elem::INACTIVE);
	};

}; //namespace LibmeshBinary

#endif //__LIBMESH_BINARY_ELEMENT_H
