#include "BasisBuilderRule.h"

	using namespace Basis;
	using namespace std;

	////TODO: verify if it is optimizable
	//it follows the alghoritm proposed in "Modellistica numerica per problemi differenziali", A.Quarteroni, 5a ed., p.234
	vector<Polinomial<1>> LegendreBuilder::operator() (size_t degree)
	{
		vector<Polinomial<1> > result;
		Polinomial<1> x(1,1);
		x[0] = 0;
		Polinomial<1> first (0, sqrt(0.5));
////////////////////////////////////////////		Polinomial<1> first (0, 1);
		result.push_back(first);
		if (degree)
		{
			Polinomial<1> second = x * sqrt(1.5);
////////////////////////////////////////////			Polinomial<1> second = x;
			result.push_back(second);
			Polinomial<1> before(second);
			Polinomial<1> beforeBefore(first);
			Polinomial<1> newPol;

			//at each step I calculate di k+1 degree polinomial
			size_t k (1);
			while (k < degree)
			{
//#ifdef MYDEBUG
//				cout << "fattori: " << endl; before.print(); beforeBefore.print();
//				cout << "I fattori hanno grado >>>>> " << before.degree() << ",	" << beforeBefore.degree() << endl;
//#endif //MYDEBUG
				double k_d = static_cast<double>(k);

				//L_(k + 1) computation
				newPol =  x * (2 * k_d + 1);
//#ifdef MYDEBUG
//				cout << "Coefficienti nuovo termine:	";
//				newPol.print();
//#endif //MYDEBUG				

				newPol *= (before / sqrt(k_d + 0.5));
////////////////////////////////////////////////////////				newPol *= before;

//#ifdef MYDEBUG
//				cout << "Coefficienti nuovo termine:	";
//				newPol.print();
//#endif //MYDEBUG
				newPol -= (beforeBefore / sqrt(k_d - 0.5)) * k_d;
////////////////////////////////////////////////////////				newPol -= beforeBefore * k_d;

				newPol /= (k_d + 1);

				//normalization for the orthonormality
				newPol *= sqrt(k_d + 1.5);
//#ifdef MYDEBUG
//				cout << "Coefficienti nuovo termine:	";
//				newPol.print();
//				cout << "Grado del nuovo termine >>>>>> " << newPol.degree() << endl; 
//#endif //MYDEBUG
				result.push_back(newPol);
				beforeBefore = before;
				before = newPol;
				++k;
			}
//#ifdef MYDEBUG
//			cout << "Mi aspetto grado >>>>>> " << degree << endl;
//			cout << "Ho effettivamente >>>>> " << newPol.degree() << endl;
//			cout << "La base ha dimensione = " << result.size() << endl;
//			cout << "Elementi di grado: ";
//			for (auto pol : result)
//				cout << pol.degree() << " ";
//			cout << endl;
//#endif //MYDEBUG 
		}
		return result;
	};

	vector<Polinomial<1>> TrivialBuilder::operator() (size_t degree)
	{	
		vector<Polinomial<1> > result;

		Polinomial<1> first (0, 1);
		result.push_back(first);
		
		if (degree)
		{
			Polinomial<1> x (1,1);
			x[0]=0;
//#ifdef MYDEBUG
//			cout << "coefficienti di first:";
//			for (auto c : first)
//				cout << "	" << c;
//			cout << endl;
//			cout << "first ha grado	"	<< first.degree() << endl;
//			cout << "coefficienti di x:";
//			for (auto c : x)
//				cout << "	" << c;
//			cout << endl;
//			cout << "x ha grado	" 		<< x.degree() << endl;
//#endif //MYDEBUG
			result.push_back(x);
			Polinomial<1>* before = &x;
			Polinomial<1> newPol;
			for (size_t k (2); k <= degree; ++k)
			{	
				newPol =  x * (*before);
//#ifdef MYDEBUG
//				cout << "Grado del nuovo termine >>>>>> " << newPol.degree() << endl; 
//#endif //MYDEBUG				
				result.push_back(newPol);
				before = &newPol;
			}
//#ifdef MYDEBUG
//			cout << "Mi aspetto grado >>>>>> " << degree << endl;
//			cout << "Ho effettivamente >>>>> " << newPol.degree() << endl;
//			cout << "La base ha dimensione = " << result.size() << endl;
//			cout << "Elementi di grado: ";
//			for (auto pol : result)
//				cout << pol.degree() << " ";
//			cout << endl;
//#endif //MYDEBUG 
		}
		return result;
	};

