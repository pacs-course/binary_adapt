#ifndef __BINARY_TREE_HELPER_H
#define __BINARY_TREE_HELPER_H

#include "TypeEnumerations.h"

#include <memory> //std::shared_ptr, std::make_shared, std::make_unique
#include <functional> //std::function
#include <fstream> //std::basic_streambuf, std::ostream
#include <iostream>
#include <string> //std::string


namespace Helpers
{
/**
	Function behaving as std::make_unique.
	MakeUnique gives an interface for a function which behaves exactly as std::make_unique
	(available since standard c++14). 
	If the compiling options make available the std::make_unique, it simply returns its output.
**/
	template <class T, class... Args>
	std::unique_ptr<T> MakeUnique(Args&&... args)
	{
#ifdef __cpp_lib_make_unique
		return std::make_unique<T>(args...);
#else
		return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
#endif
	};

/**
		Class which redirects buffers on file.
		Logfile logfile_object(filename) automatically riderects buffers to filename.
		The destruction of logfile_object redirects back buffers to std ones, restoring the original state  
**/
	class Logfile
	{
		public:
/**
			constructor.
			The constructor of a Logfile objects redirects std::cout, std::clog, std::cerr to file;
			the file name is passed as input.
			Each std buffer is stored in correspondent Logfile attribute
**/
			Logfile(const std::string&);
/**
			destructor.
			When the Logfile object is destroyed buffers are redirected to std ones
**/
			~Logfile();

		private:
/**
			The output file stream where buffers are redirected
**/
			std::ofstream _ofs;
/**
			Attribute to store std::cout
**/
			std::basic_streambuf<char>* _out_buf;
/**
			Attribute to store std::clog
**/
			std::basic_streambuf<char>* _log_buf;
/**
			Attribute to store std::cerr
**/
			std::basic_streambuf<char>* _err_buf;
	};
	
/**
	Builders to be registered in the factories
**/
	template <typename ConcreteProduct, typename AbstractProduct = ConcreteProduct>
		struct Builders
		{
/**
			It makes sure that only one element is constructed.
**/
			static std::shared_ptr<AbstractProduct> BuildSingleton()
			{
				static std::shared_ptr<AbstractProduct> ptr = std::make_shared<ConcreteProduct>();
				return ptr;
			};

			static std::unique_ptr<AbstractProduct> BuildObject()
			{
				return MakeUnique<ConcreteProduct>();
			};
		};

/**
	Function to convert BasisType enum into string
**/
	std::string BasisTypeConverter(FiniteElements::BasisType const & id);
/**
	Function to convert ElementType enum into string
**/
	std::string ElementTypeConverter(Geometry::ElementType const & id);

/**
	An optimized function to compute powers when the exponent is an unsigned integer
	and it is known at runtime
**/
	inline double IntPower (double basis, size_t exp)
	{
#ifdef BASIC
		double power = 1;
		if (exp)
		{
			power = basis;
			for (size_t j = 1; j < exp; ++j)
				power *= basis;
		}
		return power;
#else //BASIC

		if (exp == 0)
			return 1.0;
		if (exp == 1)
			return basis;
#ifdef NOT_RECURSIVE
		double power = basis;
		size_t cur_exp = 2;
		for (; cur_exp <= exp; cur_exp *= 2)
			power *= power;

		for (cur_exp /= 2; cur_exp < exp; ++cur_exp)
			power *= basis;
		return power;
#else //NOT_RECURSIVE

		if (exp == 2)
			return basis * basis;

		double half_exp_result = IntPower(basis * basis, exp/2);
		return exp % 2 ? basis * half_exp_result : half_exp_result;
#endif //NOT_RECURSIVE

#endif //BASIC
	};

/**
		An optimized function to compute powers when the exponent is an unsigned integer
		and it is known at compile time
**/
	template <size_t exp>
		double Power (double basis)
		{
			double result = 
			exp % 2 ? basis * Power<exp/2> (basis*basis) : Power<exp/2> (basis*basis);
			return result;
		};

	template <>
		double Power<0> (double);

	template <>
		double Power<1> (double);

	template <>
		double Power<2> (double);
	

} //namespace Helpers

#endif //__BINARY_TREE_HELPER_H
