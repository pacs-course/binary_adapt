#include "MuParserInterface.h"

using namespace std;

namespace Functions
{
	template <>
	void AssignVariablesName<1> (std::array<double, 1>& container,
								 mu::Parser& mpi)
	{
		mpi.DefineVar ("x", & (container[0]));
	};

	template <>
	void AssignVariablesName<2> (std::array<double, 2>& container,
								 mu::Parser& mpi)
	{
		mpi.DefineVar ("x", & (container[0]));
		mpi.DefineVar ("y", & (container[1]));
	};

} //namespace Functions
