#ifndef __BINARY_TREE_HELPER_H
#define __BINARY_TREE_HELPER_H

#include "TypeEnumerations.h"

#include <memory>//shared_ptr, make_shared, make_unique
#include <functional> //function
#include <fstream> //basic_streambuf, ostream
#include <iostream>
#include <string> //string


#ifdef LIBMESH_BUG_FIXED
	#include <GetPot>
#else
	#include "libmesh/getpot.h"
#endif


/**
	Helpful tools, classes and functions needed by the library objects.
**/
namespace Helpers
{
	using namespace std;

	/**
		Function behaving as make_unique.
		MakeUnique gives an interface for a function which behaves exactly as
		make_unique [available since standard c++14].
		If the compiling options make available the make_unique,
		it simply returns its output.
	**/
	template <class T, class... Args>
	unique_ptr<T> MakeUnique (Args&& ... args)
	{
#ifdef __cpp_lib_make_unique
		return make_unique<T> (args...);
#else
		return unique_ptr<T> (new T (forward<Args> (args)...));
#endif
	};

	/**
		Class which redirects buffers on file.
		Logfile logfile_object(filename) automatically riderects buffers to filename.
		The destruction of logfile_object redirects back buffers to std ones,
		restoring the original state
	**/
	class Logfile
	{
	  public:
		/**
			constructor.
			The constructor of a Logfile objects redirects
			cout, clog, cerr to file;
			the file name is passed as input.
			Each std buffer is stored in correspondent Logfile attribute
		**/
		Logfile (const string&);
		/**
			destructor.
			When the Logfile object is destroyed buffers are redirected to std ones
		**/
		~Logfile();

	  private:
		/**
			The output file stream where buffers are redirected
		**/
		ofstream _ofs;
		/**
			Attribute to store cout
		**/
		basic_streambuf<char>* _out_buf;
		/**
			Attribute to store clog
		**/
		basic_streambuf<char>* _log_buf;
		/**
			Attribute to store cerr
		**/
		basic_streambuf<char>* _err_buf;
	};

	/**
		Proxy class wrapping access to GetPot.
		Provides a string interface to the GetPot methods,
		and checks for existence of the input configuration file before creating the GetPot object.
	**/
	class Cfgfile
	{
	  public:
		/**
			constructor from filename.
			If the file in input does not exist, raises an exception
		**/
		Cfgfile (const string&);

		/**
			constructor from main parameters.
		**/
		Cfgfile (int argc, char** argv);

		/**
			destructor.
		**/
		~Cfgfile();

		/**
			Find the int value of the input string ID
		**/
		int					operator()(const string, int) const;
		/**
			Find the double value of the input string ID
		**/
		double				operator()(const string, const double&) const;
		/**
			Find the string value of the input string ID
		**/
		const string	operator()(const string, const string) const;

//		template <typename T>
//		T operator() (const string var_name, const T& default_val) const
//		{
//			if (_ok)
//				return _cl(var_name.c_str(), default_val);
//			else
//				return default_val;
//		};

		/**
			Return true if -h or --help option is found in GetPot object
		**/
		bool HasHelp() const;

		/**
			Return true if the configuration file has been found
		**/
		operator bool();
		/**
			Explicit cast to GetPot type object
		**/
		operator GetPot();

	  private:
		/**
			The GetPot object handling the configuration
		**/
		GetPot _cl;
		/**
			Flag telling if configuration file has been found 
		**/
		bool _ok;
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
		static shared_ptr<AbstractProduct> BuildSingleton()
		{
			static shared_ptr<AbstractProduct> ptr =
				make_shared<ConcreteProduct>();
			return ptr;
		};

		static unique_ptr<AbstractProduct> BuildObject()
		{
			return MakeUnique<ConcreteProduct>();
		};
	};

	/**
		Function to convert BasisType enum into string
	**/
	string BasisTypeConverter (FiniteElements::BasisType const& id);
	/**
		Function to convert ElementType enum into string
	**/
	string ElementTypeConverter (Geometry::ElementType const& id);

	/**
		An optimized function to compute powers when the exponent is
		an unsigned integer and it is known at runtime
	**/
	inline double IntPower (double basis, size_t exp)
	{
		if (exp == 0)
			return 1.0;
		if (exp == 1)
			return basis;
		if (exp == 2)
			return basis * basis;

		double half_exp_result = IntPower (basis * basis, exp / 2);
		return exp % 2 ? basis * half_exp_result : half_exp_result;
	};

	/**
		An optimized function to compute powers when the exponent is
		an unsigned integer and it is known at compile time
	**/
	template <size_t exp>
	double Power (double basis)
	{
		double result =
			exp % 2 ? basis * Power < exp / 2 > (basis * basis) : Power < exp / 2 >
			(basis * basis);
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
