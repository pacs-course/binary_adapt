#ifndef __MUPARSER_INTERFACE_H
#define __MUPARSER_INTERFACE_H

#include <string>
#include <array>

#include "muParser.h"

namespace Functions
{
	/**
		Initialize mu::Parser variables with array elements addresses and names.
		It assigns the name to variables, as they should be called in the expression.
		Called by MuParserInterface constructor.
	**/
	template <size_t dim>
	void AssignVariablesName (std::array<double, dim>&, mu::Parser&);

	/**
		Wrapper for muParser library.
		It implements an interface to use muParser objects.
		It is a functor with dimensionality dim,
		when called operator() it evaluates	the mathematical Expression() at input points.
	**/
	template <size_t dim>
	class MuParserInterface
	{
	  public:
		/**
			constructor.
			It initializes _parser variables with _variables content.
		**/
		MuParserInterface();
		/**
			constructor with string.
			As for default constructor, but it initializes _expr expression too.
		**/
		MuParserInterface (const std::string&);
		/**
			destructor
		**/
		~MuParserInterface();
		/**
			copy constructor
		**/
		MuParserInterface (const MuParserInterface&);
		/**
			assignment operator
		**/
		MuParserInterface& operator = (const MuParserInterface&);

		/**
			set the mathematical expression
		**/
		void Expression (const std::string&);
		/**
			get the mathematical expression
		**/
		std::string Expression()const;
		/**
			evaluate the mathematical expression at input values
		**/
		double operator() (const std::array<double, dim>&) const;

	  protected:
		/**
			parser of mathematical expression.
			It needs the addresses where to store the variables values,
			which is set to _variables elements.
			The underlying mathematical expression is set to _expr string.
			Call _parser.Eval() to evaluate the expression at variables values.
		**/
		mu::Parser _parser;
		/**
			mathematical expression
		**/
		std::string _expr;

	  private:
		/**
			Location for variables values.
			Here _parser variables are stored, needed when evaluating operator() method.
			operator(), which is labeled const because it's called by Functor<dim>::operator() const method,
			needs to modify this attribute: so it's labeled mutable.
		**/
		mutable std::array<double, dim> _variables;
	};

	template <size_t dim>
	MuParserInterface<dim>::MuParserInterface()
	{
		AssignVariablesName<dim> (this->_variables, this->_parser);
	};

	template <size_t dim>
	MuParserInterface<dim>::MuParserInterface (const std::string& s) :
		MuParserInterface()
	{
		this->Expression (s);
	};

	template <size_t dim>
	MuParserInterface<dim>::~MuParserInterface()
	{
		this->_parser.ClearVar();
	};

	template <size_t dim>
	MuParserInterface<dim>::MuParserInterface (const MuParserInterface<dim>& mpi)
	{
		(*this) = mpi;
	};

	template <size_t dim>
	MuParserInterface<dim>& MuParserInterface<dim>
	::operator= (const MuParserInterface<dim>& mpi)
	{
		if (this != &mpi)
		{
			this->_parser.ClearVar();
			this->_expr = mpi._expr;
			this->_variables = mpi._variables;
			this->_parser.SetExpr (this->_expr);
			AssignVariablesName<dim> (this->_variables, this->_parser);
		}
		return *this;
	};

	template <size_t dim>
	void MuParserInterface<dim>::Expression (const std::string& s)
	{
		this->_expr = s;
		this->_parser.SetExpr (s);
	};

	template <size_t dim>
	std::string MuParserInterface<dim>::Expression() const
	{
		return this->_expr;
	};

	template <size_t dim>
	double MuParserInterface<dim>::operator() (
		const std::array<double, dim>& values) const
	{
		this->_variables = values;
		return this->_parser.Eval();
	};

	/**
		By default variables are called xi, with i = 1:dim
	**/
	template <size_t dim>
	void AssignVariablesName (std::array<double, dim>& container,
							  mu::Parser& mpi)
	{
		size_t cont (0);
		for (auto& var : container)
			mpi.DefineVar ("x" + std::to_string (++cont), &var);
	};

	/**
		If dim==1 variable is called x
	**/
	template <>
	void AssignVariablesName<1> (std::array<double, 1>&, mu::Parser&);

	/**
		If dim==2 variables are called x and y
	**/
	template <>
	void AssignVariablesName<2> (std::array<double, 2>&, mu::Parser&);

} //namespace Functions
#endif //__MUPARSER_INTERFACE_H
