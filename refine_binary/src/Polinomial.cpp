#include "Polinomial.h"

using namespace std;

namespace FiniteElements
{	
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
//		Print();
//		cout << "per il double " << scalar << endl;

//		cout << "Il risultato e' inizializzato a" << endl;
//		result.Print();
//		size_t i = 0;
#endif //MYDEBUG
		
		for (auto& c : result)
			c *= scalar;

#ifdef MYDEBUG
//		cout << "Il risultato prima del return e': " << endl;
//		result.Print();
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

		size_t this_degree = Degree();
		size_t p_degree = p.Degree();

		Polinomial<1> result (this_degree + p_degree);

//#ifdef MYDEBUG
//		cout << "Sto moltiplicando" << endl;
//		Print();
//		cout << "con" << endl;
//		p.Print();		
//#endif //MYDEBUG

		for (size_t i = 0; i <= this_degree; ++i)
			for (size_t j = 0; j <= p_degree; ++j)
				result[i + j] += this->_coeff[i] * p[j];

//#ifdef MYDEBUG
//		cout << "Coefficienti risultato:		";
//		result.Print();
//		cout << "Risultato ha grado >>> " << result.degree() << endl; 
//#endif //MYDEBUG
		return result;
	};


	Polinomial<1> Polinomial<1>::operator + (const Polinomial<1>& p) const
	{
//#ifdef MYDEBUG
//		cout << "Sto sommando " << endl;
//		Print();
//		cout << "a" << endl;
//		p.Print();
//#endif //MYDEBUG
		size_t p_degree = p.Degree();
		size_t this_degree = Degree();
		size_t result_degree = max(p_degree, this_degree);

		const Polinomial<1>& max_poli	(
													p_degree == result_degree ? p : *this 
												);

		const Polinomial<1>& min_poli	(
													p_degree == result_degree ? *this : p 
												);											

		Polinomial<1> result(result_degree);
//#ifdef MYDEBUG
//		cout << "Risultato inizializzato a " << endl;
//		result.Print();
//#endif //MYDEBUG

		size_t ind(0);
		
		while (ind <= min_poli.Degree())
		{
			result[ind] = min_poli[ind] + max_poli[ind];
//#ifdef MYDEBUG
//			cout << min_poli[ind] << " + " << max_poli[ind] << " = " << result[ind] << endl;
//#endif //MYDEBUG
			++ind;
		}

		while (ind <= result_degree)
		{
			result[ind] = max_poli[ind];
			++ind;
		}
//#ifdef MYDEBUG
//		cout << "Risultato somma " << endl;
//		result.Print();
//#endif //MYDEBUG
		return result;	
	};

	Polinomial<1> Polinomial<1>::operator - (const Polinomial<1>& p) const
	{
//#ifdef MYDEBUG
//		cout << "Sto sottraendo " << endl;
//		p.Print();
//		cout << "a" << endl;
//		Print();
//#endif //MYDEBUG

		Polinomial<1> temp(p);
		for (auto& coeff : temp)
			coeff = 0.0 - coeff;
//#ifdef MYDEBUG
//		cout << "Risultato inversione segni: " << endl;
//		temp.Print();
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
		auto one_less_end = _coeff.rend() - 1;
		for (; c_i != one_less_end; ++c_i)
		{
			tot += *c_i;
			tot *= x;
		}
		tot += *c_i;

		return tot;
	};

	size_t Polinomial<1>::Degree() const
	{
		return _coeff.size() - 1;
	};

	void Polinomial<1>::Degree(size_t val)
	{
		_coeff.resize(val + 1);
	};

	bool Polinomial<1>::Null() const
	{
		return _coeff.empty();
	};

	void Polinomial<1>::Print(bool verbose, string s, bool new_line) const
	{
		if (verbose)
		{
			bool first(true);
			size_t exponent(0);
			for (auto& c : this->_coeff)
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
