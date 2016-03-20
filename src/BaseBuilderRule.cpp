#include "BaseBuilderRule.h"

	vector<Polinomial<1>> Legendre::operator() (size_t degree)
	{
		vector<Polinomial<1> > result;

		Polinomial<1> first (0, 1);
		result.push_back(first);

		if (degree)
		{
			Polinomial<1> x (1, 1);
			x[0] = 0;
			result.push_back(x);
			Polinomial<1> before(x);
			Polinomial<1> beforeBefore(first);
			Polinomial<1> newPol;

			size_t k (2);
			while (k <= degree)
			{
#ifdef DEBUG
				cout << "fattori: " << endl; before.print(); beforeBefore.print();
				cout << "I fattori hanno grado >>>>> " << before.degree() << ",	" << beforeBefore.degree() << endl;
#endif //DEBUG
				double k_d = static_cast<double>(k);
				newPol =  x * ((2 * k_d + 1) / (k_d + 1));
#ifdef DEBUG
				cout << "Coefficienti nuovo termine:	";
				newPol.print();
#endif //DEBUG				

				newPol *= before;
#ifdef DEBUG
				cout << "Coefficienti nuovo termine:	";
				newPol.print();
#endif //DEBUG				
				newPol -= (beforeBefore * (k_d / (k_d + 1)));
#ifdef DEBUG
				cout << "Coefficienti nuovo termine:	";
				newPol.print();
				cout << "Grado del nuovo termine >>>>>> " << newPol.degree() << endl; 
#endif //DEBUG				
				result.push_back(newPol);
				beforeBefore = before;
				before = newPol;
				++k;
			}
#ifdef DEBUG
			cout << "Mi aspetto grado >>>>>> " << degree << endl;
			cout << "Ho effettivamente >>>>> " << newPol.degree() << endl;
			cout << "La base ha dimensione = " << result.size() << endl;
			cout << "Elementi di grado: ";
			for (auto pol : result)
				cout << pol.degree() << " ";
			cout << endl;
#endif //DEBUG 
		}
		return result;
	};

	vector<Polinomial<1>> Trivial::operator() (size_t degree)
	{	
		vector<Polinomial<1> > result;

		Polinomial<1> first (0, 1);
		result.push_back(first);
		
		if (degree)
		{
			Polinomial<1> x (1,1);
			x[0]=0;
#ifdef DEBUG
			cout << "coefficienti di first:";
			for (auto c : first)
				cout << "	" << c;
			cout << endl;
			cout << "first ha grado	"	<< first.degree() << endl;
			cout << "coefficienti di x:";
			for (auto c : x)
				cout << "	" << c;
			cout << endl;
			cout << "x ha grado	" 		<< x.degree() << endl;
#endif //DEBUG
			result.push_back(x);
			Polinomial<1>* before = &x;
			Polinomial<1> newPol;
			for (size_t k (2); k <= degree; ++k)
			{	
				newPol =  x * (*before);
#ifdef DEBUG
				cout << "Grado del nuovo termine >>>>>> " << newPol.degree() << endl; 
#endif //DEBUG				
				result.push_back(newPol);
				before = &newPol;
			}
#ifdef DEBUG
			cout << "Mi aspetto grado >>>>>> " << degree << endl;
			cout << "Ho effettivamente >>>>> " << newPol.degree() << endl;
			cout << "La base ha dimensione = " << result.size() << endl;
			cout << "Elementi di grado: ";
			for (auto pol : result)
				cout << pol.degree() << " ";
			cout << endl;
#endif //DEBUG 
		}
		return result;
	};

