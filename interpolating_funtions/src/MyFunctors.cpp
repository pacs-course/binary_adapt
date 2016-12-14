#include "MyFunctors.h"

#include <math.h>

namespace MyFunctions
{
	double SqrtX::operator() (const Geometry::Point<1>& p)
	{
		return sqrt(p[0]);
	};

} //namespace MyFunctions
