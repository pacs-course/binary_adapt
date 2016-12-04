#include "Polinomial.h"

namespace FiniteElements
{
	using namespace std;
	
	Polinomial<1>::Polinomial ()					 					: _coeff()	 					{};
	Polinomial<1>::Polinomial (size_t degree) 					: _coeff(degree + 1, 0)		{};
	Polinomial<1>::Polinomial (const vector<double>& v) 		: _coeff(v) 					{};
	Polinomial<1>::Polinomial (size_t degree, double coeff)	: _coeff(degree + 1, coeff){};

	Polinomial<1>::Polinomial (const Polinomial<1>& p)
	{
		*this = p;
	};

	Polinomial<1>& Polinomial<1>::operator = (const Polinomial<1>& p)
	{
//#ifdef MYDEBUG
//		cout << "Sono in operator = " << endl;
//#endif //MYDEBUG
		if (&p != this)
		{
//			_coeff.resize(p._coeff.size());
//			copy (p._coeff.begin(), p._coeff.end(), _coeff.begin());
			_coeff = p._coeff;
		}
//#ifdef MYDEBUG
//		cout << "Esco da operator = " << endl;
//#endif //MYDEBUG
		return *this;
	};

	Polinomial<1> Polinomial<1>::operator * (const double& scalar) const
	{
		Polinomial<1> result(*this);
#ifdef MYDEBUG
//		cout << "moltiplico il polinomio di coefficienti" << endl;
//		print();
//		cout << "per il double " << scalar << endl;

//		cout << "Il risultato e' inizializzato a" << endl;
//		result.print();
//		size_t i = 0;
#endif //MYDEBUG
		
		for (auto& c : result)
			c *= scalar;

#ifdef MYDEBUG
//		cout << "Il risultato prima del return e': " << endl;
//		result.print();
#endif //MYDEBUG
		return result;
	}

	Polinomial<1> Polinomial<1>::operator / (const double& scalar) const
	{
		Polinomial<1> result(*this);
		for (auto& c : result)
			c /= scalar;

		return result;
	};

	Polinomial<1> Polinomial<1>::operator * (const Polinomial<1>& p) const
	{

		size_t thisDegree = degree();
		size_t pDegree = p.degree();

		Polinomial<1> result (thisDegree + pDegree);

//#ifdef MYDEBUG
//		cout << "Sto moltiplicando" << endl;
//		print();
//		cout << "con" << endl;
//		p.print();		
//#endif //MYDEBUG

		for (size_t i = 0; i <= thisDegree; ++i)
			for (size_t j = 0; j <= pDegree; ++j)
				result[i + j] += _coeff[i] * p[j];

//#ifdef MYDEBUG
//		cout << "Coefficienti risultato:		";
//		result.print();
//		cout << "Risultato ha grado >>> " << result.degree() << endl; 
//#endif //MYDEBUG
		return result;
	};


	Polinomial<1> Polinomial<1>::operator + (const Polinomial<1>& p) const
	{
//#ifdef MYDEBUG
//		cout << "Sto sommando " << endl;
//		print();
//		cout << "a" << endl;
//		p.print();
//#endif //MYDEBUG
		size_t pDegree = p.degree();
		size_t thisDegree = degree();
		size_t resultDegree = max(pDegree, thisDegree);

		const Polinomial<1>& maxPoli	(
													pDegree == resultDegree ? p : *this 
												);

		const Polinomial<1>& minPoli	(
													pDegree == resultDegree ? *this : p 
												);											

		Polinomial<1> result(resultDegree);
//#ifdef MYDEBUG
//		cout << "Risultato inizializzato a " << endl;
//		result.print();
//#endif //MYDEBUG

		size_t ind(0);
		
		while (ind <= minPoli.degree())
		{
			result[ind] = minPoli[ind] + maxPoli[ind];
//#ifdef MYDEBUG
//			cout << minPoli[ind] << " + " << maxPoli[ind] << " = " << result[ind] << endl;
//#endif //MYDEBUG
			++ind;
		}

		while (ind <= resultDegree)
		{
			result[ind] = maxPoli[ind];
			++ind;
		}
//#ifdef MYDEBUG
//		cout << "Risultato somma " << endl;
//		result.print();
//#endif //MYDEBUG
		return result;	
	};

	Polinomial<1> Polinomial<1>::operator - (const Polinomial<1>& p) const
	{
//#ifdef MYDEBUG
//		cout << "Sto sottraendo " << endl;
//		p.print();
//		cout << "a" << endl;
//		print();
//#endif //MYDEBUG

		Polinomial<1> temp(p);
		for (auto& coeff : temp)
			coeff = 0.0 - coeff;
//#ifdef MYDEBUG
//		cout << "Risultato inversione segni: " << endl;
//		temp.print();
//#endif //MYDEBUG
		return ((*this) + temp);
	};

	Polinomial<1>& Polinomial<1>::operator -= (const Polinomial<1>& p)
	{
//#ifdef MYDEBUG
//		cout << "Entro in operator -= " << endl;
//#endif //MYDEBUG
		(*this) = (*this) - p;
//#ifdef MYDEBUG
//		cout << "Pronto per uscire da operator -= " << endl;
//#endif //MYDEBUG
		return (*this);
	};
	Polinomial<1>& Polinomial<1>::operator += (const Polinomial<1>& p)
	{
		(*this) = (*this) + p;
		return (*this);
	};
	Polinomial<1>& Polinomial<1>::operator *= (const Polinomial<1>& p)
	{
		(*this) = (*this) * p;
		return (*this);
	};
	Polinomial<1>& Polinomial<1>::operator *= (const double& a)
	{
		(*this) = (*this) * a;
		return (*this);
	};

	Polinomial<1>& Polinomial<1>::operator /= (const double& a)
	{
		(*this) = (*this) / a;
		return (*this);
	}


	double& Polinomial<1>::operator[] (size_t ind)
	{
		return _coeff[ind];
	};

	const double& Polinomial<1>::operator[] (size_t ind) const
	{
		return _coeff[ind];
	};

	double Polinomial<1>::get (size_t ind) const
	{
		return _coeff[ind];
	};

	vector<double>::iterator Polinomial<1>::begin()
	{
		return _coeff.begin();
	};
	vector<double>::iterator Polinomial<1>::end()
	{
		return _coeff.end();
	};
	vector<double>::const_iterator Polinomial<1>::begin() const
	{
		return _coeff.begin();
	};
	vector<double>::const_iterator Polinomial<1>::end() const
	{
		return _coeff.end();
	};


//smart evaluation to minimize number of products
	double Polinomial<1>::operator() (Point<1> const& x) const
	{
		return (*this)(x[0]);
	};

	double Polinomial<1>::operator() (double const& x) const
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
			tot *= x;
		}
		tot += *c_i;
		return tot;

	};
	size_t Polinomial<1>::degree() const
	{
		return _coeff.size() - 1;
	};

	void Polinomial<1>::degree(size_t val)
	{
		_coeff.resize(val + 1);
	};

	bool Polinomial<1>::null() const
	{
		return _coeff.empty();
	};

	void Polinomial<1>::print(bool verbose, string s, bool new_line) const
	{
		if (verbose)
		{
			bool first(true);
			size_t exponent(0);
			for (auto& c : _coeff)
			{
				if (c)
				{
					if (!exponent)
						cout << c;
					else
					{
						if(!first)
						{
							cout << " ";
							c > 0 ? cout << " + " : cout << " - ";
						}
						if (abs(c) != 1) cout << abs(c); 
						cout << s;
						if (exponent > 1) cout << "^" << exponent;
					}
					first = false;
				}
				++exponent;
			}
			if (new_line) cout << endl;
		}
		else
		{
			for (auto& c : _coeff)
				cout << c << "	";
			cout << endl;
		}
	};

} //FiniteElements
