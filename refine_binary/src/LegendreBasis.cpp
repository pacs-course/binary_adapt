#include "LegendreBasis.h"

//jacobi_polynomial.hpp need the "using namespace std" before the include
using namespace std;

#include "jacobi_polynomial.hpp"


namespace FiniteElements
{
	double LegendreEvaluator::SingleEvaluation (size_t index, double x)
	{
#ifdef VERBOSE
		static bool called = true;
		if (called)
		{
			cerr << "Warning: OneDEvaluation method in "
				 << "LegendreBasis is deprecated because of inefficiency"
				 << endl;
			called = false;
		}
#endif //VERBOSE

#ifndef INTERNAL_LEGENDRE
		unique_ptr<double[]> eval (j_polynomial (1, index, 0, 0, &x));
#else
		unique_ptr<double[]> eval = move (ManualEvaluation (index, x));
#endif
		return eval[index];
	}

	unique_ptr<double[]> LegendreEvaluator::Evaluate (size_t degree, double x)
	{
#ifndef INTERNAL_LEGENDRE
		return move (std::unique_ptr<double[]> (j_polynomial (1, degree, 0, 0, &x)));
#else
		return move (ManualEvaluation (degree, x));
#endif
	}

#ifdef INTERNAL_LEGENDRE
	unique_ptr<double[]> LegendreEvaluator::ManualEvaluation (size_t index,
															  double x)
	{
		unique_ptr<double[]> result (new double[index + 1]);

		double previous_value = 1.0;
		double value = x;
		result[0] = previous_value;

		if (index >= 1)
			result[1] = value;

		for (size_t k = 1; k < index; ++k)
		{
			double k_val = static_cast<double> (k);
			double temp = previous_value;
			previous_value = value;
			value = ( (2 * k_val + 1) * x * value - k_val * temp) / (k_val + 1);
			result[k + 1] = value;
		}

		return move (result);
	}
#endif //INTERNAL_LEGENDRE

}
