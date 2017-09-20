#include "Test.h"

#include <array>
#include <utility> //move

#include "Point.h"
#include "Polinomial.h"
#include "StdFElement.h"
#include "BinaryTreeHelper.h"

#include "LibMeshRefiner.h"
#include "LibMeshBinaryElements.h"

#include "libmesh/mesh_generation.h" //MeshTools
	
using namespace std;
using namespace Geometry;
using namespace BinaryTree;
using namespace FiniteElements;
using namespace LibmeshBinary;

Logfile::Logfile(const string& filename) : _ofs(filename, ios_base::app)
														,_out_buf(cout.rdbuf(_ofs.rdbuf()))
														,_log_buf(clog.rdbuf(_ofs.rdbuf()))
														,_err_buf(cerr.rdbuf(_ofs.rdbuf()))
														,_libmesh_err_buf(libMesh::err.rdbuf(_ofs.rdbuf()))
														,_libmesh_out_buf(libMesh::out.rdbuf(_ofs.rdbuf()))
{
#ifdef MYDEBUG
	clog << "I buffer sono su file" << endl;
#endif //MYDEBUG
};

Logfile::~Logfile()
{
#ifdef MYDEBUG
	clog << "Redirigo i buffer sullo standard" << endl;
#endif //MYDEBUG
	clog.rdbuf(_log_buf);
	cout.rdbuf(_out_buf);
	cerr.rdbuf(_err_buf);
	libMesh::err.rdbuf(_libmesh_err_buf);
	libMesh::out.rdbuf(_libmesh_out_buf);
#ifdef MYDEBUG
	cerr << "Buffer rediretti" << endl;
#endif //MYDEBUG
};

bool Initializer::Load(const vector<string>& handles)
{
	for (auto& handle : handles)
		_pl.Add(handle);
	return _pl.Load();
};

LoadTest::LoadTest() : _mesh_init_ptr(nullptr), _out("test.log")
{
#ifdef MYDEBUG
	clog << "Sono nel costruttore di LoadTest" << endl;
#endif //MYDEBUG

	string exe_name = "test";
	const char* trivialargv[] = {exe_name.c_str()};
	_mesh_init_ptr = HelperFunctions::MakeUnique<libMesh::LibMeshInit>(1, trivialargv);
};

LoadTest::~LoadTest()
{
#ifdef DESTRUCTOR_ALERT
	clog << "Sono nel distruttore di LoadTest" << endl;
#endif //DESTRUCTOR_ALERT
};

void LoadTest::TearDown()
{
#ifdef MYDEBUG
	clog << "Sono nella TearDown di LoadTest" << endl;
#endif //MYDEBUG
};

void LoadTest::SetUp()
{
#ifdef MYDEBUG
	clog << "Sono nella SetUp di LoadTest" << endl;
#endif //MYDEBUG

	vector<string> handles;
#ifndef DEBUG
	handles.push_back("libmesh_quadrature.so");
	handles.push_back("libsandia_quadrature.so");
	handles.push_back("libinterpolating_functions.so");
#else //DEBUG
	handles.push_back("libmesh_quadrature_Debug.so");
	handles.push_back("libsandia_quadrature_Debug.so");
	handles.push_back("libinterpolating_functions_Debug.so");
#endif //DEBUG
	if (!_initializer.Load(handles))
		throw runtime_error ("Houston we have a problem: something went wrong loading plugins");

};

TEST_F(LoadTest, IsRegistered)
{
//	clog << endl << "Starting IsRegistered" << endl;

	auto& quad_fac(Geometry::QuadratureFactory<1>::Instance());
	auto quad_ptr = quad_fac.create(Geometry::IntervalType);
	EXPECT_NE(quad_ptr.get(), nullptr);

//	clog << "IsRegisterd ended" << endl << endl;
};


TEST_F(LoadTest, PointTest)
{
	clog << endl << "Starting PointTest" << endl;

	Point<1> p_test({1});
	EXPECT_EQ(p_test[0], 1) << "1d construction by initializer list failed";
	double p = p_test;
	EXPECT_EQ(p, 1) << "1d cast to double failed";	

	Point<1> p_copy(p_test);
	Point<1> p1 = p_copy;

	EXPECT_EQ(p1[0], 1) << "Assignment operator failed";

	array<double, 2> init = {2,3};
	Point<2> p2(init);
	EXPECT_EQ(p2[0], 2) << "2d construction by initializer list failed";
	EXPECT_EQ(p2[1], 3) << "2d construction by initializer list failed";

	Point<3> p3 = p1.Tensor(p2);
	EXPECT_EQ(p3[0], 1) << "Tensor product failed";
	EXPECT_EQ(p3[1], 2) << "Tensor product failed";
	EXPECT_EQ(p3[2], 3) << "Tensor product failed";

	double scalar(2);
	Point<3> p4 = p3 * scalar;

	EXPECT_EQ(p4[0], 2) << "Product by scalar failed";
	EXPECT_EQ(p4[1], 4) << "Product by scalar failed";
	EXPECT_EQ(p4[2], 6) << "Product by scalar failed";

	Point<3> p5 = p4 - p3;

	auto diff = abs(p5.Abs() - sqrt(14));
	EXPECT_LT(diff, 1E-4) << "Vector norm failed";

	auto dist_diff = abs(p5.distance(p4) - sqrt(14));
	EXPECT_LT(dist_diff, 1E-4) << "Vector distance failed";
	//TODO: test other member functions

	clog << "PointTest ended" << endl << endl;
};


TEST_F(LoadTest, BinaryElementsConstruction)
{
	clog << endl << "Starting BinaryElementsConstruction" << endl;

	int n = 2;
	libMesh::Mesh mesh(_mesh_init_ptr->comm());

	libMesh::MeshRefinement mesh_refiner(mesh);

////Build a 1D mesh with n elements from x=0 to x=1, using
	libMesh::MeshTools::Generation::build_line(mesh, n, 0., 1., LibmeshIntervalType);

	auto& f_factory(FunctionsFactory<1>::Instance());
	std::unique_ptr<Functor<1>> smart_ptr = f_factory.create("x_squared");
	BinaryTree::FunctionPtr<1> f_ptr (smart_ptr.release());

	BinarityMap::MakeBinary<1> (mesh_refiner, f_ptr);

	ASSERT_TRUE(BinarityMap::CheckBinarity(mesh)) << "After MakeBinary mesh is not binary!";

	mesh.prepare_for_use(/*skip_renumber =*/ false);

////TODO: add other assertion

////TODO: construct other possible binary objects

	clog << "BinaryElementsConstruction ended" << endl << endl;
};


TEST_F(LoadTest, StdIntegration)
{
	clog << endl << "Starting StdIntegration" << endl;

	//interval (-1, 1)
#ifndef SINGLETON_ENABLED
	StdIperCube<1> std_interval;
#else //SINGLETON_ENABLED
	shared_ptr<StdIperCube<1>> interval_ptr = StdIperCube<1>::Instance();
	StdIperCube<1>& std_interval(*interval_ptr);
#endif //SINGLETON_ENABLED

	clog << "I check the simmetry of the quadrature nodes" << endl;
	auto points = std_interval.GetQuadPoints();
	auto right_begin = points.rbegin();
	for(auto point : points)
	{
		auto err = point[0] + (*right_begin)[0];
		EXPECT_LT(err, 1E-4) << "The point "
									+ to_string(point[0])
									+ " is not symmetric: error = "
									+ to_string(err);
		++right_begin;
	}

	clog << "I check the simmetry of the quadrature weights" << endl;
	auto weights = std_interval.GetQuadWeights();
	int i = 0, j = weights.size() - 1;
	for(; i < weights.size(); ++i)
	{
		auto err = weights(i) - weights(j);
		EXPECT_LT(err, 1E-4) << "The weight #"
									+ to_string(i)
									+ " valued "
									+ to_string(weights(i))
									+ " is not symmetric: error = "
									+ to_string(err);
		--j;
	}

	//TODO: add the check on the quadrature order, if exactly integrated verify if the tolerance should be decreased

	//I compute the integral of x between -1 and 1
	double val1 = std_interval.Integrate([](const Point<1>& x){return x[0];});

	EXPECT_LT(abs(val1), 1E-4) << "Wrong integral for x between -1 and 1";

	//I compute the integral of x^2 between -1 and 1
	double val2 = std_interval.Integrate([](const Point<1>& x){return x[0]*x[0];});
	double solex2 = 2; solex2 /= 3;
	EXPECT_LT(abs(val2 - solex2), 1E-4) << "Wrong integral for x^2 between -1 and 1";

	//TODO: decomment
//	//I compute the integral of x^42 between -1 and 1
//	double val6 = std_interval.Integrate([](const Point<1>& x){return pow(x[0], 42);});
//	double solex6 = 2; solex6 /= 43;
//	cout << "Soluzione esatta : " << solex6 << endl;
//	cout << "Soluzione numerica : " << val6 << endl;
//	cout << "errore : " << abs(val6 - solex6) << endl;

//	cout << "Calcolo l'integrale tra -1 e 1 di x^44" << endl;
//	double val8 = std_interval.Integrate([](const Point<1>& x){return pow(x[0], 44);});
//	double solex8 = 2; solex8 /= 45;
//	cout << "Soluzione esatta : " << solex8 << endl;
//	cout << "Soluzione numerica : " << val8 << endl;
//	cout << "errore : " << abs(val8 - solex8) << endl;

//	cout << "Calcolo l'integrale tra -1 e 1 di x^49" << endl;
//	double val3 = std_interval.Integrate([](const Point<1>& x){return pow(x[0], 49);});
//	double solex3 = 0;
//	cout << "Soluzione esatta : " << solex3 << endl;
//	cout << "Soluzione numerica : " << val3 << endl;
//	cout << "errore : " << abs(val3 - solex3) << endl;

//	cout << "Calcolo l'integrale tra -1 e 1 di 1E+30 * x^99" << endl;
//	double val5 = std_interval.Integrate([](const Point<1>& x){return pow(10.0, 30) * pow(x[0], 99);});
//	double solex5 = 0;
//	cout << "Soluzione esatta : " << solex5 << endl;
//	cout << "Soluzione numerica : " << val5 << endl;
//	cout << "errore : " << abs(val5 - solex5) << endl;

//	cout << "Calcolo l'integrale tra -1 e 1 di x^99" << endl;
//	double val9 = std_interval.Integrate([](const Point<1>& x){return pow(x[0], 99);});
//	double solex9 = 0;
//	cout << "Soluzione esatta : " << solex9 << endl;
//	cout << "Soluzione numerica : " << val9 << endl;
//	cout << "errore : " << abs(val9 - solex9) << endl;

//	cout << "Calcolo l'integrale tra -1 e 1 di x^100" << endl;
//	double val7 = std_interval.Integrate([](const Point<1>& x){return pow(x[0], 100);});
//	double solex7 = 2; solex7 /= 101;
//	cout << "Soluzione esatta : " << solex7 << endl;
//	cout << "Soluzione numerica : " << val7 << endl;
//	cout << "errore : " << abs(val7 - solex7) << endl;

//	//standard square
//#ifndef SINGLETON_ENABLED
//	StdIperCube<2> std_square;
//#else //SINGLETON_ENABLED
//	shared_ptr<StdIperCube<2>> square_ptr = StdIperCube<2>::Instance();
//	StdIperCube<2>& std_square(*square_ptr);
//#endif //SINGLETON_ENABLED

//	cout << "Calcolo l'integrale sul quadrato tra -1 e 1 di x^2 + y^2" << endl;
//	double val4 = std_square.Integrate([](const Point<2>& x){return x[0]*x[0]+x[1]*x[1];});
//	double solex4 = 4 * solex2;
//	cout << "Soluzione esatta : " << solex4 << endl;
//	cout << "Soluzione numerica : " << val4 << endl;
//	cout << "errore : " << abs(val4 - solex4) << endl;
	
	//TODO : make other integrals
	//TODO : check other standard elements

	clog << "StdIntegration ended" << endl << endl;
};

TEST_F(LoadTest, MapsTest)
{
	clog << endl << "Starting MapsTest" << endl;

	int n = 1;

	libMesh::Mesh mesh1(_mesh_init_ptr->comm());

	// Build a 1D mesh with n elements from x=1 to x=2
	libMesh::MeshTools::Generation::build_line(mesh1, n, 1., 2., LibmeshIntervalType);

	libMesh::Elem* element = mesh1.elem_ptr(0);

	Point<1> p1({-1});
	Point<1> p2({0});
	Point<1> p3({1});

	clog << "Creo la mappa da (-1, 1) a (1, 2)" << endl;
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
	clog << "Mappa creata" << endl;

	Point<1> mapped_p1 = map1.Evaluate(p1);
	auto err = abs(mapped_p1[0] - 1);
	EXPECT_LT(err, 1E-4) << "Point -1 expected to be mapped in 1, is mapped in " + to_string(mapped_p1[0]);

	Point<1> mapped_p2 = map1.Evaluate(p2);
	err = abs(mapped_p2[0] - 1.5);
	EXPECT_LT(err, 1E-4) << "Point 0 expected to be mapped in 1.5, is mapped in " + to_string(mapped_p2[0]);

	Point<1> mapped_p3 = map1.Evaluate(p3);
	err = abs(mapped_p3[0] - 2);
	EXPECT_LT(err, 1E-4) << "Point 1 expected to be mapped in 2, is mapped in " + to_string(mapped_p3[0]);


	clog << "Provo l'inversa" << endl;

	Point<1> inverse_p1 = map1.ComputeInverse(mapped_p1);
	err = abs(p1[0] - inverse_p1[0]);
	EXPECT_LT(err, 1E-4) << "Point 1 expected to be inverse mapped in -1, is mapped in " + to_string(inverse_p1[0]);

	Point<1> inverse_p2 = map1.ComputeInverse(mapped_p2);
	err = abs(p2[0] - inverse_p2[0]);
	EXPECT_LT(err, 1E-4) << "Point 1.5 expected to be inverse mapped in 0, is mapped in " + to_string(inverse_p2[0]);

	Point<1> inverse_p3 = map1.ComputeInverse(mapped_p3);
	err = abs(p3[0] - inverse_p3[0]);
	EXPECT_LT(err, 1E-4) << "Point 2 expected to be inverse mapped in 1, is mapped in " + to_string(inverse_p3[0]);

	clog << "2D test" << endl << endl;

	int nx = 1;
	int ny = 1;

	libMesh::Mesh mesh2(_mesh_init_ptr->comm());

	// Build a 2D square (1,2)x(1,2) mesh with nx subdivisions along x axis and ny subdivisions along y axis
	libMesh::MeshTools::Generation::build_square(mesh2, nx, ny, 1., 2., 1., 2., libMesh::TRI3);

	Point<2> p4({0,0});
	Point<2> p5({1,0});	
	Point<2> p6({0.5,0.5});
	Point<2> p7({0,1});

	libMesh::Elem* tri_elem1 = mesh2.elem_ptr(0);

	clog << "Creo la mappa per il primo triangolo" << endl;
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
	clog << "Mappa creata" << endl;

	Point<2> mapped_p4 = map2.Evaluate(p4);
	err = abs(mapped_p4[0] - 1 + mapped_p4[1] - 1);
	EXPECT_LT(err, 1E-4) << "Point (0,0) expected to be mapped in (1,1), is mapped in ("
									+ to_string(mapped_p4[0])
									+ ","
									+ to_string(mapped_p4[1])
									+ ")";

	Point<2> mapped_p5 = map2.Evaluate(p5);
	err = abs(mapped_p5[0] - 2 + mapped_p5[1] - 1);
	EXPECT_LT(err, 1E-4) << "Point (1,0) expected to be mapped in (2,1), is mapped in ("
									+ to_string(mapped_p5[0])
									+ ","
									+ to_string(mapped_p5[1])
									+ ")";

	Point<2> mapped_p6 = map2.Evaluate(p6);
	err = abs(mapped_p6[0] - 2 + mapped_p6[1] - 1.5);
	EXPECT_LT(err, 1E-4) << "Point (0.5,0.5) expected to be mapped in (2,1.5), is mapped in ("
									+ to_string(mapped_p6[0])
									+ ","
									+ to_string(mapped_p6[1])
									+ ")";

	Point<2> mapped_p7 = map2.Evaluate(p7);
	err = abs(mapped_p7[0] - 2 + mapped_p7[1] - 2);
	EXPECT_LT(err, 1E-4) << "Point (0,1) expected to be mapped in (2,2), is mapped in ("
									+ to_string(mapped_p7[0])
									+ ","
									+ to_string(mapped_p7[1])
									+ ")";
	
//	TODO: decomment
//	libMesh::Elem* tri_elem2 = mesh2.elem_ptr(1);

//	clog << "Creo la mappa per il secondo triangolo" << endl;
//	TriMap map3;

//	auto geometry_nodes3 = tri_elem2->get_nodes();
//	auto length3 = tri_elem2->n_nodes();
//	NodesVector<2> result3;
//	for (size_t i(0); i < length3; ++i)
//	{
//		auto current_ptr = geometry_nodes3[i];
//		Point<2> p;
//		for (size_t j(0); j < 2; ++j)
//			p[j] = (*current_ptr)(j);

//		result3.push_back(p);
//	}

//	map3.Init(result3);
//	cout << "Mappa creata" << endl;

//	Point<2> mapped_p8 = map3.Evaluate(p4);
//	p4.Print(false);
//	cout << " viene mappato in ";
//	mapped_p8.Print();

//	Point<2> mapped_p9 = map3.Evaluate(p5);
//	p5.Print(false);
//	cout << " viene mappato in ";
//	mapped_p9.Print();

//	Point<2> mapped_p10 = map3.Evaluate(p6);
//	p6.Print(false);
//	cout << " viene mappato in ";
//	mapped_p10.Print();

//	Point<2> mapped_p11 = map3.Evaluate(p7);
//	p7.Print(false);
//	cout << " viene mappato in ";
//	mapped_p11.Print();

	//TODO: check the inverse
	//TODO: check singular maps between reference elements

	clog << "MapsTest ended" << endl << endl;
};

TEST_F(LoadTest, GeneralIntegration)
{
	clog << endl << "Starting GeneralIntegration" << endl;

	int n = 1;
	libMesh::Mesh mesh1(_mesh_init_ptr->comm());

	// Build a 1D mesh with n elements from x=1 to x=2
	libMesh::MeshTools::Generation::build_line(mesh1, n, 1., 2., LibmeshIntervalType);

	auto iter = mesh1.elements_begin();
	auto& element = *iter;
	libMesh::Edge2* libmesh_interval = dynamic_cast<libMesh::Edge2*> (element);
	ASSERT_NE(libmesh_interval, nullptr) << "LibmeshIntervalType not recognized casting to libMesh::Edge2";

	clog << "Costruisco FElement" << endl;
	FElement<1, LegendreType, LibmeshIntervalClass> interval(iter);

	clog << "Inizializzo interval" << endl;

	interval.Init();

	//TODO: add the check on the quadrature order, if exactly integrated verify if the tolerance should be decreased
	//Computing integral of x^2 between 1 and 2	
	double val = interval.Integrate([](const Point<1>& x){return x[0]*x[0];});
	double solex = 7; solex /= 3; 
	auto err = abs(val - solex);	
	EXPECT_LT(err, 1E-4) << "Wrong integral, the error wrt the exact solution is" + to_string(err);

	clog << "GeneralIntegration ended" << endl << endl;
};


//TEST(LoadTest, LibmeshVersionCompatibility)
//{
//	//TODO
//};


TEST_F(LoadTest, LegendreOrthonormality)
{
	clog << endl << "Starting LegendreOrthonormality" << endl;

#ifndef SINGLETON_ENABLED
	StdFIperCube<1, LegendreType> sfic;
#else //SINGLETON_ENABLED
	shared_ptr<StdFIperCube<1, LegendreType> > sfic_ptr = StdFIperCube<1, LegendreType>::Instance();
	StdFIperCube<1, LegendreType>& sfic(*sfic_ptr);
#endif //SINGLETON_ENABLED
	size_t order = sfic.QuadratureOrder();

	clog << "Checking the orthogonality on the standard interval" << endl;
	for (size_t k(0); k < 100; ++k)
	{
		size_t i = 0;
		for (; i + k <= order; ++i)
		{
			double I = sfic.L2prod	(
												[&](const Point<1>& p){ return sfic.EvaluateBasisFunction(i, p);},
												[&](const Point<1>& p){ return sfic.EvaluateBasisFunction(k, p);}
											);

			if (i != k)
				EXPECT_LT(I, 1E-10) << "Legendre functions #"
											+ to_string(i)
											+ " and #"
											+ to_string(k)
											+ " are not orthogonal: L2 product = "
											+ to_string(I);
		}

		//TODO: add checks on these values
//		for (; i + k <= 100; ++i)
//		{
//			double I = sfic.L2prod	(
//												[&](const Point<1>& p){ return sfic.EvaluateBasisFunction(i, p);},
//												[&](const Point<1>& p){ return sfic.EvaluateBasisFunction(k, p);}
//											);

//			if (i != k)
//				clog << "Legendre functions #"
//						+ to_string(i)
//						+ " and #"
//						+ to_string(k)
//						+ " have L2 product = "
//						+ to_string(I);
//		}
	}

	cout << endl << endl << "Let's check the normality on the standard interval :" << endl;

	size_t i = 0;

	//order is a size_t, so when divided by two it's truncated downward to the last value
	//for which the exactness of the quadrature rule for the L2 norm of the function still holds
	for (; i < order/2; ++i)
	{
		double I = sfic.L2prod	(
											[&](const Point<1>& p){ return sfic.EvaluateBasisFunction(i, p);},
											[&](const Point<1>& p){ return sfic.EvaluateBasisFunction(i, p);}
										);
		I *= (static_cast<double>(i) + 0.5);
		auto err = abs(I - 1);
		EXPECT_LT(err, 1E-10) << "Legendre function #"
									+ to_string(i)
									+ " is not normal: Integral = "
									+ to_string(I);
	}
	//TODO: add checks on these values
//	for (; i < 100; ++i)
//	{
//		double I = sfic.L2prod	(
//											[&](const Point<1>& p){ return sfic.EvaluateBasisFunction(i, p);},
//											[&](const Point<1>& p){ return sfic.EvaluateBasisFunction(i, p);}
//										);
//		I *= (static_cast<double>(i) + 0.5);
//		clog << "Legendre function #" + to_string(i) + " has I = " + to_string(I) << endl;
//	}


	clog << "LegendreOrthonormality ended" << endl << endl;
};

TEST_F(LoadTest, ProjectionTest)
{
	clog << endl << "Starting ProjectionTest" << endl;

	//Do not change this function! It's needed to be x^2 for the last part of the test
	auto& f_factory(FunctionsFactory<1>::Instance());
	std::unique_ptr<Functor<1>> smart_ptr = f_factory.create("x_squared");
	BinaryTree::FunctionPtr<1> f_ptr (smart_ptr.release());

	libMesh::Mesh mesh(_mesh_init_ptr->comm());
	libMesh::MeshRefinement mesh_refiner(mesh);
	libMesh::MeshTools::Generation::build_line(mesh, 1, 0, 1, LibmeshIntervalType);

	BinarityMap::MakeBinary<1> (mesh_refiner, f_ptr);
	ASSERT_TRUE(BinarityMap::CheckBinarity(mesh)) << "After MakeBinary mesh is not binary!";

	//TODO: add checks on these two lines
	auto el = dynamic_cast<BinaryTreeElement<1, LegendreType, LibmeshIntervalClass>*> (mesh.elem_ptr(0));
	const auto& f_el(el->GetFElement());

	size_t order = f_el.QuadratureOrder();

	clog << "Checking the basis orthogonality on the (0, 1) interval:" << endl;
	for (size_t k(0); k < 100; ++k)
	{
		size_t i = 0;
		for (; i + k <= order; ++i)
		{
			double I = f_el.L2prod	(
												[&](const Point<1>& p){ return f_el.EvaluateBasisFunction(i, p);},
												[&](const Point<1>& p){ return f_el.EvaluateBasisFunction(k, p);}
											);

			if (i != k)
				EXPECT_LT(I, 1E-10) << "Legendre functions #"
											+ to_string(i)
											+ " and #"
											+ to_string(k)
											+ " are not orthogonal: L2 product = "
											+ to_string(I);
		}
	}

	clog << "Checking the basis normality on the (0, 1) interval :" << endl;
	for (size_t i(0); i < order/2; ++i)
	{
		double I = f_el.L2prod	(
											[&](const Point<1>& p){ return f_el.EvaluateBasisFunction(i, p);},
											[&](const Point<1>& p){ return f_el.EvaluateBasisFunction(i, p);}
										);
		I *= (static_cast<double>(i) + 0.5);
		auto err = abs(I - 1);
		EXPECT_LT(err, 1E-10) << "Legendre function #"
									+ to_string(i)
									+ " is not normal: Integral = "
									+ to_string(I);
	}

	//TODO: add checks on the coefficient values

	EXPECT_EQ(el->PLevel(), 0) << "p level not modified yet, expected zero but it values " + to_string(el->PLevel());
	double error = el->ProjectionError();
//	clog << "Errore di interpolazione su P" << el->PLevel() << " : " << error << endl;

	size_t p = 1;
	// quadrature exact till order - 2 since i'm doing the L2 product with x^2
	for (; p <= order - 2; ++p)
	{
		el->PLevel(p);
		EXPECT_EQ(el->PLevel(), p) << "Element p level: expected " + to_string(p) + " is " + to_string(el->PLevel());
		error = el->ProjectionError();
		if (p >= 2)
		{
			EXPECT_LT(error, 1E-10) << "x^2 wrongly projected on P" + to_string(p) + " : interpolation error = " + to_string(error);
//			clog << "Errore di interpolazione su P" << el->PLevel() << " : " << error << endl;
		}
	}

	//TODO: add checks on these values
//	for (; p < 40; ++p)
//	{
//		el->PLevel(p);
//		EXPECT_EQ(el->PLevel(), p) << "Element p level: expected " + to_string(p) + " is " + to_string(el->PLevel());
//		clog << "P level : " << el->PLevel() << endl;
//		error = el->ProjectionError();
//		clog << "Errore di interpolazione su P" << el->PLevel() << " : " << error << endl;
//	}

	clog << "ProjectionTest ended" << endl << endl;
};

TEST_F(LoadTest, LibmeshRefinement)
{
	clog << endl << "Starting LibmeshRefinement" << endl;

	size_t cont = 0;
	int n = 1;
	libMesh::Mesh mesh(_mesh_init_ptr->comm());

	// Build a 1D mesh with n elements from x=1 to x=2
	libMesh::MeshTools::Generation::build_line(mesh, n, 1., 2., libMesh::EDGE2);
	auto& element_ptr = *(mesh.elements_begin());

	ASSERT_NE(element_ptr, nullptr) << "Mesh element pointer is null";

	libMesh::Edge2* copy_element_ptr = dynamic_cast<libMesh::Edge2*>(element_ptr);
	//I want to call the assignment operator
	libMesh::Edge2 copy_element = *copy_element_ptr;
	copy_element_ptr = &copy_element;
	ASSERT_TRUE(copy_element_ptr->operator== (*element_ptr)) << "The two elements are different";

	auto el_nodes = element_ptr->get_nodes();
	auto copy_el_nodes = copy_element_ptr->get_nodes();
	EXPECT_EQ ( (*el_nodes)->norm(), (*copy_el_nodes)->norm() );
	
	EXPECT_EQ ( (*(el_nodes[0]))(0), (*(copy_el_nodes[0]))(0) );
	EXPECT_EQ ( (*(el_nodes[1]))(0), (*(copy_el_nodes[1]))(0) );

	//libmesh element has not copy constructor implement,
	//so the default one should copy the address of the nodes attribute in the copy
	EXPECT_EQ(el_nodes[0], copy_el_nodes[0]);

#ifdef ACTIVATE_BUG
	//insert_elem destroys the element pointed by element_ptr since it has the same id of its copy
	//the destruction deletes memory pointed by copy_el_nodes
	mesh.insert_elem(copy_element_ptr);
#endif //ACTIVATE_BUG

//	element_ptr = copy_element_ptr;

	EXPECT_TRUE (copy_element_ptr->operator== (**(mesh.elements_begin())) ) << "After the insert the element is different from the starting one";
	el_nodes = element_ptr->get_nodes();
	copy_el_nodes = mesh.elem_ptr(0)->get_nodes();	
	EXPECT_EQ ( (*(el_nodes[0]))(0), (*(copy_el_nodes[0]))(0) );
	EXPECT_EQ ( (*(el_nodes[1]))(0), (*(copy_el_nodes[1]))(0) );

	mesh.prepare_for_use(/*skip_renumber =*/ false);

	element_ptr->set_refinement_flag(libMesh::Elem::REFINE);

	EXPECT_EQ (mesh.n_elem(), 1);
	EXPECT_EQ ((*mesh.elements_begin())->refinement_flag(), libMesh::Elem::REFINE);
	EXPECT_EQ (element_ptr, *(mesh.elements_begin()));

	clog << "I start one refinement iteration on the first mesh element" << endl;
	libMesh::MeshRefinement refiner(mesh);
	element_ptr->refine(refiner);

	int count = mesh.n_elem();

	for (auto iter = mesh.elements_begin(); iter != mesh.elements_end(); ++iter)
		if (!(*iter)->active())
			--count;

	EXPECT_EQ(mesh.n_elem(), 3) << "Started from a mesh with "
											+ to_string(n)
											+ " elements; after refining one element there are "
											+ to_string(mesh.n_elem())
											+ " elements";

	EXPECT_EQ(count, 2) << "Started from a mesh with "
								+ to_string(n)
								+ " elements; after refining one element there are"
								+ to_string(cont) + " ACTIVE elements";

	clog << "LibmeshRefinement ended" << endl << endl;
};

TEST_F(LoadTest, ManualBinaryRefinement)
{
	clog << endl << "Starting ManualBinaryRefinement" << endl;

	int n = 1;
	libMesh::Mesh mesh(_mesh_init_ptr->comm());

	// Build a 1D mesh with n elements from x=1 to x=2
	libMesh::MeshTools::Generation::build_line(mesh, n, 1., 2., libMesh::EDGE2);
	libMesh::MeshRefinement refiner(mesh);

	auto& f_factory(FunctionsFactory<1>::Instance());
	std::unique_ptr<Functor<1>> smart_ptr = f_factory.create("x_squared");
	BinaryTree::FunctionPtr<1> f_ptr (smart_ptr.release());

#define MAKE_OPTION
#ifdef MAKE_OPTION
	BinarityMap::MakeBinary<1>(refiner, f_ptr);
	Interval* I = dynamic_cast<Interval*>(mesh.elem(0));
	ASSERT_NE (I, nullptr) << "MakeBinary has not worked properly!";

#else //MAKE_OPTION

	//construction ed initialization of a Binary object of type Interval
	auto iter = mesh.elements_begin();
	auto ptr = dynamic_cast<libMesh::Edge2*>(*iter);
	auto smart_ptr = unique_ptr<libMesh::Edge2>(ptr);
	Interval* I = new Interval(move(smart_ptr), f_ptr, refiner);
	I->Init();
	*iter = I;

	//to check if I have a LibmeshGeometry method as expected
	EXPECT_TRUE (I->active());

#endif //MAKE_OPTION

	clog << "I directly call the Interval refine method" << endl;
	I->Bisect();
	EXPECT_TRUE(BinarityMap::CheckBinarity(mesh));
	EXPECT_NE(I->Left(), nullptr) << "After refinement the interval has not left child";
	EXPECT_NE(I->Right(), nullptr) << "After refinement the interval has not right child";
	
	auto hansel = dynamic_cast<Interval*>(I->Left());
	auto gretel = dynamic_cast<Interval*>(I->Right());
	EXPECT_TRUE(hansel->active());
	EXPECT_TRUE(gretel->active());	
	EXPECT_FALSE(I->active());	
//	hansel->Activate();
//	gretel->Activate();
//	I->Deactivate();

	mesh.prepare_for_use(/*skip_renumber =*/ false);

	size_t cont = 0;

	for (auto iter = mesh.active_elements_begin(); iter != mesh.active_elements_end(); ++iter)
		++cont;

	EXPECT_EQ(mesh.n_elem(), 3) << "Started from a mesh with "
											+ to_string(n)
											+ " elements; after refining one time one element there are "
											+ to_string(mesh.n_elem())
											+ " elements";

	EXPECT_EQ(cont, 2) << "Started from a mesh with "
								+ to_string(n)
								+ " elements; after one refining one time one element there are "
								+ to_string(cont) + " ACTIVE elements";

	clog << "ManualBinaryRefinement ended" << endl << endl;
};

//TODO, TOP PRIORITY
TEST_F(LoadTest, BinaryRefinement)
{
	clog << endl << "Starting BinaryRefinement" << endl;

	auto mesh_ptr = make_shared<libMesh::Mesh> (_mesh_init_ptr->comm());

	int n = 2;
	libMesh::MeshTools::Generation::build_line(*mesh_ptr, n, 0, 1, LibmeshIntervalType);
	LibmeshBinary::BinaryRefiner<1> binary_refiner;
	binary_refiner.Init("x_squared");
	binary_refiner.SetMesh(mesh_ptr);
	EXPECT_EQ(mesh_ptr, binary_refiner.GetMesh());

	size_t n_iter(10);
	clog << "Starting Binary Tree hp Refinement with " << n_iter << " degrees of freedom" << endl;
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

	clog << "BinaryRefinement ended" << endl << endl;
};

//TODO
TEST_F(LoadTest, IOTest)
{
	clog << endl << "Starting libMesh file IO test" << endl;

	libMesh::Mesh mesh(_mesh_init_ptr->comm());

	// Build a 2D square (1,2)x(1,2) mesh with nx subdivisions along x axis and ny subdivisions along y axis
	int nx = 1, ny = 1;
	libMesh::MeshTools::Generation::build_square(mesh, nx, ny, 1., 2., 1., 2., libMesh::TRI3);
	mesh.write("./mesh2d.msh");

	clog << "libMesh file IO test ended" << endl << endl;
};

//TODO: perche' il primo polinomio non integrato esattamente ha norma nulla?
TEST_F(LoadTest, FirstNotExactElement)
{
	clog << endl << "Starting FirstNotExactElement" << endl;

	int n = 1;

	libMesh::Mesh mesh1(_mesh_init_ptr->comm());
	libMesh::MeshTools::Generation::build_line(mesh1, n, 0., 1., LibmeshIntervalType);

	auto iter = mesh1.elements_begin();
	FElement<1, LegendreType, LibmeshIntervalClass> f_el(iter);
	
	f_el.Init();

	size_t i = f_el.QuadratureOrder();
	i /= 2;
	i++;

	clog << "Let's check the normality of the element #" << i << " of the basis :" << endl;

	double I = f_el.L2prod	(
										[&](const Point<1>& p){ return f_el.EvaluateBasisFunction(i, p);},
										[&](const Point<1>& p){ return f_el.EvaluateBasisFunction(i, p);}
									);
	I *= i + 0.5;
	//TODO: do something on this value
	clog << "Integral: " << I << endl;

	clog << "FirstNotExactElement ended" << endl << endl;
};

TEST_F(LoadTest, MuParser)
{
	clog << endl << "Starting MuParser" << endl;

	//testing the getpot initialization
	auto& one_f_factory(FunctionsFactory<1>::Instance());
	//HERE I HAVE A SEGFAULT!!!
//	std::unique_ptr<Functor<1>> par_fun_one_d_ptr = std::move(one_f_factory.create("mu_parser_expr"));

//	std::array<double, 1> val;
//	val[0] = 2;
//	double result = (*par_fun_one_d_ptr)(val);
//	//i'm supposing the expression is x^2, the default one
//	double error = abs(result - 4);
//	EXPECT_LT(error, 1E-4)	<< "MuParser functor 1D evaluation failed:" << endl
//									<< "expected: 4" << endl
//									<< "result:" << result << endl;

//	auto& two_f_factory(FunctionsFactory<2>::Instance());
//	std::unique_ptr<Functor<2>> par_fun_two_d_ptr = std::move(two_f_factory.create("mu_parser_expr"));

//	std::array<double, 2> vals;
//	vals[0] = 2;
//	vals[1] = 3;
//	result = (*par_fun_two_d_ptr)(vals);
//	//I'm supposing the expression is x*y, the default one
//	error = abs(result - 6);
//	EXPECT_LT(error, 1E-4)	<< "MuParser functor 2D evaluation failed:" << endl
//									<< "expected: 6" << endl
//									<< "result:" << result << endl;

	clog << "MuParser ended" << endl << endl;
};

//void test2(int argc, char ** argv)
//{
//	//to silence warning by the compiler 
//	(void)argc;
//	(void)argv;

//	cout << endl << "Starting Polinomial test" << endl;

//	Point<1> p_test({1});
//	Point<1> p_copy(p_test);
//	Point<1> p1 = p_copy;
//	array<double, 2> init = {2,3};
//	Point<2> p2(init);
//	Point<3> p3 = p1.Tensor(p2);
//	double scalar(2);
//	Point<3> p4 = p3 * scalar;
//	Point<3> p5 = p4 - p3;


//	Polinomial<1> pol1(1,-2);
//	Polinomial<1> pol2(2,4);
//	cout << "Polinomi di una variabile: " << endl;
//	cout << "poli1" << endl;
//	pol1.Print(false);
//	pol1.Print(true,"x");
//	cout << "poli2" << endl;
//	pol2.Print(false);
//	pol2.Print();

//	cout << endl << "Polinomio di 2 variabili poli3 = poli1 tensor poli2" << endl;
//	Polinomial<2> pol3 = pol1.tensor(pol2);
//	pol3.Print();

//	Polinomial<3> pol4 = pol3.tensor(pol2);
//	Polinomial<3> pol5 = pol1.tensor(pol3);

//	cout << endl << "Polinomi di 3 variabili:" << endl;
//	cout << "1D tensor 2D: poli3 tensor poli2" << endl;
//	pol4.Print();
// 	cout << "2D tensor 1D: poli1 tensor poli3" << endl;
//	pol5.Print();

//	Polinomial<4> pol6 = pol3.tensor(pol3);
//	cout << endl << "Polinomio di 4 variabili poli4 = poli3 tensor poli3" << endl;
//	pol6.Print();

//	cout << endl << "VALUTAZIONE DEI POLINOMI" << endl;
//	cout << endl << "valuto il polinomio" << endl;
//	pol4.Print();
//	cout << "nel punto " << endl;
//	p5.Print();
//	cout << "Risultato: " << pol4(p5) << endl;

//	Polinomial<1> one(5,0);
//	one[0] = 1;
//	Polinomial<1> x(1,0);
//	x[1] = 1;
//	Polinomial<3> z = one.tensor(one).tensor(x);
//	cout << endl << "valuto il polinomio p(x,y,z) = z" << endl;
//	z.Print();
//	cout << "nel punto " << endl;
//	p5.Print();
//	cout << "Risultato: " << z(p5) << endl;

//	cout << "Polinomial test ended" << endl;
//};

