#ifndef __MUPARSER_INTERFACE_H
#define __MUPARSER_INTERFACE_H

#include <string>
#include <array>

#include "muParser.h"

namespace MyFunctions
{
	template <size_t dim>
		void AssignVariablesName(std::array<double, dim>&, mu::Parser&);

	template <size_t dim>
		class MuParserInterface
		{
			public:
				MuParserInterface();
				MuParserInterface(const std::string&);
				~MuParserInterface();
				MuParserInterface(const MuParserInterface&);
				MuParserInterface& operator = (const MuParserInterface&);

				void Expression(const std::string&);
				std::string Expression()const;
				double operator() (const std::array<double, dim>&) const;

			protected:
				mu::Parser _parser;
				std::string _expr;

			private:
				//It's modified by the derived method operator(), which is labeled const
				mutable std::array<double, dim> _variables;
		};

	template <size_t dim>
		MuParserInterface<dim>::MuParserInterface()
		{
			AssignVariablesName<dim> (this->_variables, this->_parser);
		};

	template <size_t dim>
		MuParserInterface<dim>::MuParserInterface(const std::string& s) : MuParserInterface()
		{
			this->Expression(s);
		};

	template <size_t dim>
		MuParserInterface<dim>::~MuParserInterface()
		{
			this->_parser.ClearVar();
		};

	template <size_t dim>
		MuParserInterface<dim>::MuParserInterface(const MuParserInterface<dim>& mpi)
		{
			(*this) = mpi;
		};

	template <size_t dim>
		MuParserInterface<dim>& MuParserInterface<dim>::operator= (const MuParserInterface<dim>& mpi)
		{
			if (this != &mpi)
			{
				this->_parser.ClearVar();
				this->_expr = mpi._expr;
				this->_variables = mpi._variables;
				this->_parser.SetExpr(this->_expr);
				AssignVariablesName<dim> (this->_variables, this->_parser);
			}
			return *this;
		};

	template <size_t dim>
		void MuParserInterface<dim>::Expression(const std::string& s)
		{
			this->_expr = s;
			this->_parser.SetExpr(s);
		};

	template <size_t dim>
		std::string MuParserInterface<dim>::Expression()const
		{
			return this->_expr;
		};

	template <size_t dim>
		double MuParserInterface<dim>::operator() (const std::array<double, dim>& values) const
		{
			this->_variables = values;
			return this->_parser.Eval();
		};

	template <size_t dim>
		void AssignVariablesName(std::array<double, dim>& container, mu::Parser& mpi)
		{
			size_t cont(0);
			for(auto& var : container)
				mpi.DefineVar("x" + std::to_string(++cont), &var);
		};

	template <>
		void AssignVariablesName<1>(std::array<double, 1>&, mu::Parser&);

	template <>
		void AssignVariablesName<2>(std::array<double, 2>&, mu::Parser&);

} //namespace MyFunctions
#endif //__MUPARSER_INTERFACE_H
