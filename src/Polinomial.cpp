#include "Polinomial.h"

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
//#ifdef DEBUG
//		cout << "Sono in operator = " << endl;
//#endif //DEBUG
		if (&p != this)
		{
//			_coeff.resize(p._coeff.size());
//			copy (p._coeff.begin(), p._coeff.end(), _coeff.begin());
			_coeff = p._coeff;
		}
//#ifdef DEBUG
//		cout << "Esco da operator = " << endl;
//#endif //DEBUG
		return *this;
	};

	Polinomial<1> Polinomial<1>::operator * (const double& scalar) const
	{
		Polinomial<1> result(*this);
#ifdef DEBUG
//		cout << "moltiplico il polinomio di coefficienti" << endl;
//		print();
//		cout << "per il double " << scalar << endl;

//		cout << "Il risultato e' inizializzato a" << endl;
//		result.print();
//		size_t i = 0;
#endif //DEBUG
		
		for (auto& c : result)
			c *= scalar;

#ifdef DEBUG
//		cout << "Il risultato prima del return e': " << endl;
//		result.print();
#endif //DEBUG
		return result;
	}

	Polinomial<1> Polinomial<1>::operator * (const Polinomial<1>& p) const
	{

		size_t thisDegree = degree();
		size_t pDegree = p.degree();

		Polinomial<1> result (thisDegree + pDegree);

#ifdef DEBUG
		cout << "Sto moltiplicando" << endl;
		print();
		cout << "con" << endl;
		p.print();		
#endif //DEBUG

		for (size_t i = 0; i <= thisDegree; ++i)
			for (size_t j = 0; j <= pDegree; ++j)
				result[i + j] += _coeff[i] * p[j];

#ifdef DEBUG
		cout << "Coefficienti risultato:		";
		result.print();
		cout << "Risultato ha grado >>> " << result.degree() << endl; 
#endif //DEBUG
		return result;
	};

	Polinomial<1> Polinomial<1>::operator + (const Polinomial<1>& p) const
	{
#ifdef DEBUG
		cout << "Sto sommando " << endl;
		print();
		cout << "a" << endl;
		p.print();
#endif //DEBUG
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
#ifdef DEBUG
		cout << "Risultato inizializzato a " << endl;
		result.print();
#endif //DEBUG

		size_t ind(0);
		
		while (ind <= minPoli.degree())
		{
			result[ind] = minPoli[ind] + maxPoli[ind];
#ifdef DEBUG
			cout << minPoli[ind] << " + " << maxPoli[ind] << " = " << result[ind] << endl;
#endif //DEBUG
			++ind;
		}

		while (ind <= resultDegree)
		{
			result[ind] = maxPoli[ind];
			++ind;
		}
#ifdef DEBUG
		cout << "Risultato somma " << endl;
		result.print();
#endif //DEBUG
		return result;	
	};

	Polinomial<1> Polinomial<1>::operator - (const Polinomial<1>& p) const
	{
#ifdef DEBUG
		cout << "Sto sottraendo " << endl;
		p.print();
		cout << "a" << endl;
		print();
		
#endif //DEBUG
		Polinomial<1> temp(p);
		for (auto& coeff : temp)
			coeff = 0.0 - coeff;
#ifdef DEBUG
		cout << "Risultato inversione segni: " << endl;
		temp.print();
#endif //DEBUG
		return ((*this) + temp);
	};

	Polinomial<1>& Polinomial<1>::operator -= (const Polinomial<1>& p)
	{
//#ifdef DEBUG
//		cout << "Entro in operator -= " << endl;
//#endif //DEBUG
		(*this) = (*this) - p;
//#ifdef DEBUG
//		cout << "Pronto per uscire da operator -= " << endl;
//#endif //DEBUG
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

	void Polinomial<1>::print() const
	{
		for (auto& c : _coeff)
			cout << c << "	";
		cout << endl;
	};
