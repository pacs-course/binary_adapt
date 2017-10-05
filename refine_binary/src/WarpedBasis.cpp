#include "WarpedBasis.h"

//jacobi_polynomial.hpp need the "using namespace std" before the include
//I'm in a header file, I don't want to put "using namespace std" outside the namespace FiniteElements
using namespace std;
#include "jacobi_polynomial.hpp"

namespace FiniteElements
{
	WarpedBasis::WarpedBasis() : AbstractBasis<2>(){};
	WarpedBasis::WarpedBasis(const WarpedBasis& input_basis)
	{
		*this = input_basis;
	};
	WarpedBasis& WarpedBasis::operator= (const WarpedBasis& input_basis)
	{
		if (this != &input_basis)
		{
			AbstractBasis<2>::Copy(input_basis);
		}
		return *this;
	};

	/* This basis doesn't is not the result of a simple tensorization,
		so I don't have a 1d version of it. If I'm in OneDEvaluation
		it means that something has gone wrong, so I raise an exception */
	double WarpedBasis::OneDEvaluation(size_t, double)const
	{
		throw logic_error("Trying to call the meaningless 1d evaluation of the warped basis");
	};

	double WarpedBasis::Evaluate(size_t ind, const Geometry::Point<2>& p)
	{
		static bool called = true;
		if (called)
		{
			cerr << "WARNING: Evaluate method in WarpedBasis is deprecated because of inefficiency" << endl;
			called = false;
		}

		size_t degree = 0;
		size_t length = 0;
		while (length < ind)
			length = ComputeSize(degree++);

		vector<double> full_evaluation = WarpedBasis::EvaluateBasis(degree, p);
		return full_evaluation[ind];
	};

	//Thanks to the jacobi_polynomial working principle I can optimize this method 
	vector<double> WarpedBasis::EvaluateBasis(size_t degree, const Geometry::Point<2>& p)
	{
		size_t length = this->ComputeSize(degree);
		this->UpdateSize(length);

		unique_ptr<double[]> k1_evaluations;

		double x = p[0];
		k1_evaluations = move(unique_ptr<double[]> (j_polynomial(1, degree, 0, 0, &x)));
		double y = p[1];

		unique_ptr<double[]> k1_k2_evaluations;
		vector<double> result;

		for (size_t k1 = 0; k1 <= degree; ++k1)
		{
			size_t max_k2 = degree - k1;
			k1_k2_evaluations = move(unique_ptr<double[]> (j_polynomial(1, max_k2, 2*k1 + 1, 0, &y)));

			for (size_t k2 = 0; k2 <= max_k2; ++k2)
			{
				double k1_val = k1_evaluations[k1];
				double k1_k2_val = pow(1-y, k1) * k1_k2_evaluations[k2];
				double val = k1_val * k1_k2_val;
				result.push_back(val);
			}
		}
//		cerr << "lunghezza attesa: " << length << endl;
//		cerr << "lunghezza risultato: " << result.size() << endl;
//		cerr << "lunghezza teorica: " << static_cast<double>((degree + 1) * (degree + 2)) / 2 << endl;

		return result;
	};

} //namespace FiniteElements
