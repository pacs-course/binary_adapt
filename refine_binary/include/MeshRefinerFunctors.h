#ifndef __MESH_REFINER_FUNCTORS_H
#define __MESH_REFINER_FUNCTORS_H

#include <fstream> //std::ofstream

#include "BinaryNode.h"

namespace BinaryTree
{
	class NodeOperator
	{
		public:
			NodeOperator(){};
			~NodeOperator(){};
			virtual void operator()(BinaryNode*) = 0;
	};

	class Counter : public NodeOperator
	{
		public:
			Counter():_counter(0){};
			virtual void operator()(BinaryNode*)override;
			size_t GetCount()const;
		protected:
			size_t _counter;
	};

	class ErrorComputer : public NodeOperator
	{
		public:
			ErrorComputer(double&);
			virtual void operator()(BinaryNode*)override;

			void ResetError();
		protected:
			double& _error_variable;
	};

	class PlevelsExtractor : public NodeOperator
	{
		public:
			PlevelsExtractor() : _p_levels(){};
			virtual void operator()(BinaryNode*)override;
			
			std::vector<size_t> GetPLevels();
		protected:
			std::vector<size_t> _p_levels;
	};

	template <size_t dim>
		class DimOperator
		{
			public:
				DimOperator(){};
				~DimOperator(){};
				virtual void operator()(DimensionedNode<dim>*) = 0;
		};

	class GnuPlotPrinter : public DimOperator<1>
	{
		public:
			GnuPlotPrinter(std::ofstream&);
			~GnuPlotPrinter(){};
			virtual void operator() (DimensionedNode<1>*) override;
			double XMin()const;
			double XMax()const;
			size_t PMax()const;
			std::vector<double> XLeftVec()const;
		protected:
			std::ofstream& _output_file;
			double _x_min;
			double _x_max;
			size_t _p_max;
			std::vector<double> _x_left_vec;
			std::vector<double> _x_right_vec;
			std::vector<size_t> _p_level_vec;
			size_t _cont;
	};

	class ProjectionPrinter : public DimOperator<1>
	{
		public:
			ProjectionPrinter(std::ofstream&, double);
			~ProjectionPrinter(){};
			virtual void operator() (DimensionedNode<1>*) override;
		protected:
			std::ofstream& _output_file;
			double _x_step;
	};
} //namespace BinaryTree
#endif //__MESH_REFINER_FUNCTORS_H
