#include "Poly.h"

	using namespace std;

	Polinomial<1>::Polinomial (size_t degree) : _coeff(degree, 0){};
	Polinomial<1>::Polinomial (const vector<double>& v): _coeff(v){};
	Polinomial<1> Polinomial<1>::operator * (Polinomial<1>& p) const
	{
		size_t thisDegree = degree();
		size_t pDegree = p.degree();

		Polinomial<1> result (thisDegree + pDegree);

		for (size_t i = 0; i < thisDegree; ++i)
			for (size_t j = 0; j < pDegree; ++j)
				result[i + j] += _coeff[i]*p[j];

		return result;
	};

	double& Polinomial<1>::operator [] (size_t ind)
	{
		return _coeff[ind];
	}

//smart evaluation to minimize number of products
	double Polinomial<1>::operator() (Point<1> const& x) const
	{
		double tot = 0;
/*
begin() = rend()-1 : last one must not be multiplicated by x
*/
		auto c_i = _coeff.rbegin();
		auto oneLessEnd = _coeff.rend() - 1;
		for (; c_i != oneLessEnd; ++c_i)
		{
			tot += *c_i;
			tot *= x[0];
		}
		tot += *c_i;
		return tot;
	};

	size_t Polinomial<1>::degree() const
	{
		return _coeff.size();
	};

	void Polinomial<1>::degree(size_t val)
	{
		_coeff.resize(val);
	}



















//	Poly1D::Poly1D ()											: _coefficients () 						{};
//	Poly1D::Poly1D (vector<double>& coeff)				: _coefficients (coeff) 				{};
//	Poly1D::Poly1D (size_t& degree, double& coeff)	: _coefficients (degree + 1, coeff)	{};

//	double Poly1D::operator() (Point<1>& x)
//	{
//		return (*this)(x[0]);
//	};

//	double Poly1D::operator() (double& x)
//	{
//		size_t p(0);
//		return accumulate(
//								_coefficients.begin(),
//								_coefficients.end(),
//								0.0,
//								[&&] (double tot, double coeff) { return tot += coeff*pow(x, p++); }
//								);
//	};

//	size_t Poly1D::degree ()
//	{
//		return _coefficients.size() - 1;
//	};

//	void Poly1D::degree (size_t& p, double& val)
//	{
//		_coefficients.resize(p + 1, val);
//	}


