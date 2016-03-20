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

//template <typename T>
//	typename enable_if<is_array<T>::value, void>::type
//	prova(T input)
//	{
//			cout <<"ECCOMI >>>>> "<< *input << endl;
//	};

	template <size_t N = 1, typename enable_if< !(N < 1), size_t>::type = 0>
		void foo (string ss)
		{
			cout << ss << endl;
		};

	template <size_t N = 1, typename enable_if< (N < 1), size_t>::type = 0>
		void foo (string ss)
		{
			cout << ss << endl;
		};


int main()
{
//	Tensor<Point<2>, 2> test(vector<Point<1>>(3,1),vector<Point<1>>(2,2));
//	cout << test.size() << endl;

	Point<1> p_test({1});
	Point<1> p_copy(p_test);
	Point<1> p1 = p_copy;
	array<double, 2> init = {2,3};
	Point<2> p2(init);
	Point<3> p3 = p1.tensor(p2);
	p1.print();
	p2.print();
	p3.print();

	cout << "Testo l'enable_if" << endl;
	string s("Funziona!!!!!!");
	foo<1>(s);
	string ss("Funziona ancora!!!!!");
	foo<2>(ss);
	foo (ss);
	size_t degree(2);

	Polinomial<1> pol1(3,2);
	Polinomial<1> pol2(4,4);
	Polinomial<2> pol3 = pol1.tensor(pol2);
	pol3.print();

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

