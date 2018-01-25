#include "BinaryTreeHelper.h"

#include <sys/stat.h>

using namespace std;

namespace Helpers
{

	Logfile::Logfile (const string& filename) : _ofs (filename, ios_base::app)
		, _out_buf (cout.rdbuf (_ofs.rdbuf()))
		, _log_buf (clog.rdbuf (_ofs.rdbuf()))
		, _err_buf (cerr.rdbuf (_ofs.rdbuf()))
	{}

	Logfile::~Logfile()
	{
		clog.rdbuf (_log_buf);
		cout.rdbuf (_out_buf);
		cerr.rdbuf (_err_buf);
	}


	Cfgfile::Cfgfile (const string& filename)
	{
		// WARNING:
		// the GetPot version provided with libmesh raises an exception if the file does not exist
		// instead, the official GetPot silently ignores it
		// in order to be compatible with both of them, we explicitely check that the file exists

		struct stat buffer;   
		if (stat (filename.c_str(), &buffer) == 0)
		{
			_ok = true;
			_cl = GetPot(filename.c_str());
		}
		else
		{
			_ok = false;
			//throw runtime_error ("Configuration file " + filename + " does not exist.");
		}

	}

	Cfgfile::Cfgfile (int argc, char** argv)
	{
		_cl = GetPot(argc, argv);
		_ok = true;
	}

	Cfgfile::~Cfgfile()
	{}

	int	
	Cfgfile::operator()(const string var_name, int default_val) const
	{
		if (_ok)
			return _cl(var_name.c_str(), default_val);
		else
			return default_val;
	}

	double
	Cfgfile::operator()(const string var_name, const double& default_val) const
	{
		if (_ok)
			return _cl(var_name.c_str(), default_val);
		else
			return default_val;
	}

	const string
	Cfgfile::operator()(const string var_name, const string default_val) const
	{
		if (_ok)
			return _cl(var_name.c_str(), default_val);
		else
			return default_val;
	}

	bool Cfgfile::HasHelp() const
	{
		auto __cl(_cl);
		return __cl.search (2, "--help", "-h");
	}

	Cfgfile::operator bool()
	{
		return _ok;
	}

	Cfgfile::operator GetPot()
	{
		return _cl;
	}


	string BasisTypeConverter (FiniteElements::BasisType const& id)
	{
		switch (id)
		{
			case (FiniteElements::TrivialType)		: return "TrivialType";
			case (FiniteElements::LegendreType)		: return "LegendreType";
			case (FiniteElements::WarpedType)		: return "WarpedType";
			case (FiniteElements::InvalidFeType)	: return "InvalidFeType";
			case (FiniteElements::ThirdPartyFeType)	: return "ThirdPartyFeType";
		}
		return "NULLSTRING";
	}

	string ElementTypeConverter (Geometry::ElementType const& id)
	{
		switch (id)
		{
			case (Geometry::IntervalType)			: return "IntervalType";
			case (Geometry::SquareType)				: return "SquareType";
			case (Geometry::TriangleType)			: return "TriangleType";
			case (Geometry::CubeType)				: return "CubeType";
			case (Geometry::InvalidElementType)		: return "InvalidElementType";
			case (Geometry::ThirdPartyElementType)	: return "ThirdPartyElementType";
		}
		return "NULLSTRING";
	}


	template <>
	double Power<0> (double)
	{
		return 1.0;
	}

	template <>
	double Power<1> (double basis)
	{
		return basis;
	}

	template <>
	double Power<2> (double basis)
	{
		return basis * basis;
	}

} //namespace Helpers
