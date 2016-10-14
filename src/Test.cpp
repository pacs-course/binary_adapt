#include <iostream>
#include <vector>
#include <array>
#include <string>

#include "Test.h"

#include "libmesh/getpot.h" //GetPot
#include "libmesh/mesh_generation.h" //MeshTools
#include "libmesh/mesh.h"
#include "libmesh/libmesh.h"

using namespace std;
using namespace Geometry;
using namespace BinaryTree;
using namespace Basis;

//Point test
void test1()
{
//	Point<1> p_test({1});
//	Point<1> p_copy(p_test);
//	Point<1> p1 = p_copy;
//	array<double, 2> init = {2,3};
//	Point<2> p2(init);
//	Point<3> p3 = p1.tensor(p2);
//	double scalar(2);
//	Point<3> p4 = p3 * scalar;
//	cout << "p1:" << endl;
//	p1.print();
//	cout << "p2:" << endl;
//	p2.print();
//	cout << "p3 = p1 tensor p2" << endl;
//	p3.print();
//	cout << "p4 = 2 * p3" << endl;
//	p4.print();
//	Point<3> p5 = p4 - p3;
//	cout << "p5 = p4 - p3" << endl;
//	p5.print();
//	cout << " |p5| = " << p5.abs() << endl;
//	cout << " |p5 - p4| = " << p5.distance(p4) << endl;
};

/*
Polinomial test
*/
void test2()
{
//	Point<1> p_test({1});
//	Point<1> p_copy(p_test);
//	Point<1> p1 = p_copy;
//	array<double, 2> init = {2,3};
//	Point<2> p2(init);
//	Point<3> p3 = p1.tensor(p2);
//	double scalar(2);
//	Point<3> p4 = p3 * scalar;
//	Point<3> p5 = p4 - p3;


//	Polinomial<1> pol1(1,-2);
//	Polinomial<1> pol2(2,4);
//	cout << "Polinomi di una variabile: " << endl;
//	cout << "poli1" << endl;
//	pol1.print(false);
//	pol1.print(true,"x");
//	cout << "poli2" << endl;
//	pol2.print(false);
//	pol2.print();

//	cout << endl << "Polinomio di 2 variabili poli3 = poli1 tensor poli2" << endl;
//	Polinomial<2> pol3 = pol1.tensor(pol2);
//	pol3.print();

//	Polinomial<3> pol4 = pol3.tensor(pol2);
//	Polinomial<3> pol5 = pol1.tensor(pol3);

//	cout << endl << "Polinomi di 3 variabili:" << endl;
//	cout << "1D tensor 2D: poli3 tensor poli2" << endl;
//	pol4.print();
// 	cout << "2D tensor 1D: poli1 tensor poli3" << endl;
//	pol5.print();

//	Polinomial<4> pol6 = pol3.tensor(pol3);
//	cout << endl << "Polinomio di 4 variabili poli4 = poli3 tensor poli3" << endl;
//	pol6.print();

//	cout << endl << "VALUTAZIONE DEI POLINOMI" << endl;
//	cout << endl << "valuto il polinomio" << endl;
//	pol4.print();
//	cout << "nel punto " << endl;
//	p5.print();
//	cout << "Risultato: " << pol4(p5) << endl;

//	Polinomial<1> one(5,0);
//	one[0] = 1;
//	Polinomial<1> x(1,0);
//	x[1] = 1;
//	Polinomial<3> z = one.tensor(one).tensor(x);
//	cout << endl << "valuto il polinomio p(x,y,z) = z" << endl;
//	z.print();
//	cout << "nel punto " << endl;
//	p5.print();
//	cout << "Risultato: " << z(p5) << endl;
};

/*
Basis test
*/
void test3()
{
//	size_t degree(2);
//#ifndef SINGLETON_ENABLED

//	cout << "Provo a costruire la base trivial:" << endl;
//	PoliBaseBuilderRule_ptr trivial = unique_ptr<TrivialBuilder>(new TrivialBuilder);
//	vector<Polinomial<1>> pol = (*trivial)(degree);
//	//TODO: confrontare polinomi
//	PolinomialBasis<1, TRIVIAL> not_orthonormal_first (degree);
//	cout << "Base Trivial 1D : grado " << not_orthonormal_first.degree() << "; dimensione " << not_orthonormal_first.size() << endl;
//	not_orthonormal_first.print();

//	PolinomialBasis<3, TRIVIAL> not_orthonormal_third (degree);
//	cout << "Base Trivial 3D : grado " << not_orthonormal_third.degree() << "; dimensione " << not_orthonormal_third.size() << endl;
//	not_orthonormal_third.print();

//	cout << "Provo a costruire la base di legendre:" << endl;
//	PoliBaseBuilderRule_ptr legendre = unique_ptr<LegendreBuilder>(new LegendreBuilder);
//	vector<Polinomial<1>> orthoPol = (*legendre)(degree);
//	//TODO: confrontare polinomi
//	PolinomialBasis<1, LEGENDRE> orthonormal_first (degree);
//	cout << "Base Legendre 1D : grado " << orthonormal_first.degree() << "; dimensione " << orthonormal_first.size() << endl;
//	orthonormal_first.print();
//	PolinomialBasis<2, LEGENDRE> orthonormal_second (degree);
//	cout << "Base Legendre 2D : grado " << orthonormal_second.degree() << "; dimensione " << orthonormal_second.size() << endl;
//	orthonormal_second.print();

//#endif //SINGLETON_ENABLED
};


/*
BinaryElements construction
*/
void test4(int argc, char ** argv)
{
//	libMesh::LibMeshInit init (argc, argv);
////	GetPot command_line (argc, argv); //ECCOLO!!!

//	int n = 4;
////	if (command_line.search(1, "-n"))
////		n = command_line.next(n);

////	// Brief message to the user regarding the program name
////	// and command line arguments.
//	libMesh::out << "Running " << argv[0];

//	libMesh::Mesh mesh(init.comm());

//	// Build a 1D mesh with n elements from x=0 to x=1, using
//	libMesh::MeshTools::Generation::build_line(mesh, n, 0., 1., LibmeshIntervalType);

//	shared_ptr<function<double(Point<1>)> > f_ptr = make_shared<function<double(Point<1>)>>([](Point<1> x){return x[0]*x[0];});
//#ifdef MYDEBUG
//	Point<1> x({2});
//	cout << "f(2) = " << (*f_ptr)(x) << endl;
//#endif //MYDEBUG

//	Interval I(f_ptr);
//	libMesh::Node n1(1);
//	libMesh::Node n2(2);
//	I.set_node(0) = &n1;
//	I.set_node(1) = &n2;

//	I.init();
};
