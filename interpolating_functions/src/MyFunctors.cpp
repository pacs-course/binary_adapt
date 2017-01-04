#include "MyFunctors.h"

#include <math.h>

namespace MyFunctions
{
	double SqrtX::operator() (const Geometry::Point<1>& p)const
	{
		return sqrt(p[0]);
	};

	double HalfStep::operator() (const Geometry::Point<1>& p)const
	{
		double val = p[0];
		if (val > 0.5)
			return 1.0;
		return 0.0;
	};

} //namespace MyFunctions
