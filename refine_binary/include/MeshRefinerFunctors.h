#ifndef __MESH_REFINER_FUNCTORS_H
#define __MESH_REFINER_FUNCTORS_H

#include <fstream> //std::ofstream

#include "BinaryNode.h"

namespace BinaryTree
{
	/**
		Class for functors that can modify binary tree nodes.
		NodeOperator object has to be passed to MeshRefiner::IterateActiveNodes(),
		and the operator() will be called on every active element.
		This functor can modify the nodes, and so the mesh.
	**/
	class NodeOperator
	{
	  public:
		/**
			default constructor
		**/
		NodeOperator();
		/**
			default destructor
		**/
		virtual ~NodeOperator();

		virtual void operator() (BinaryNode*) = 0;
	};

	/**
		Class for functors that can't modify binary tree nodes.
		ConstOperator object has to be passed to MeshRefiner::IterateActiveNodes(),
		and the operator() will be called on every active element.
		This functor can't modify the nodes, and so the mesh.
	**/
	class ConstOperator
	{
	  public:
		/**
			default constructor
		**/
		ConstOperator();
		/**
			default destructor
		**/
		virtual ~ConstOperator();

		virtual void operator() (const BinaryNode*) = 0;
	};

	/**
		It simply counts the active elements of the mesh.
		Count can be got through GetCount() method.
	**/
	class Counter : public ConstOperator
	{
	  public:
		/**
			constructor
		**/
		Counter();
		/**
			default destructor
		**/
		virtual ~Counter();

		virtual void operator() (const BinaryNode*)override;
		size_t GetCount()const;

	  protected:
		size_t _counter;
	};

	/**
		Functor to compute the global projection error on the mesh.
		On every node it gets the local error and adds it to the _error_variable attribute.
		ResetError() to set it to 0.
	**/
	class ErrorComputer : public NodeOperator
	{
	  public:
		/**
			constructor.
			It takes by reference the variable where error is stored
		**/
		ErrorComputer (double&);
		/**
			default destructor
		**/
		virtual ~ErrorComputer();
		/**
			it adds the error on input node to _error_variable
		**/
		virtual void operator() (BinaryNode*)override;
		/**
			Set the _error_variable to 0
		**/
		void ResetError();

	  protected:
		/**
			error location
		**/
		double& _error_variable;
	};

	/**
		Functor the get from the underlying mesh the elements p levels.
		When iterating over active nodes it adds the node p level to the
		#_p_levels vector; at the end of the iteration do GetPLevels to get it.
	**/
	class PlevelsExtractor : public ConstOperator
	{
	  public:
		/**
			default constructor
		**/
		PlevelsExtractor();
		/**
			default destructor
		**/
		virtual ~PlevelsExtractor();

		/**
			push_back to #_p_levels vector the p level of the node
		**/
		virtual void operator() (const BinaryNode*)override;
		/**
			get  p levels
		**/
		std::vector<size_t> GetPLevels()const;

	  protected:
		/**
			p levels vector
		**/
		std::vector<size_t> _p_levels;
	};

	/**
		Class for functors depending on the dimensionality.
		DimOperator object has to be passed to MeshRefiner::IterateActiveNodes(),
		and the operator() will be called on every active element.
		It implements functions which depends on the dimensionality of the problem;
		This functor can modify the nodes, and so the mesh.
	**/
	template <size_t dim>
	class DimOperator
	{
	  public:
		/**
			default constructor
		**/
		DimOperator() {};
		/**
			default destructor
		**/
		virtual ~DimOperator() {};

		virtual void operator() (DimensionedNode<dim>*) = 0;
	};

	/**
		Class for functors depending on the dimensionality, cannot modify.
		DimOperator object has to be passed to MeshRefiner::IterateActiveNodes(),
		and the operator() will be called on every active element.
		It implements functions which depends on the dimensionality of the problem;
		i.e. it's needed by MeshRefiner::ExportProjection() and MeshRefiner::ExportGnuPlot() method.
		This functor can't modify the nodes, and so the mesh.
	**/
	template <size_t dim>
	class ConstDimOperator
	{
	  public:
		/**
			default constructor
		**/
		ConstDimOperator() {};
		/**
			default destructor
		**/
		virtual ~ConstDimOperator() {};

		virtual void operator() (const DimensionedNode<dim>*) = 0;
	};

	/**
		Functor for MeshRefiner::ExportGnuPlot() method.
		It directly prints on file.
	**/
	class GnuPlotPrinter : public ConstDimOperator<1>
	{
	  public:
		/**
			constructor.
			It gets the output file stream.
		**/
		GnuPlotPrinter (std::ofstream&);
		/**
			default destructor
		**/
		virtual ~GnuPlotPrinter();
		/**
			Print the node boundaries and p level.
			On every node it prints on file the boundaries and stores them in x vectors;
			it prints on file the p level and stores it in p levels vector
		**/
		virtual void operator() (const DimensionedNode<1>*) override;
		/**
			Get #_x_min attribute
		**/
		double XMin()const;
		/**
			Get #_x_max attribute
		**/
		double XMax()const;
		/**
			Get #_p_max attribute
		**/
		size_t PMax()const;

		/**
			Get #_x_left_vec attribute
		**/
		std::vector<double> XLeftVec()const;
	  protected:
		/**
			file where to write
		**/
		std::ofstream& _output_file;
		/**
			left bound of the interval
		**/
		double _x_min;
		/**
			right bound of the interval
		**/
		double _x_max;
		/**
			maximum p level found during the iteration
		**/
		size_t _p_max;
		/**
			vector of left bounds of mesh elements
		**/
		std::vector<double> _x_left_vec;
		/**
			vector of right bounds of mesh elements
		**/
		std::vector<double> _x_right_vec;
		/**
			vector of mesh elements p levels
		**/
		std::vector<size_t> _p_level_vec;
		/**
			number of elements
		**/
		size_t _cont;
	};

	/**
		Functor for MeshRefiner::ExportProjection() method.
		It directly prints on file.
	**/
	class ProjectionPrinter : public ConstDimOperator<1>
	{
	  public:
		/**
			constructor.
			It gets the output file stream.
		**/
		ProjectionPrinter (std::ofstream&, double);
		/**
			default destructor
		**/
		virtual ~ProjectionPrinter();
		/**
			Print on file projection values.
			It prints on #_output_file the values of the interpolated function
			computed at equidistant points between the node boundaries,
			whith distance #_x_step between each other.
		**/
		virtual void operator() (const DimensionedNode<1>*) override;
	  protected:
		/**
			The file where to print
		**/
		std::ofstream& _output_file;
		/**
			The length between two different point on which the projection has to be evaluated.
		**/
		double _x_step;
	};
} //namespace BinaryTree
#endif //__MESH_REFINER_FUNCTORS_H
