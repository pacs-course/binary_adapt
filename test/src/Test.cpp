#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <utility> //std::move

#include "Test.h"
#include "Point.h"
#include "Quadrature.h"
#include "Polinomial.h"
#include "StdFElement.h"

#include "LibMeshRefiner.h"
#include "LibMeshBinaryElements.h"

#include "MyFunctors.h"

#include "libmesh/libmesh.h"
#include "libmesh/mesh_generation.h" //MeshTools
#include "libmesh/getpot.h" //Getpot

using namespace std;
using namespace Geometry;
using namespace BinaryTree;
using namespace FiniteElements;
using namespace LibmeshBinary;

namespace myhelpers
{
	void dim_helper (libMesh::Elem* el)
	{
		cout << "Che dimensione ha l'elemento di input?" << endl;
		cout << el->dim() << endl;
	};

	libMesh::Mesh build_line_helper (int argc, char ** argv, int n, double a, double b)
	{
		libMesh::LibMeshInit init (argc, argv);
		libMesh::Mesh mesh(init.comm());

		libMesh::MeshTools::Generation::build_line(mesh, n, a, b, libMesh::EDGE2);
		return mesh;
	};

} //namespace myhelpers
void test1(int argc, char ** argv)
{
	//to silence warning by the compiler 
	(void)argc;
	(void)argv;

	cout << endl << "Starting Point test" << endl;

	Point<1> p_test({1});
	Point<1> p_copy(p_test);
	Point<1> p1 = p_copy;
	array<double, 2> init = {2,3};
	Point<2> p2(init);
	Point<3> p3 = p1.Tensor(p2);
	double scalar(2);
	Point<3> p4 = p3 * scalar;
	cout << "p1:" << endl;
	p1.Print();
	cout << "p2:" << endl;
	p2.Print();
	cout << "p3 = p1 tensor p2" << endl;
	p3.Print();
	cout << "p4 = 2 * p3" << endl;
	p4.Print();
	Point<3> p5 = p4 - p3;
	cout << "p5 = p4 - p3" << endl;
	p5.Print();
	cout << " |p5| = " << p5.abs() << endl;
	cout << " |p5 - p4| = " << p5.distance(p4) << endl;

	cout << "Point test ended" << endl;

};

void test2(int argc, char ** argv)
{
	//to silence warning by the compiler 
	(void)argc;
	(void)argv;

	cout << endl << "Starting Polinomial test" << endl;

	Point<1> p_test({1});
	Point<1> p_copy(p_test);
	Point<1> p1 = p_copy;
	array<double, 2> init = {2,3};
	Point<2> p2(init);
	Point<3> p3 = p1.Tensor(p2);
	double scalar(2);
	Point<3> p4 = p3 * scalar;
	Point<3> p5 = p4 - p3;


	Polinomial<1> pol1(1,-2);
	Polinomial<1> pol2(2,4);
	cout << "Polinomi di una variabile: " << endl;
	cout << "poli1" << endl;
	pol1.Print(false);
	pol1.Print(true,"x");
	cout << "poli2" << endl;
	pol2.Print(false);
	pol2.Print();

	cout << endl << "Polinomio di 2 variabili poli3 = poli1 tensor poli2" << endl;
	Polinomial<2> pol3 = pol1.tensor(pol2);
	pol3.Print();

	Polinomial<3> pol4 = pol3.tensor(pol2);
	Polinomial<3> pol5 = pol1.tensor(pol3);

	cout << endl << "Polinomi di 3 variabili:" << endl;
	cout << "1D tensor 2D: poli3 tensor poli2" << endl;
	pol4.Print();
 	cout << "2D tensor 1D: poli1 tensor poli3" << endl;
	pol5.Print();

	Polinomial<4> pol6 = pol3.tensor(pol3);
	cout << endl << "Polinomio di 4 variabili poli4 = poli3 tensor poli3" << endl;
	pol6.Print();

	cout << endl << "VALUTAZIONE DEI POLINOMI" << endl;
	cout << endl << "valuto il polinomio" << endl;
	pol4.Print();
	cout << "nel punto " << endl;
	p5.Print();
	cout << "Risultato: " << pol4(p5) << endl;

	Polinomial<1> one(5,0);
	one[0] = 1;
	Polinomial<1> x(1,0);
	x[1] = 1;
	Polinomial<3> z = one.tensor(one).tensor(x);
	cout << endl << "valuto il polinomio p(x,y,z) = z" << endl;
	z.Print();
	cout << "nel punto " << endl;
	p5.Print();
	cout << "Risultato: " << z(p5) << endl;

	cout << "Polinomial test ended" << endl;
};

void test3(int argc, char ** argv)
{
	//to silence warning by the compiler 
	(void)argc;
	(void)argv;

	cout << endl << "Starting Basis test" << endl;

//#ifndef SINGLETON_ENABLED
//	size_t degree(2);

//	cout << "Provo a costruire la base trivial:" << endl;
//	PoliBaseBuilderRule_ptr trivial = unique_ptr<TrivialBuilder>(new TrivialBuilder);
//	vector<Polinomial<1>> pol = (*trivial)(degree);
//////TODO: confrontare polinomi
//	PolinomialBasis<1, TrivialType> not_orthonormal_first (degree);
//	cout << "Base Trivial 1D : grado " << not_orthonormal_first.Degree() << "; dimensione " << not_orthonormal_first.Size() << endl;
//	not_orthonormal_first.Print();

//	PolinomialBasis<3, TrivialType> not_orthonormal_third (degree);
//	cout << "Base Trivial 3D : grado " << not_orthonormal_third.Degree() << "; dimensione " << not_orthonormal_third.Size() << endl;
//	not_orthonormal_third.Print();

//	cout << "Provo a costruire la base di legendre:" << endl;
//	PoliBaseBuilderRule_ptr legendre = unique_ptr<LegendreBuilder>(new LegendreBuilder);
//	vector<Polinomial<1>> orthoPol = (*legendre)(degree);
//////TODO: confrontare polinomi
//	PolinomialBasis<1, LegendreType> orthonormal_first (degree);
//	cout << "Base Legendre 1D : grado " << orthonormal_first.Degree() << "; dimensione " << orthonormal_first.Size() << endl;
//	orthonormal_first.Print();
//	PolinomialBasis<2, LegendreType> orthonormal_second (degree);
//	cout << "Base Legendre 2D : grado " << orthonormal_second.Degree() << "; dimensione " << orthonormal_second.Size() << endl;
//	orthonormal_second.Print();

//#endif //SINGLETON_ENABLED

	cout << "Basis test ended" << endl;
};

void test4(int argc, char ** argv)
{
	cout << endl << "BinaryElements construction test" << endl; 
	libMesh::LibMeshInit init (argc, argv);
	GetPot command_line (argc, argv);

	int n = 2;
	if (command_line.search(1, "-n"))
		n = command_line.next(n);

	// Brief message to the user regarding the program name
	// and command line arguments.
//	libMesh::out << "Running " << argv[0];

	libMesh::Mesh mesh(init.comm());
	libMesh::MeshRefinement mesh_refiner(mesh);

	// Build a 1D mesh with n elements from x=0 to x=1, using
	libMesh::MeshTools::Generation::build_line(mesh, n, 0., 1., LibmeshIntervalType);

	BinaryTree::FunctionPtr<1> f_ptr = make_shared<MyFunctions::XSquared>();

	BinarityMap::MakeBinary<1> (mesh_refiner, f_ptr);

#ifdef MYDEBUG
	cout << "Sono uscito da make binary" << endl;
	if (BinarityMap::CheckBinarity(mesh))
		cout << "Good! La mesh e' binaria" << endl;
	else
		cout << "ATTENZIONE! La mesh non e' binaria!" << endl;
#endif //MYDEBUG

//	mesh.prepare_for_use(/*skip_renumber =*/ false);

	//TODO: check the correctness of the construction
	//TODO: construct other possible binary objects

	cout << "BinaryElements construction test ended" << endl;
};


void test5(int argc, char ** argv)
{
	//to silence warning by the compiler 
	(void)argc;
	(void)argv;

	cout << endl << "Starting Integration on standard elements test" << endl;

	//interval (-1, 1)
#ifndef SINGLETON_ENABLED
	StdIperCube<1> std_interval;
#else //SINGLETON_ENABLED
	shared_ptr<StdIperCube<1>> interval_ptr = StdIperCube<1>::Instance();
	StdIperCube<1>& std_interval(*interval_ptr);
#endif //SINGLETON_ENABLED

	cout << "Calcolo l'integrale tra -1 e 1 di x" << endl;
	double val1 = std_interval.Integrate([](const Point<1>& x){return x[0];});
	double solex1 = 0;
	cout << "Soluzione esatta : " << solex1 << endl;
	cout << "Soluzione numerica : " << val1 << endl;
	cout << "errore : " << abs(val1 - solex1) << endl;

	cout << "Calcolo l'integrale tra -1 e 1 di x^2" << endl;
	double val2 = std_interval.Integrate([](const Point<1>& x){return x[0]*x[0];});
	double solex2 = 2; solex2 /= 3;
	cout << "Soluzione esatta : " << solex2 << endl;
	cout << "Soluzione numerica : " << val2 << endl;
	cout << "errore : " << abs(val2 - solex2) << endl;

	cout << "Calcolo l'integrale tra -1 e 1 di x^42" << endl;
	double val6 = std_interval.Integrate([](const Point<1>& x){return pow(x[0], 42);});
	double solex6 = 2; solex6 /= 43;
	cout << "Soluzione esatta : " << solex6 << endl;
	cout << "Soluzione numerica : " << val6 << endl;
	cout << "errore : " << abs(val6 - solex6) << endl;

	cout << "Calcolo l'integrale tra -1 e 1 di x^44" << endl;
	double val8 = std_interval.Integrate([](const Point<1>& x){return pow(x[0], 44);});
	double solex8 = 2; solex8 /= 45;
	cout << "Soluzione esatta : " << solex8 << endl;
	cout << "Soluzione numerica : " << val8 << endl;
	cout << "errore : " << abs(val8 - solex8) << endl;

	cout << "Calcolo l'integrale tra -1 e 1 di x^49" << endl;
	double val3 = std_interval.Integrate([](const Point<1>& x){return pow(x[0], 49);});
	double solex3 = 0;
	cout << "Soluzione esatta : " << solex3 << endl;
	cout << "Soluzione numerica : " << val3 << endl;
	cout << "errore : " << abs(val3 - solex3) << endl;

	cout << "Calcolo l'integrale tra -1 e 1 di 1E+30 * x^99" << endl;
	double val5 = std_interval.Integrate([](const Point<1>& x){return pow(10.0, 30) * pow(x[0], 99);});
	double solex5 = 0;
	cout << "Soluzione esatta : " << solex5 << endl;
	cout << "Soluzione numerica : " << val5 << endl;
	cout << "errore : " << abs(val5 - solex5) << endl;

	cout << "Calcolo l'integrale tra -1 e 1 di x^99" << endl;
	double val9 = std_interval.Integrate([](const Point<1>& x){return pow(x[0], 99);});
	double solex9 = 0;
	cout << "Soluzione esatta : " << solex9 << endl;
	cout << "Soluzione numerica : " << val9 << endl;
	cout << "errore : " << abs(val9 - solex9) << endl;

	cout << "Calcolo l'integrale tra -1 e 1 di x^100" << endl;
	double val7 = std_interval.Integrate([](const Point<1>& x){return pow(x[0], 100);});
	double solex7 = 2; solex7 /= 101;
	cout << "Soluzione esatta : " << solex7 << endl;
	cout << "Soluzione numerica : " << val7 << endl;
	cout << "errore : " << abs(val7 - solex7) << endl;

	//standard square
#ifndef SINGLETON_ENABLED
	StdIperCube<2> std_square;
#else //SINGLETON_ENABLED
	shared_ptr<StdIperCube<2>> square_ptr = StdIperCube<2>::Instance();
	StdIperCube<2>& std_square(*square_ptr);
#endif //SINGLETON_ENABLED

	cout << "Calcolo l'integrale sul quadrato tra -1 e 1 di x^2 + y^2" << endl;
	double val4 = std_square.Integrate([](const Point<2>& x){return x[0]*x[0]+x[1]*x[1];});
	double solex4 = 4 * solex2;
	cout << "Soluzione esatta : " << solex4 << endl;
	cout << "Soluzione numerica : " << val4 << endl;
	cout << "errore : " << abs(val4 - solex4) << endl;
	
	//TODO : make other integrals
	//TODO : check other standard elements

	cout << "Integration on standard elements test ended " << endl; 
};

void test6(int argc, char ** argv)
{
	cout << endl << "Starting Maps test" << endl;

	libMesh::LibMeshInit init (argc, argv);

	int n = 1;

	libMesh::Mesh mesh1(init.comm());

	// Build a 1D mesh with n elements from x=1 to x=2
	libMesh::MeshTools::Generation::build_line(mesh1, n, 1., 2., LibmeshIntervalType);

	libMesh::Elem* element = mesh1.elem_ptr(0);

	Point<1> p1({-1});
	Point<1> p2({0});
	Point<1> p3({1});

	cout << "Creo la mappa da (-1, 1) a (1, 2)" << endl;
	IntervalMap map1;

	//TODO: avoid to repeat always this same code, rewrite after the code is library independent
	auto geometry_nodes1 = element->get_nodes();
	auto length1 = element->n_nodes();
	NodesVector<1> result1;
	for (size_t i(0); i < length1; ++i)
	{
		auto current_ptr = geometry_nodes1[i];
		Point<1> p;
		for (size_t j(0); j < 1; ++j)
			p[j] = (*current_ptr)(j);

		result1.push_back(p);
	}

	map1.Init(result1);
	cout << "Mappa creata" << endl;

	Point<1> mapped_p1 = map1.Evaluate(p1);
	p1.Print(false);
	cout << " viene mappato in ";
	mapped_p1.Print();

	Point<1> mapped_p2 = map1.Evaluate(p2);
	p2.Print(false);
	cout << " viene mappato in ";
	mapped_p2.Print();

	Point<1> mapped_p3 = map1.Evaluate(p3);
	p3.Print(false);
	cout << " viene mappato in ";
	mapped_p3.Print();

	cout << "Provo l'inversa" << endl;
	Point<1> inverse_p1 = map1.ComputeInverse(mapped_p1);
	cout << "Voglio riottenere : ";
	p1.Print();
	cout << "Ho : ";
	inverse_p1.Print();

	Point<1> inverse_p2 = map1.ComputeInverse(mapped_p2);
	cout << "Voglio riottenere : ";
	p2.Print();
	cout << "Ho : ";
	inverse_p2.Print();

	Point<1> inverse_p3 = map1.ComputeInverse(mapped_p3);
	cout << "Voglio riottenere : ";
	p3.Print();
	cout << "Ho : ";
	inverse_p3.Print();

	cout << endl << "2D test" << endl << endl;

	int nx = 1;
	int ny = 1;

	libMesh::Mesh mesh2(init.comm());

	// Build a 2D square (1,2)x(1,2) mesh with nx subdivisions along x axis and ny subdivisions along y axis
	libMesh::MeshTools::Generation::build_square(mesh2, nx, ny, 1., 2., 1., 2., libMesh::TRI3);
	cout << endl << "Mesh quadrata costruita" << endl;
	cout << "La mesh ha " << mesh2.n_elem() << " elementi" << endl;

	Point<2> p4({0,0});
	Point<2> p5({1,0});	
	Point<2> p6({0.5,0.5});
	Point<2> p7({0,1});

	libMesh::Elem* tri_elem1 = mesh2.elem_ptr(0);
	libMesh::Elem* tri_elem2 = mesh2.elem_ptr(1);

	cout << "Creo la mappa per il primo triangolo" << endl;
	TriMap map2;

	auto geometry_nodes2 = tri_elem1->get_nodes();
	auto length2 = tri_elem1->n_nodes();
	NodesVector<2> result2;
	for (size_t i(0); i < length2; ++i)
	{
		auto current_ptr = geometry_nodes2[i];
		Point<2> p;
		for (size_t j(0); j < 2; ++j)
			p[j] = (*current_ptr)(j);

		result2.push_back(p);
	}

	map2.Init(result2);
	cout << "Mappa creata" << endl;

	Point<2> mapped_p4 = map2.Evaluate(p4);
	p4.Print(false);
	cout << " viene mappato in ";
	mapped_p4.Print();

	Point<2> mapped_p5 = map2.Evaluate(p5);
	p5.Print(false);
	cout << " viene mappato in ";
	mapped_p5.Print();

	Point<2> mapped_p6 = map2.Evaluate(p6);
	p6.Print(false);
	cout << " viene mappato in ";
	mapped_p6.Print();

	Point<2> mapped_p7 = map2.Evaluate(p7);
	p7.Print(false);
	cout << " viene mappato in ";
	mapped_p7.Print();
	
	cout << "Creo la mappa per il secondo triangolo" << endl;
	TriMap map3;

	auto geometry_nodes3 = tri_elem2->get_nodes();
	auto length3 = tri_elem2->n_nodes();
	NodesVector<2> result3;
	for (size_t i(0); i < length3; ++i)
	{
		auto current_ptr = geometry_nodes3[i];
		Point<2> p;
		for (size_t j(0); j < 2; ++j)
			p[j] = (*current_ptr)(j);

		result3.push_back(p);
	}

	map3.Init(result3);
	cout << "Mappa creata" << endl;

	Point<2> mapped_p8 = map3.Evaluate(p4);
	p4.Print(false);
	cout << " viene mappato in ";
	mapped_p8.Print();

	Point<2> mapped_p9 = map3.Evaluate(p5);
	p5.Print(false);
	cout << " viene mappato in ";
	mapped_p9.Print();

	Point<2> mapped_p10 = map3.Evaluate(p6);
	p6.Print(false);
	cout << " viene mappato in ";
	mapped_p10.Print();

	Point<2> mapped_p11 = map3.Evaluate(p7);
	p7.Print(false);
	cout << " viene mappato in ";
	mapped_p11.Print();

	//TODO: check the inverse
	//TODO: check singular maps between reference elements

	cout << "Maps test ended" << endl;
};

void test7(int argc, char ** argv)
{
	cout << endl << "Starting Integration on generic elements test" << endl;
	
	libMesh::LibMeshInit init (argc, argv);

	int n = 1;

	libMesh::Mesh mesh1(init.comm());

	// Build a 1D mesh with n elements from x=1 to x=2
	libMesh::MeshTools::Generation::build_line(mesh1, n, 1., 2., LibmeshIntervalType);

	auto iter = mesh1.elements_begin();
	auto& element = *iter;
	libMesh::Edge2* libmesh_interval = dynamic_cast<libMesh::Edge2*> (element);
	unique_ptr<libMesh::Edge2> int_ptr (libmesh_interval);

	cout << "Costruisco FElement" << endl;
	FElement<1, LegendreType, LibmeshIntervalClass> interval(move(int_ptr));
	element = nullptr;
	cout << "Inizializzo interval" << endl;
	try
	{
		interval.Init();
	}
	catch(invalid_argument& ia)
	{
		cerr << "Inizializzazione dell'intervallo fallita" << endl;
		cerr << ia.what() << endl;
		return;
	}
	cout << "interval inizializzato" << endl;

	cout << "Calcolo l'integrale tra 1 e 2 di x^2" << endl;	
	double val5 = interval.Integrate([](const Point<1>& x){return x[0]*x[0];});
	double solex5 = 7; solex5 /= 3; 
	cout << "Soluzione esatta : " << solex5 << endl;
	cout << "Soluzione numerica : " << val5 << endl;
	cout << "errore : " << abs(val5 - solex5) << endl;	

	cout << "Integration on generic elements test ended" << endl;
};


void test8(int argc, char ** argv)
{
	cout << endl << "Starting libMesh version compatibility test" << endl;

	//to silence warning by the compiler 
	(void)argc;
	(void)argv;

	//TODO

	cout << "libMesh version compatibility test ended" << endl;
};


void test9(int argc, char ** argv)
{
	//to silence warning by the compiler 
	(void)argc;
	(void)argv;

	cout << endl << "Starting Basis orthonormality test" << endl;

#ifndef SINGLETON_ENABLED
	StdFIperCube<1, LegendreType> sfic;
#else //SINGLETON_ENABLED
	shared_ptr<StdFIperCube<1, LegendreType> > sfic_ptr = StdFIperCube<1, LegendreType>::Instance();
	StdFIperCube<1, LegendreType>& sfic(*sfic_ptr);
#endif //SINGLETON_ENABLED
#ifdef MYDEBUG
	cout << "Nodi di quadrature sull'intervallo (-1, 1)" << endl;
	for (auto point : sfic.GetQuadPoints())
		cout << point[0] << " ";
	cout << endl;

	cout << "Controllo che i nodi siano simmetrici" << endl;
	auto points = sfic.GetQuadPoints();
	auto right_begin = points.rbegin();
	for(auto point : sfic.GetQuadPoints())
	{
		cout << point[0] + (*right_begin)[0] << " " << endl;
		++right_begin;
	}

	cout << "Pesi di quadrature sull'intervallo (-1, 1)" << endl;
	auto weights = sfic.GetQuadWeights();
	for (size_t i(0); i < weights.size(); ++i)
		cout << weights(i) << " ";
	cout << endl;

	cout << "Controllo che i pesi siano simmetrici" << endl;
	int i = 0, j = weights.size() - 1;
	for(; i < weights.size(); ++i)
	{
		cout << weights(i) - weights(j) << " " << endl;
		--j;
	}
#endif //MYDEBUG

	cout << "Let's check the orthogonality on the standard interval:" << endl;
	for (size_t k(1); k < /*8; ++i)
	{
		double I = sfic.L2prod	(
											[&](const Point<1>& p){ return sfic.EvaluateBasisFunction(k+1, p);},
											[&](const Point<1>& p){ return sfic.EvaluateBasisFunction(k, p);}
										);

		cout << "Integral #" << i << " : " << I << endl;
	}

	cout << "Let's check the orthogonality at high orders:" << endl;
	size_t k = 40, t = k - 1;
	for (; k < */50; ++k)
	{
		double I = sfic.L2prod	(
											[&](const Point<1>& p){ return sfic.EvaluateBasisFunction(0, p);},
											[&](const Point<1>& p){ return sfic.EvaluateBasisFunction(k, p);}
										);

		cout << "Integral #" << k << " : " << I << endl;
	}

	cout << endl << endl << "Let's check the normality on the standard interval :" << endl;
	for (size_t i(0); i < /*8; ++i)
	{
		double I = sfic.L2prod	(
											[&](const Point<1>& p){ return sfic.EvaluateBasisFunction(i, p);},
											[&](const Point<1>& p){ return sfic.EvaluateBasisFunction(i, p);}
										);
		I *= (static_cast<double>(i) + 0.5);
#ifdef MYDEBUG
		cout << "Integrating the square of" << endl;
		sfic.PrintBasis(i);
#endif //MYDEBUG
		cout << "Integral #" << i << " : " << I << endl;
	}	

	cout << "Let's check the normality at high orders :" << endl;
	for (size_t i(50); i < */58; ++i)
	{
		double I = sfic.L2prod	(
											[&](const Point<1>& p){ return sfic.EvaluateBasisFunction(i, p);},
											[&](const Point<1>& p){ return sfic.EvaluateBasisFunction(i, p);}
										);
		I *= (static_cast<double>(i) + 0.5);
#ifdef MYDEBUG
		cout << "Integrating the square of" << endl;
		sfic.PrintBasis(i);
#endif //MYDEBUG
		cout << "Integral #" << i << " : " << I << endl;
	}	

	cout << "Basis orthonormality test ended" << endl;
};

void test10(int argc, char ** argv)
{
	cout << endl << "Starting Projection on element test" << endl;

	BinaryTree::FunctionPtr<1> f_ptr = make_shared<MyFunctions::XSquared>();
//	BinaryTree::FunctionPtr<1> f_ptr = make_shared<MyFunctions::AdvectionDiffusionSolution<100>>();

	libMesh::LibMeshInit init (argc, argv);
	libMesh::Mesh mesh(init.comm());
	libMesh::MeshRefinement mesh_refiner(mesh);
	libMesh::MeshTools::Generation::build_line(mesh, 1, 0, 1, LibmeshIntervalType);

	BinarityMap::MakeBinary<1> (mesh_refiner, f_ptr);

	auto el = dynamic_cast<BinaryTreeElement<1, LegendreType, LibmeshIntervalClass>*> (mesh.elem_ptr(0));
	const auto& f_el(el->GetFElement());

	cout << "Let's check the basis orthogonality on the (0, 1) interval:" << endl;
	for (size_t i(0); i < 100; ++i)
	{
		double I = f_el.L2prod(
										[&](const Point<1>& p){ return f_el.EvaluateBasisFunction(i+2, p);},
										[&](const Point<1>& p){ return f_el.EvaluateBasisFunction(i, p);}
									 );
		cout << "Integral #" << i << " : " << I << endl;
	}

	cout << "Let's check the basis normality on the (0, 1) interval :" << endl;
	for (size_t i(0); i < 100; ++i)
	{
		double I = f_el.L2prod	(
											[&](const Point<1>& p){ return f_el.EvaluateBasisFunction(i, p);},
											[&](const Point<1>& p){ return f_el.EvaluateBasisFunction(i, p);}
										);
		I *= i + 0.5;
		cout << "Integral #" << i << " : " << I << endl;
	}



	cout << "P level : " << el->PLevel() << endl;
	cout << "coefficienti : ";
	el->PrintCoefficients();
	double error = el->ProjectionError();
	cout << "Errore di interpolazione su P" << el->PLevel() << " : " << error << endl;

	for (size_t p = 1; p < 25; ++p)
	{
		el->PLevel(p);
		cout << "P level : " << el->PLevel() << endl;
//		cout << "coefficienti : ";
//		el->PrintCoefficients();
		error = el->ProjectionError();
		cout << "Errore di interpolazione su P" << el->PLevel() << " : " << error << endl;
	}

	cout << "Projection on element test ended" << endl;
};

void test11(int argc, char ** argv)
{
	cout << endl << "Starting manual libMesh refinement test" << endl;

	libMesh::LibMeshInit init (argc, argv);

	int n = 1;

	libMesh::Mesh mesh(init.comm());

	// Build a 1D mesh with n elements from x=1 to x=2
	libMesh::MeshTools::Generation::build_line(mesh, n, 1., 2., libMesh::EDGE2);

	libMesh::Elem* element = *(mesh.elements_begin());

#ifdef ACTIVATE_BUG
	libMesh::Edge2* copy_element_ptr = dynamic_cast<libMesh::Edge2*>(element);
	libMesh::Edge2 copy_element = *copy_element_ptr;
	copy_element_ptr = &copy_element;
	if (copy_element_ptr->operator== (*element))
		cout << "I DUE ELEMENTI SONO UGUALI" << endl;
	auto el_nodes = element->get_nodes();
	auto copy_el_nodes = copy_element_ptr->get_nodes();
	cout << "el_nodes ha size : " << (*el_nodes)->size() << endl;
	cout << "copy_el_nodes ha size : " << (*copy_el_nodes)->size() << endl;
	
	cout << "primo nodo :" << (*(el_nodes[0]))(0) << endl;
	cout << "primo nodo copia :" << (*(copy_el_nodes[0]))(0) << endl;
	cout << "secondo nodo :" << (*(el_nodes[1]))(0) << endl;
	cout << "secondo nodo copia :" << (*(copy_el_nodes[1]))(0) << endl;

	cout << "Indirizzo dei nodi di element	" << el_nodes[0] << endl;
	cout << "Indirizzo dei nodi della copia	" << copy_el_nodes[0] << endl;
	//fundamental instruction to have the bug
	mesh.insert_elem(copy_element_ptr);

	if (copy_element_ptr->operator== (**(mesh.elements_begin())) )
		cout << "DOPO L'INSERT L'ELEMENTO E' UGUALE A QUELLO DI PARTENZA" << endl;
	el_nodes = element->get_nodes();
	copy_el_nodes = mesh.elem_ptr(0)->get_nodes();	
	cout << "primo nodo :" << (*(el_nodes[0]))(0) << endl;
	cout << "primo nodo copia :" << (*(copy_el_nodes[0]))(0) << endl;
	cout << "secondo nodo :" << (*(el_nodes[1]))(0) << endl;
	cout << "secondo nodo copia :" << (*(copy_el_nodes[1]))(0) << endl;

	mesh.prepare_for_use(/*skip_renumber =*/ false);

	element = copy_element_ptr;
#endif //ACTIVATE_BUG

	element->set_refinement_flag(libMesh::Elem::REFINE);

	libMesh::MeshRefinement refiner(mesh);
	//the test crashes on this refine if ACTIVATE_BUG is defined
	element->refine(refiner);

	int cont = mesh.n_elem();

	cout << "I start one refinement iteration on the first mesh element" << endl;

	for (auto iter = mesh.elements_begin(); iter != mesh.elements_end(); ++iter)
		if ((*iter)->refinement_flag() == libMesh::Elem::INACTIVE)
			--cont;

	cout << "I have started from a mesh with " << n << " elements; after refinement it has " << mesh.n_elem() << " elements, " << cont << " of them are ACTIVE" << endl;

	cout << endl << "Manual libMesh refinement test ended" << endl;
};

void test12(int argc, char ** argv)
{
	cout << endl << "Starting manual binary refinement test" << endl;

	libMesh::LibMeshInit init (argc, argv);

	int n = 1;

	libMesh::Mesh mesh(init.comm());

	// Build a 1D mesh with n elements from x=1 to x=2
	libMesh::MeshTools::Generation::build_line(mesh, n, 1., 2., libMesh::EDGE2);
	libMesh::MeshRefinement refiner(mesh);

	BinaryTree::FunctionPtr<1> f_ptr = make_shared<MyFunctions::XSquared>();

//#define MAKE_OPTION
#ifdef MAKE_OPTION
//TODO: does not work since I don't replace the mesh element
//		  maybe it could work if the libMeshBinaryElement takes
//		  as constructor parameter the mesh iterator instead of
//		  the poiter to the element, so in constructor it can do
//		  the replacement by itself 
	BinarityMap::MakeBinary<1>(refiner, f_ptr);
	Interval* I = dynamic_cast<Interval*>(mesh.elem(0));
	if (I == nullptr)
		cout << "MakeBinary has not worked properly!";

	cout << "I directly call the Interval refine method" << endl;
	I->Bisect();

#else //MAKE_OPTION

	//construction ed initialization of a Binary object of type Interval
	auto iter = mesh.elements_begin();
	auto ptr = dynamic_cast<libMesh::Edge2*>(*iter);
	auto smart_ptr = unique_ptr<libMesh::Edge2>(ptr);
	Interval* I = new Interval(move(smart_ptr), f_ptr, refiner);
	I->Init();
	*iter = I;

	//to check if I has a LibmeshGeometry method as expected
	cout << "The interval has refinement_flag = " << I->refinement_flag() << endl;

	cout << "I directly call the Interval refine method" << endl;
	I->Bisect();
#endif //MAKE_OPTION

	BinarityMap::MakeBinary<1>(refiner, f_ptr);

	int cont = mesh.n_elem();

	for (auto iter = mesh.elements_begin(); iter != mesh.elements_end(); ++iter)
		if ((*iter)->refinement_flag() == libMesh::Elem::INACTIVE)
			--cont;

	cout << "I have started from a mesh with " << n << " elements; after refinement it has " << mesh.n_elem() << " elements, " << cont << " of them are ACTIVE" << endl;

	cout << endl << "Manual binary refinement test ended" << endl;
};
#include <quadrature.h>
void test13(int argc, char** argv)
{
	cout << endl << "Starting BinaryRefinement test" << endl;
	auto f_ptr = HelperFunctions::MakeUnique<MyFunctions::XSquared>();

	libMesh::LibMeshInit init (argc, argv);
	auto mesh_ptr = make_shared<libMesh::Mesh> (init.comm());

	int n = 2;
	libMesh::MeshTools::Generation::build_line(*mesh_ptr, n, 0, 1, LibmeshIntervalType);
	LibmeshBinary::BinaryRefiner<1> binary_refiner;
	binary_refiner.Init(move(f_ptr));
	binary_refiner.SetMesh(mesh_ptr);

	size_t n_iter(10);
	cout << "Starting Binary Tree hp Refinement with " << n_iter << " degrees of freedom that can be added" << endl;
	binary_refiner.Refine(n_iter);

	int cont = mesh_ptr->n_elem();

	for (auto iter = mesh_ptr->elements_begin(); iter != mesh_ptr->elements_end(); ++iter)
		if ((*iter)->refinement_flag() == libMesh::Elem::INACTIVE)
			--cont;
		else
			cout << "Active Element of polinomial degree : " << (*iter)->p_level() << endl;
	cout << "I have started from a mesh with " << n << " elements; after refinement it has " << mesh_ptr->n_elem() << " elements, " << cont << " of them are ACTIVE" << endl;

	cout << endl << "Now I check the same thing with the active elements iteration" << endl;
	cout << "The mesh has " << binary_refiner.ActiveNodesNumber() << " ACTIVE elements" << endl;

	vector<size_t> p_levels = binary_refiner.ExtractPLevels();
	cout << "Livelli di p refinement : " << endl;
	for (auto l : p_levels)
		cout << l << " ";
	cout << endl;

	cout << "BinaryRefinement test ended" << endl;
};

void test14(int argc, char** argv)
{
	cout << endl << "Starting libMesh file IO test" << endl;
	libMesh::LibMeshInit init (argc, argv);
	libMesh::Mesh mesh(init.comm());

	// Build a 2D square (1,2)x(1,2) mesh with nx subdivisions along x axis and ny subdivisions along y axis
	int nx = 1, ny = 1;
	libMesh::MeshTools::Generation::build_square(mesh, nx, ny, 1., 2., 1., 2., libMesh::TRI3);
	mesh.write("./mesh2d.msh");

	cout << "libMesh file IO test ended" << endl;
};

void test15(int argc, char** argv)
{
	cout << endl << "Starting 1D refining complete test" << endl;

	libMesh::LibMeshInit init (argc, argv);
	vector<double> error;

	for (size_t n_iter = 0; n_iter < 51; ++n_iter)
	{
		cerr << endl << "#DOF = " << n_iter << endl;
		auto mesh_ptr = make_shared<libMesh::Mesh> (init.comm());

		int n = 1;
		libMesh::MeshTools::Generation::build_line(*mesh_ptr, n, 0, 1, LibmeshIntervalType);
		LibmeshBinary::BinaryRefiner<1> binary_refiner;

//		auto f_ptr = HelperFunctions::MakeUnique<MyFunctions::XSquared>();
//		auto f_ptr = HelperFunctions::MakeUnique<MyFunctions::XExpBeta<40>>();
//		auto f_ptr = HelperFunctions::MakeUnique<MyFunctions::SqrtX>();
//		auto f_ptr = HelperFunctions::MakeUnique<MyFunctions::HalfStep>();
//		auto f_ptr = HelperFunctions::MakeUnique<MyFunctions::HalfSqrt>();
//		auto f_ptr = HelperFunctions::MakeUnique<MyFunctions::HalfX>();
//		auto f_ptr = HelperFunctions::MakeUnique<MyFunctions::HalfSquare>();
//		auto f_ptr = HelperFunctions::MakeUnique<MyFunctions::HalfTwenty>();
		auto f_ptr = HelperFunctions::MakeUnique<MyFunctions::AdvectionDiffusionSolution<100>>();
		cout << "Function: " << f_ptr->Name() << endl;

		binary_refiner.Init(move(f_ptr));
		binary_refiner.SetMesh(mesh_ptr);

		binary_refiner.Refine(n_iter);
	
		auto current_error = binary_refiner.GlobalError();
		error.push_back(current_error);
		cout << endl << "Interpolation error after refinement >>>>>> " << current_error << endl << endl;;
//		auto interpolated_f = binary_refiner.ExtractInterpolatedFunction();

		vector<size_t> p_levels = binary_refiner.ExtractPLevels();
		cout << "Livelli di p refinement : " << endl;
		for (auto l : p_levels)
			cout << l << " ";
		cout << endl;
		cout << "The mesh has " << binary_refiner.ActiveNodesNumber() << " ACTIVE elements" << endl;
	}
	
	cout << endl << "N vs Projection Error:" << endl;
	size_t cont = 0;
	for (auto e : error)
		cout << cont++ << "	:	" << e << endl;
	
	cout << "1D refining complete test ended" << endl;
};

void test16(int argc, char ** argv)
{
	cout << endl << "Starting bug isolated test" << endl;

	BinaryTree::FunctionPtr<1> f_ptr = make_shared<MyFunctions::XSquared>();

	libMesh::LibMeshInit init (argc, argv);
	libMesh::Mesh mesh(init.comm());
	libMesh::MeshRefinement mesh_refiner(mesh);
	libMesh::MeshTools::Generation::build_line(mesh, 1, 0, 1, LibmeshIntervalType);

	BinarityMap::MakeBinary<1> (mesh_refiner, f_ptr);

	auto el = dynamic_cast<BinaryTreeElement<1, LegendreType, LibmeshIntervalClass>*> (mesh.elem_ptr(0));
	const auto& f_el(el->GetFElement());

	size_t i(52);
	cout << "Let's check the normality of the element #" << i << " of the basis :" << endl;

	double I = f_el.L2prod	(
										[&](const Point<1>& p){ return f_el.EvaluateBasisFunction(i, p);},
										[&](const Point<1>& p){ return f_el.EvaluateBasisFunction(i, p);}
									);
	I *= i + 0.5;
	cout << "Integral: " << I << endl;

	cout << "Bug isolated test ended" << endl;
};

