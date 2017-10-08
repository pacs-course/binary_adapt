#ifndef __INTERPOLATING_FUNCTIONS_H
#define __INTERPOLATING_FUNCTIONS_H

#include "Functor.h"
#include "MuParserInterface.h"

//#include <GetPot>
#include "getpot.h"
namespace MyFunctions
{	
	using namespace std;

	template <size_t dim>
		class ParserFunctor : public BinaryTree::Functor<dim>
		{
			public:
				ParserFunctor()
				{
					string thisfile = __FILE__;
					string conf_file = thisfile.substr(0, thisfile.find_last_of('/')) + "/interpolating_functions.conf";
#ifdef DEBUG
					cerr << "ParserFunctor: opening configuration file " << conf_file << endl;
#endif //DEBUG
					GetPot cl(conf_file.c_str());
					string expr = std::to_string(dim) + "D/mu_parser_expr";

					this->_parser.Expression(cl(expr.c_str(), ""));

					if (this->_parser.Expression() == "")
						throw runtime_error("Error reading the configuration file in ParserFunctor: empty string expression");

					//I try to call the _parser operator() to check the correctness of the expression
					//I don't want to make this check every time I call ParserFunctor::operator()
					//I do it once here
					CheckExpressionValidity();
				};

				//needed for testing purpose
				ParserFunctor(const std::string& s) : _parser(s)
				{
					CheckExpressionValidity();
				};

				virtual double operator() (const Geometry::Point<dim>& p) const override
				{
					return (this->_parser)(p.get());
				};

				virtual std::string Formula() const override
				{
					return this->_parser.Expression();
				};

				virtual std::string ID() const override
				{
					return "RENAME_ME_parsed_functor";
				};


			protected:
				void CheckExpressionValidity()
				{
					try
					{
						std::array<double, dim> test;
						test.fill(0.0);
						//to avoid warning by the compiler
						(void) this->_parser(test);
					}
					catch(mu::Parser::exception_type& e)
					{
						std::string error 	=	"Content of muParser exception"					+ std::string("\n")
													+	"Message:  "	+ std::string(e.GetMsg())		+ std::string("\n")
													+	"Formula:  "	+ std::string(e.GetExpr())		+ std::string("\n")
													+	"Token:    "	+ std::string(e.GetToken())	+ std::string("\n")
													+	"Position: "	+ std::to_string(e.GetPos())	+ std::string("\n")
													+	"Errc:     "	+ std::to_string(e.GetCode())	+ std::string("\n");

						//I prefer to throw a runtime_error, since outside this plugin I don't see the mu:: namespace
						throw std::runtime_error(error);
					}
				};

			protected:
				MuParserInterface<dim> _parser;
		};

	template <int exp>
		class XExpBeta : public BinaryTree::Functor<1>
		{
			public:
				XExpBeta(){};
				virtual double operator() (const Geometry::Point<1>& p)const override
				{
					double result(1.0);
					for(std::size_t e(0); e < exp; ++e)
						result *= p[0];
					
					return result;
				};

				virtual std::string Formula()const override
				{
					return "x^" + std::to_string(exp);
				};
				virtual std::string ID()const override	
				{
					return "x_" + std::to_string(exp);
				};
		};

	template <int b_mu>
		class AdvectionDiffusionSolution : public BinaryTree::Functor<1>
		{
			public:
				AdvectionDiffusionSolution(){};
				virtual double operator() (const Geometry::Point<1>& p)const override
				{
					return (exp(b_mu * p[0]) - 1) / (exp(b_mu) - 1);
				};

				virtual std::string Formula()const override
				{
					return "(exp(" + std::to_string(b_mu) + "*x) - 1 ) / (exp(" + std::to_string(b_mu) + ") - 1)";
				};
				virtual std::string ID()const override
				{
					return "adv_diff_" + std::to_string(b_mu);
				};
		};

	using XSquared = XExpBeta<2>;

	class HalfStep : public BinaryTree::Functor<1>
	{
		public:
			HalfStep(){};
			virtual double operator() (const Geometry::Point<1>&)const override;
			virtual std::string Formula()const override;
			virtual std::string ID()const override;
	};

	class HalfSqrt : public BinaryTree::Functor<1>
	{
		public:
			HalfSqrt(){};
			virtual double operator() (const Geometry::Point<1>&)const override;			
			virtual std::string Formula()const override;
			virtual std::string ID()const override;
	};

	class HalfX : public BinaryTree::Functor<1>
	{
		public:
			HalfX(){};
			virtual double operator() (const Geometry::Point<1>&)const override;			
			virtual std::string Formula()const override;
			virtual std::string ID()const override;
	};

	class HalfSquare : public BinaryTree::Functor<1>
	{
		public:
			HalfSquare(){};
			virtual double operator() (const Geometry::Point<1>&)const override;			
			virtual std::string Formula()const override;
			virtual std::string ID()const override;
	};

	class HalfTwenty : public BinaryTree::Functor<1>
	{
		public:
			HalfTwenty(){};
			virtual double operator() (const Geometry::Point<1>&)const override;			
			virtual std::string Formula()const override;
			virtual std::string ID()const override;
	};

	class SqrtX : public BinaryTree::Functor<1>
	{
		public:
			SqrtX(){};
			virtual double operator() (const Geometry::Point<1>&)const override;
			virtual std::string Formula()const override;
			virtual std::string ID()const override;
	};

	class X2PlusY2 : public BinaryTree::Functor<2>
	{
		public:
			X2PlusY2(){};
			virtual double operator() (const Geometry::Point<2>&)const override;
			virtual std::string Formula()const override;
			virtual std::string ID()const override;
	};

} //namespace MyFunctions

#endif //__INTERPOLATING_FUNCTIONS_H
