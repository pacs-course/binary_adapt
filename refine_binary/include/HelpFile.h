#ifndef __HELPFILE_H
#define __HELPFILE_H

#include <fstream> //basic_streambuf, ostream


#ifdef LIBMESH_BUG_FIXED
	#include <GetPot>
#else
	#include "libmesh/getpot.h"
#endif

namespace Helpers
{
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
		Logfile (const std::string&);
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
			Attribute to store cout
		**/
		std::basic_streambuf<char>* _out_buf;
		/**
			Attribute to store clog
		**/
		std::basic_streambuf<char>* _log_buf;
		/**
			Attribute to store cerr
		**/
		std::basic_streambuf<char>* _err_buf;
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
		Cfgfile (const std::string&);

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
		int	operator()(const std::string, int) const;
		/**
			Find the double value of the input string ID
		**/
		double operator()(const std::string, const double&) const;
		/**
			Find the string value of the input string ID
		**/
		const std::string operator()(const std::string, const std::string) const;

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

}

#endif //__HELPFILE_H
