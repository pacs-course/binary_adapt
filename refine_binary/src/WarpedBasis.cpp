#include "WarpedBasis.h"
#include "BinaryTreeHelper.h"

//jacobi_polynomial.hpp need the "using namespace std" before the include
//I'm in a header file, I don't want to put "using namespace std" outside the namespace FiniteElements
using namespace std;
#include "jacobi_polynomial.hpp"

namespace FiniteElements
{
	WarpedBasis::WarpedBasis() : TensorialBasis<2>() {};
	WarpedBasis::~WarpedBasis() {};

	WarpedBasis::WarpedBasis (const WarpedBasis& input_basis)
	{
		*this = input_basis;
	};

	WarpedBasis& WarpedBasis::operator= (const WarpedBasis& input_basis)
	{
		if (this != &input_basis)
		{
			TensorialBasis<2>::Copy (input_basis);
		}
		return *this;
	};

	double WarpedBasis::OneDEvaluation (size_t, double) const
	{
		throw logic_error(
			"Trying to call the meaningless 1d evaluation of the warped basis");
	};

	double WarpedBasis::Evaluate (size_t ind, const Geometry::Point<2>& p)
	{
#ifdef VERBOSE
		static bool called = true;
		if (called)
		{
			cerr << "Warning: Evaluate method in WarpedBasis"
				 << "is deprecated because of inefficiency"
				 << endl;
			called = false;
		}
#endif //VERBOSE

		size_t degree = 0;
		size_t length = 0;
		while (length < ind)
			length = ComputeSize (degree++);

		Geometry::Vector full_evaluation = WarpedBasis::EvaluateBasis (degree, p);
		return full_evaluation[ind];
	};

	/* Thanks to the jacobi_polynomial working principle I can optimize this method */
	Geometry::Vector WarpedBasis::EvaluateBasis (size_t degree,
												 const Geometry::Point<2>& p)
	{
		size_t length = this->ComputeSize (degree);
		this->UpdateSize (length);

		unique_ptr<double[]> k1_evaluations;

		double x = p[0];
		k1_evaluations = move (unique_ptr<double[]>(
								j_polynomial (1, degree, 0, 0, &x)));

		vector<unique_ptr<double[]>> k1_k2_evaluations;
		double y = p[1];

		for (size_t k1 = 0; k1 <= degree; ++k1)
		{
			size_t max_k2 = degree - k1;
			auto eval = move (unique_ptr<double[]>(
								j_polynomial (1, max_k2, 2 * k1 + 1, 0, &y)));

			k1_k2_evaluations.push_back (move (eval));
		}

		Geometry::Vector result (length);
		for ( size_t i (0); i < length; ++i)
		{
			size_t k1 = this->_tensorial_indexes[i][0];
			size_t k2 = this->_tensorial_indexes[i][1];

			//optimized computation of (1-y)^k1
			double basis = 1 - y;
			double power = Helpers::IntPower (basis, k1);

			double tot = k1_evaluations[k1] * power * k1_k2_evaluations[k1][k2];
			result[i] = tot;
		};

		return result;
	};

} //namespace FiniteElements
