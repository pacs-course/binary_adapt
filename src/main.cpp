#include <iostream>
#include <vector>
#include <array>
#include <string>

#include "Basis.h"
#include "BaseBuilderRule.h"
//#include "Tensorize.h"
//#include "CubicTensor.h"
#include "Point.h" 
#include "Polinomial.h"

using namespace std;
using namespace Geometry;


int main()
{

	Point<1> p_test({1});
	Point<1> p_copy(p_test);
	Point<1> p1 = p_copy;
	array<double, 2> init = {2,3};
	Point<2> p2(init);
	Point<3> p3 = p1.tensor(p2);
	double scalar(2);
	Point<3> p4 = p3 * scalar;
	cout << "p1:" << endl;
	p1.print();
	cout << "p2:" << endl;
	p2.print();
	cout << "p3 = p1 tensor p2" << endl;
	p3.print();
	cout << "p4 = 2 * p3" << endl;
	p4.print();
	Point<3> p5 = p4 - p3;
	cout << "p5 = p4 - p3" << endl;
	p5.print();
	cout << " |p5| = " << p5.abs() << endl;
	cout << " |p5 - p4| = " << p5.distance(p4) << endl;

	Polinomial<1> pol1(1,2);
	Polinomial<1> pol2(2,4);
	cout << "Polinomi di una variabile: " << endl;
	cout << "poli1" << endl;
	pol1.print();
	cout << "poli2" << endl;
	pol2.print();
	
	cout << endl << "Polinomio di 2 variabili poli3 = poli1 tensor poli2" << endl;
	Polinomial<2> pol3 = pol1.tensor(pol2);
	pol3.print();

	Polinomial<3> pol4 = pol3.tensor(pol2);
	Polinomial<3> pol5 = pol1.tensor(pol3);

	cout << endl << "Polinomi di 3 variabili:" << endl;
	cout << "1D tensor 2D: poli3 tensor poli2" << endl;
	pol4.print();
 	cout << "2D tensor 1D: poli1 tensor poli3" << endl;
	pol5.print();

	Polinomial<4> pol6 = pol3.tensor(pol3);
	cout << endl << "Polinomio di 4 variabili poli4 = poli3 tensor poli3" << endl;
	pol6.print();

	cout << endl << "VALUTAZIONE DEI POLINOMI" << endl;
	cout << endl << "valuto il polinomio" << endl;
	pol4.print();
	cout << "nel punto " << endl;
	p5.print();
	cout << "Risultato: " << pol4(p5) << endl;

	Polinomial<1> one(5,0);
	one[0] = 1;
	Polinomial<1> x(1,0);
	x[1] = 1;
	Polinomial<3> z = one.tensor(one).tensor(x);
	cout << endl << "valuto il polinomio p(x,y,z) = z" << endl;
	z.print();
	cout << "nel punto " << endl;
	p5.print();
	cout << "Risultato: " << z(p5) << endl;
	
	size_t degree(2);

	cout << "Provo a costruire la base trivial:" << endl;
	PoliBaseBuilderRule_ptr trivial = unique_ptr<Trivial>(new Trivial);
	vector<Polinomial<1>> pol = (*trivial)(degree);

	PolinomialBasis<1> not_orthonormal_second (move(trivial), degree); 
	cout << "Base 1D : " << endl;
	not_orthonormal_second.print();

	cout << "Base 3D : " << endl;
	PolinomialBasis<3> not_orthonormal_first (pol);
	not_orthonormal_first.print();

	cout << "Provo a costruire la base di legendre:" << endl;
	PoliBaseBuilderRule_ptr legendre = unique_ptr<Legendre>(new Legendre);
	vector<Polinomial<1>> orthoPol = (*legendre)(degree);
	PolinomialBasis<1> orthonormal_first (orthoPol);
	cout << "Base 1D : " << endl;	
	orthonormal_first.print();
	PolinomialBasis<2> orthonormal_second (move(legendre), degree);
	cout << "Base 2D : " << endl;
	orthonormal_second.print();

	return 0;
};

