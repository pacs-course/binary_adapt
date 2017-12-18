#ifndef __INTERPOLATING_FUNCTIONS_H
#define __INTERPOLATING_FUNCTIONS_H

#include "Functor.h"
#include "MuParserInterface.h"
#include "BinaryTreeHelper.h"

#ifdef LIBMESH_BUG_FIXED
	#include <GetPot>
#else
	#include "libmesh/getpot.h"
#endif

/**
	It contains implementation of functor objects that can be interpolated by the binary tree algorithm
**/
namespace Functions
{
	using namespace std;

	/**
		Functor for mathematical expression parsed from file.
	**/
	template <size_t dim>
	class ParserFunctor : public BinaryTree::Functor<dim>
	{
	  public:
		/**
			constructor.
			It gets the underlying mathematical expression from
			interpolating_functions/include/interpolating_functions.conf configuration file.
			It raises an exception if the expression is not valid.
		**/
		ParserFunctor();

		/**
			default destructor
		**/
		virtual ~ParserFunctor();

		/**
			Constructor from string.
			This builder will not be registered in Functor factory.
			To use it the direct include of this file is needed.
			This constructor is needed for testing purpose.
		**/
		ParserFunctor (const std::string&);

		virtual double operator() (const Geometry::Point<dim>&) const override;

		virtual std::string Formula() const override;

		virtual std::string ID() const override;

	  protected:
		/**
			Check the semantic validity of the mu parser expression.
			It raises runtime_error exception in case of invalid expression.
		**/
		void CheckExpressionValidity();

	  protected:
		/**
			The muParser wrapper object
		**/
		MuParserInterface<dim> _parser;
	};


	/**
		Functor for exponential with integer exponent
	**/
	template <size_t exp>
	class XExpBeta : public BinaryTree::Functor<1>
	{
	  public:
		XExpBeta();
		virtual ~XExpBeta();
		virtual double operator() (const Geometry::Point<1>&) const override;
		virtual std::string Formula() const override;
		virtual std::string ID() const override;
	};

	/**
		Function for the analitycal solution of 1D advection diffusion problem with
		homogeneus Dirichlet boundary conditions
	**/
	template <int b_mu>
	class AdvectionDiffusionSolution : public BinaryTree::Functor<1>
	{
	  public:
		AdvectionDiffusionSolution();
		virtual ~AdvectionDiffusionSolution();
		virtual double operator() (const Geometry::Point<1>&) const override;
		virtual std::string Formula() const override;
		virtual std::string ID() const override;
	};

	using XSquared = XExpBeta<2>;

	/**
		Functor for the step function with step in x = 0.5
	**/
	class HalfStep : public BinaryTree::Functor<1>
	{
	  public:
		HalfStep();
		virtual ~HalfStep();
		virtual double operator() (const Geometry::Point<1>&)const override;
		virtual std::string Formula()const override;
		virtual std::string ID()const override;
	};

	/**
		Functor for square root function
		multiplied by the step function with step in x = 0.5
	**/
	class HalfSqrt : public BinaryTree::Functor<1>
	{
	  public:
		HalfSqrt();
		virtual ~HalfSqrt();
		virtual double operator() (const Geometry::Point<1>&)const override;
		virtual std::string Formula()const override;
		virtual std::string ID()const override;
	};

	/**
		Functor for identity function
		multiplied by the step function with step in x = 0.5
	**/
	class HalfX : public BinaryTree::Functor<1>
	{
	  public:
		HalfX();
		virtual ~HalfX();
		virtual double operator() (const Geometry::Point<1>&)const override;
		virtual std::string Formula()const override;
		virtual std::string ID()const override;
	};

	/**
		Functor for square function
		multiplied by the step function with step in x = 0.5
	**/
	class HalfSquare : public BinaryTree::Functor<1>
	{
	  public:
		HalfSquare();
		virtual ~HalfSquare();
		virtual double operator() (const Geometry::Point<1>&)const override;
		virtual std::string Formula()const override;
		virtual std::string ID()const override;
	};

	/**
		Functor for twentieth power function
		multiplied by the step function with step in x = 0.5
	**/
	class HalfTwenty : public BinaryTree::Functor<1>
	{
	  public:
		HalfTwenty();
		virtual ~HalfTwenty();
		virtual double operator() (const Geometry::Point<1>&)const override;
		virtual std::string Formula()const override;
		virtual std::string ID()const override;
	};

	/**
		Functor for square root function
	**/
	class SqrtX : public BinaryTree::Functor<1>
	{
	  public:
		SqrtX();
		virtual ~SqrtX();

		virtual double operator() (const Geometry::Point<1>&)const override;
		virtual std::string Formula()const override;
		virtual std::string ID()const override;
	};

	/**
		Functor for x squared plus y squared 2D function
	**/
	class X2PlusY2 : public BinaryTree::Functor<2>
	{
	  public:
		X2PlusY2();
		virtual ~X2PlusY2();

		virtual double operator() (const Geometry::Point<2>&)const override;
		virtual std::string Formula()const override;
		virtual std::string ID()const override;
	};


	template <size_t dim>
	ParserFunctor<dim>::ParserFunctor()
	{
		string thisfile = __FILE__;
		string conf_file = thisfile.substr (0, thisfile.find_last_of ('/'))
						 + "/interpolating_functions.conf";
#ifdef DEBUG
		cerr << "ParserFunctor: opening configuration file " << conf_file << endl;
#endif //DEBUG
		GetPot cl (conf_file.c_str());
		string expr = std::to_string (dim) + "D/mu_parser_expr";

		this->_parser.Expression (cl (expr.c_str(), ""));

		if (this->_parser.Expression() == "")
			throw runtime_error (
				"Error reading the configuration file in ParserFunctor: \
				 empty string expression");

		/*	I try to call the _parser operator() to check the correctness of the expression
			I don't want to make this check every time I call ParserFunctor::operator()
			I do it once here */
		CheckExpressionValidity();
	};

	template <size_t dim>
	ParserFunctor<dim>::~ParserFunctor()
	{};

	template <size_t dim>
	ParserFunctor<dim>::ParserFunctor (const std::string& s) : _parser (s)
	{
		CheckExpressionValidity();
	};

	template <size_t dim>
	double ParserFunctor<dim>::operator() (const Geometry::Point<dim>& p) const
	{
		return (this->_parser) (static_cast<array<double, dim>> (p));
	};

	template <size_t dim>
	std::string ParserFunctor<dim>::Formula() const
	{
		return this->_parser.Expression();
	};

	template <size_t dim>
	std::string ParserFunctor<dim>::ID() const
	{
		return "RENAME_ME_parsed_functor";
	};


	template <size_t dim>
	void ParserFunctor<dim>::CheckExpressionValidity()
	{
		try
		{
			std::array<double, dim> test;
			test.fill (0.0);
			this->_parser (test);
		}
		catch (mu::Parser::exception_type& e)
		{
			std::string error =	"Content of muParser exception"				+ std::string ("\n")
							  +	"Message:  " + std::string (e.GetMsg())		+ std::string ("\n")
							  +	"Formula:  " + std::string (e.GetExpr())	+ std::string ("\n")
							  +	"Token:    " + std::string (e.GetToken())	+ std::string ("\n")
							  +	"Position: " + std::to_string (e.GetPos())	+ std::string ("\n")
							  +	"Errc:     " + std::to_string (e.GetCode())	+ std::string ("\n");

			//I prefer to throw a runtime_error, since outside this plugin I don't see the mu:: namespace
			throw std::runtime_error (error);
		}
	};

	template <size_t exp>
	XExpBeta<exp>::XExpBeta()
	{};

	template <size_t exp>
	XExpBeta<exp>::~XExpBeta()
	{};

	template <size_t exp>
	double XExpBeta<exp>::operator() (const Geometry::Point<1>& p) const
	{
		return Helpers::Power<exp> (p);
	};

	template <size_t exp>
	std::string XExpBeta<exp>::Formula() const
	{
		return "x^" + std::to_string (exp);
	};

	template <size_t exp>
	std::string XExpBeta<exp>::ID() const
	{
		return "x_" + std::to_string (exp);
	};

	template <int b_mu>
	AdvectionDiffusionSolution<b_mu>::AdvectionDiffusionSolution()
	{};

	template <int b_mu>
	AdvectionDiffusionSolution<b_mu>::~AdvectionDiffusionSolution()
	{};

	template <int b_mu>
	double AdvectionDiffusionSolution<b_mu>
	::operator() (const Geometry::Point<1>& p) const
	{
		return (exp (b_mu * static_cast<double> (p)) - 1) / (exp (b_mu) - 1);
	};

	template <int b_mu>
	std::string AdvectionDiffusionSolution<b_mu>::Formula() const
	{
		return "(exp(" + std::to_string (b_mu) + "*x) - 1 ) / (exp("
			+ std::to_string (b_mu) + ") - 1)";
	};

	template <int b_mu>
	std::string AdvectionDiffusionSolution<b_mu>::ID() const
	{
		return "adv_diff_" + std::to_string (b_mu);
	};

} //namespace Functions

#endif //__INTERPOLATING_FUNCTIONS_H
