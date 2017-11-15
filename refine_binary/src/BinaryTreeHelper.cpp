#include "BinaryTreeHelper.h"

using namespace std;

namespace Helpers
{

	Logfile::Logfile(const string& filename) : _ofs(filename, ios_base::app)
															,_out_buf(cout.rdbuf(_ofs.rdbuf()))
															,_log_buf(clog.rdbuf(_ofs.rdbuf()))
															,_err_buf(cerr.rdbuf(_ofs.rdbuf()))
	{
#ifdef MYDEBUG
		clog << "I buffer sono su file" << endl;
#endif //MYDEBUG
	};

	Logfile::~Logfile()
	{
#ifdef MYDEBUG
		clog << "Redirigo i buffer sullo standard" << endl;
#endif //MYDEBUG
		clog.rdbuf(_log_buf);
		cout.rdbuf(_out_buf);
		cerr.rdbuf(_err_buf);
#ifdef MYDEBUG
		cerr << "Buffer rediretti" << endl;
#endif //MYDEBUG
	};

	string BasisTypeConverter(FiniteElements::BasisType const & id)
	{
		switch(id)
		{
			case(FiniteElements::TrivialType)		: return "TrivialType";
			case(FiniteElements::LegendreType)		: return "LegendreType";  
			case(FiniteElements::WarpedType)			: return "WarpedType";  
			case(FiniteElements::InvalidFeType)		: return "InvalidFeType";
			case(FiniteElements::ThirdPartyFeType)	: return "ThirdPartyFeType";
		}
		return "NULLSTRING";
	};

	string ElementTypeConverter(Geometry::ElementType const & id)
	{
		switch(id)
		{
			case(Geometry::IntervalType)				: return "IntervalType";
			case(Geometry::SquareType)					: return "SquareType";
			case(Geometry::TriangleType)				: return "TriangleType";
			case(Geometry::CubeType)					: return "CubeType";  
			case(Geometry::InvalidElementType)		: return "InvalidElementType";
			case(Geometry::ThirdPartyElementType)	: return "ThirdPartyElementType";
		}
		return "NULLSTRING";
	};


	template <>
		double Power<0> (double)
		{
			return 1.0;
		};

	template <>
		double Power<1> (double basis)
		{
			return basis;
		};

	template <>
		double Power<2> (double basis)
		{
			return basis * basis;
		};

} //namespace Helpers
