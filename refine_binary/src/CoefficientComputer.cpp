#include "CoefficientComputer.h"

using namespace std;

namespace FiniteElements
{
	void CoefficientComputer<2, WarpedType>::ComputeCoefficients(	const AbstractFElement<2, WarpedType>& f_el,
																						BinaryTree::FunctionPtr<2> interp_func,
																						vector<double>* coeff_location )
	{
		//I remember how many coefficients have been already computed
		size_t cursor = coeff_location->size();
		//New number of coefficients
		size_t s = f_el.BasisSize();

		if (cursor == s)
			//Coefficients already computed
			return;

		//memory allocation
		coeff_location->resize(s);

		//I don't recompute already stored coefficients
		for(; cursor < s; ++cursor)
		{
			(*coeff_location)[cursor] = f_el.L2Prod ( *interp_func,
																	[&f_el, &cursor]
																	(const Geometry::Point<2>& p)
																	{ return f_el.EvaluateBasisFunction(cursor, p);}
																 );
			double norm_2 = f_el.L2Prod	(
													[&f_el, &cursor]
													(const Geometry::Point<2>& p)
													{ return f_el.EvaluateBasisFunction(cursor, p);},
													[&f_el, &cursor]
													(const Geometry::Point<2>& p)
													{ return f_el.EvaluateBasisFunction(cursor, p);}
												);
			(*coeff_location)[cursor] /= norm_2;
		}
	};

} //namespace FiniteElements
