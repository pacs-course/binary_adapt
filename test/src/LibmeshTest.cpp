#include "LibmeshConfiguration.h"

#include "LibMeshRefiner.h"
#include "LibMeshBinaryElements.h"

#include "libmesh/mesh_generation.h" //MeshTools

using namespace std;
using namespace Geometry;
using namespace FiniteElements;


TEST_F(LibmeshTest, LibmeshVersionCompatibility)
{
	//TODO
};

//TEST_F(BasicTest, SandiaQuadratureTest)
//{
//	clog << endl << "Starting PointTest" << endl;

//	size_t order = 2;

//	LibmeshBinary::LibmeshQuadratureRule<1, IntervalType> one_d_rule();
//	SandiaQuadrature::SandiaQuadratureRule<1> one_d_rule(order);
//	SandiaQuadrature::SandiaQuadratureRule<2> two_d_rule(order);
//	auto one_d_points = one_d_rule.GetPoints();
//	auto one_d_weights = one_d_rule.GetWeights();

//	auto two_d_points = two_d_rule.GetPoints();
//	auto two_d_weights = one_d_rule.GetWeights();

//	clog << endl << "PointTest ended" << endl;	
//};


TEST_F(LibmeshTest, GeneralIntegration)
{
	clog << endl << "Starting GeneralIntegration" << endl;

	int n = 1;
	libMesh::Mesh mesh1(_mesh_init_ptr->comm());

	clog << "Building a 1D mesh with " << n << " elements (1,2)" << endl;
	libMesh::MeshTools::Generation::build_line(mesh1, n, 1., 2., LibmeshIntervalType);

	auto iter = mesh1.elements_begin();
	auto& element = *iter;
	LibmeshBinary::LibmeshIntervalClass* libmesh_interval = dynamic_cast<LibmeshBinary::LibmeshIntervalClass*> (element);
	ASSERT_NE(libmesh_interval, nullptr) << "LibmeshIntervalType not recognized casting to LibmeshBinary::LibmeshIntervalClass*";

	clog << "Costruisco FElement" << endl;
	LibmeshBinary::FElement<1, LegendreType, LibmeshBinary::LibmeshIntervalClass> interval(iter);

	clog << "Initializing interval" << endl;

	interval.Init();

	size_t order = interval.QuadratureOrder();
	
	clog << "Computing integral of x^" << order << " between 1 and 2" << endl;	
	double val = interval.Integrate([&order](const Point<1>& x){return pow(x[0], order);});
	double solex = pow(2, order+1) - 1; solex /= order+1; 
	//I consider the relative error since solex is huge 
	auto err = abs((val - solex) / solex);

	string s = "Integrating x^"
				+ to_string(order)
				+ " in (1,2)" + "\n"
				+ "Exact solution: "
				+ to_string(solex) + "\n"
				+ "Computed solution: "
				+ to_string(val);
	clog << s << endl;
	EXPECT_LT(err, 1E-14) << s; 

	int nx = 1, ny = 1;

	clog << "Building a 2D mesh with " << 2 * nx * ny << " triangular elements in (1,2)x(1,2)" << endl;
	libMesh::Mesh mesh2(_mesh_init_ptr->comm());
	libMesh::MeshTools::Generation::build_square(mesh2, nx, ny, 1., 2., 1., 2., LibmeshTriangleType);


	auto iter2 = mesh2.elements_begin();
	auto& element2 = *iter2;

	auto el_nodes = element2->get_nodes();
	
	EXPECT_EQ ( (*(el_nodes[0]))(0), 1 );
	EXPECT_EQ ( (*(el_nodes[0]))(1), 1 );
	EXPECT_EQ ( (*(el_nodes[1]))(0), 2 );
	EXPECT_EQ ( (*(el_nodes[1]))(1), 1 );
	EXPECT_EQ ( (*(el_nodes[2]))(0), 2 );
	EXPECT_EQ ( (*(el_nodes[2]))(1), 2 );


	LibmeshBinary::LibmeshTriangleClass* libmesh_triangle = dynamic_cast<LibmeshBinary::LibmeshTriangleClass*> (element2);
	ASSERT_NE(libmesh_triangle, nullptr) << "LibmeshTriangleType not recognized casting to LibmeshBinary::LibmeshTriangleClass*";

	clog << "Costruisco FElement" << endl;
	LibmeshBinary::FElement<2, WarpedType, LibmeshBinary::LibmeshTriangleClass> triangle(iter2);

	clog << "Initializing triangle" << endl;

	triangle.Init();

	order = triangle.QuadratureOrder();
	
	clog << "Computing integral of x^" << order << " + y^" << order << " in  triangle co{(1,1),(2,1),(2,2)}" << endl;
	val = triangle.Integrate([&order](const Point<2>& x){return pow(x[0], order) + pow(x[1], order);});
	solex = (pow(2, order+1) - 1) / (order+1);
	//I consider the relative error since solex is huge 
	err = abs((val - solex) / solex);

	s	= "Integrating x^"
		+ to_string(order)
		+ " + y^"
		+ to_string(order)
		+ " in co{(1,1),(2,1),(2,2)}" + "\n"
		+ "Exact solution: "
		+ to_string(solex) + "\n"
		+ "Computed solution: "
			+ to_string(val);
	clog << s << endl;
	EXPECT_LT(err, 1E-14) << s; 

	clog << "GeneralIntegration ended" << endl << endl;
};


TEST_F(LibmeshTest, BinaryElementsConstruction)
{
	clog << endl << "Starting BinaryElementsConstruction" << endl;

	int n = 2;
	libMesh::Mesh mesh(_mesh_init_ptr->comm());

	libMesh::MeshRefinement mesh_refiner(mesh);

	clog << "Building a 1D mesh with " << n << " elements (0,1)" << endl;
	libMesh::MeshTools::Generation::build_line(mesh, n, 0., 1., LibmeshIntervalType);

	auto& f_factory(BinaryTree::FunctionsFactory<1>::Instance());
	unique_ptr<BinaryTree::Functor<1>> smart_ptr = f_factory.create("x_squared");
	BinaryTree::FunctionPtr<1> f_ptr (smart_ptr.release());

	clog << "Calling MakeBinary<1>" << endl;
	LibmeshBinary::BinarityMap::MakeBinary<1> (mesh_refiner, f_ptr);

	ASSERT_TRUE(LibmeshBinary::BinarityMap::CheckBinarity(mesh)) << "After MakeBinary mesh is not binary!";

	mesh.prepare_for_use(/*skip_renumber =*/ false);

	int nx = 2, ny = 2;
	libMesh::Mesh mesh2(_mesh_init_ptr->comm());

	libMesh::MeshRefinement mesh_refiner2(mesh2);

	clog << "Building a 2D mesh with " << 2 * nx * ny << " triangular elements in (0,1)x(0,1)" << endl;
	libMesh::MeshTools::Generation::build_square(mesh2, nx, ny, 0., 1., 0., 1., LibmeshTriangleType);

	auto& f_factory2(BinaryTree::FunctionsFactory<2>::Instance());
	unique_ptr<BinaryTree::Functor<2>> smart_ptr2 = f_factory2.create("x_squared_plus_y_squared");
	BinaryTree::FunctionPtr<2> f_ptr2 (smart_ptr2.release());


	clog << "Calling MakeBinary<2>" << endl;
	LibmeshBinary::BinarityMap::MakeBinary<2> (mesh_refiner2, f_ptr2);

	ASSERT_TRUE(LibmeshBinary::BinarityMap::CheckBinarity(mesh2)) << "After MakeBinary mesh is not binary!";

	mesh2.prepare_for_use(/*skip_renumber =*/ false);

////TODO: add other assertion

	clog << "BinaryElementsConstruction ended" << endl << endl;
};


TEST_F(LibmeshTest, IntervalProjectionTest)
{
	clog << endl << "Starting IntervalProjectionTest" << endl;

	//Do not change this function! It's needed to be x^2 for the last part of the test
	auto& f_factory(BinaryTree::FunctionsFactory<1>::Instance());
	unique_ptr<BinaryTree::Functor<1>> smart_ptr = f_factory.create("x_squared");
	BinaryTree::FunctionPtr<1> f_ptr (smart_ptr.release());

	libMesh::Mesh mesh(_mesh_init_ptr->comm());
	libMesh::MeshRefinement mesh_refiner(mesh);
	libMesh::MeshTools::Generation::build_line(mesh, 1, 0, 1, LibmeshIntervalType);

	LibmeshBinary::BinarityMap::MakeBinary<1> (mesh_refiner, f_ptr);
	ASSERT_TRUE(LibmeshBinary::BinarityMap::CheckBinarity(mesh)) << "After MakeBinary mesh is not binary!";

	auto el = dynamic_cast<LibmeshBinary::BinaryTreeElement<1, LegendreType, LibmeshBinary::LibmeshIntervalClass>*> (mesh.elem_ptr(0));
	ASSERT_NE(el, nullptr) << "Mesh element not recognized casting to LibmeshBinary::BinaryElement";

	auto& f_el(el->GetFElement());

	size_t order = f_el.QuadratureOrder();

	clog << "Checking the basis orthogonality on the (0, 1) interval:" << endl;
	for (size_t k(0); k < 100; ++k)
	{
		size_t i = 0;
		for (; i + k <= order; ++i)
		{
			double I = f_el.L2Prod	(
												[&](const Point<1>& p){ return f_el.EvaluateBasisFunction(i, p);},
												[&](const Point<1>& p){ return f_el.EvaluateBasisFunction(k, p);}
											);

			if (i != k)
			{
				string s1 = "Legendre functions #"
							+ to_string(i)
							+ " and #"
							+ to_string(k)
							+ " L2 product = "
							+ to_string(I);
				string s2 = "Legendre functions #"
							+ to_string(i)
							+ " and #"
							+ to_string(k)
							+ " are not orthogonal: L2 product = "
							+ to_string(I);
				clog << s1 << endl;
				EXPECT_LT(I, 1E-10) << s2;
			}
		}
	}

	clog << "Checking the basis normality on the (0, 1) interval :" << endl;
	for (size_t i(0); i < order/2; ++i)
	{
		double I = f_el.BasisNormSquared(i);

		// I divide the integral by the manually computed value of L2 norm for legendre functions on (0,1) interval
		// so I should obtain 1
		double inverse_std_norm_squared = static_cast<double>(i) + 0.5;
		I *= 2*inverse_std_norm_squared;

		auto err = abs(I - 1);

		string s1 = "Legendre function #"
					+ to_string(i)
					+ " norm = "
					+ to_string(I);
		string s2 = "Legendre function #"
						+ to_string(i)
						+ " is not normal: Integral = "
						+ to_string(I);
		clog << s1 << endl;
		EXPECT_LT(err, 1E-10) << s2;
	}

	clog << "Projecting x^2 on (0,1)" << endl;
	EXPECT_EQ(el->PLevel(), 0) << "p level not modified yet, expected zero but it values " + to_string(el->PLevel());

	// quadrature exact till order - 2 since i'm doing the L2 product with x^2
	for (size_t p = order - 2; p >= 2; --p)
	{
		el->PLevel(p);
		EXPECT_EQ(el->PLevel(), p) << "Element p level: expected " + to_string(p) + " is " + to_string(el->PLevel());
		double error = el->ProjectionError();
		
		// p >= 2, x^2 expected to be exactly integrated
		EXPECT_LT(error, 1E-10) << "x^2 wrongly projected on P" + to_string(p) + " : interpolation error = " + to_string(error);
		clog << "Errore di interpolazione su P" << el->PLevel() << " : " << error << endl;
	}

	//TODO: add checks on these values
	el->PLevel(1);
	EXPECT_EQ(el->PLevel(), 1) << "Element p level: expected 1 is " + to_string(el->PLevel());
	double error = el->ProjectionError();
	clog << "Errore di interpolazione su P" << el->PLevel() << " : " << error << endl;

	el->PLevel(0);
	EXPECT_EQ(el->PLevel(), 0) << "Element p level: expected 0 is " + to_string(el->PLevel());
	error = el->ProjectionError();
	clog << "Errore di interpolazione su P" << el->PLevel() << " : " << error << endl;

	clog << "IntervalProjectionTest ended" << endl << endl;
}

#ifdef TRY_IT

TEST_F(LibmeshTest, SquareProjectionTest)
{
	clog << endl << "Starting SquareProjectionTest" << endl;

	int nx = 1, ny = 1;
	libMesh::Mesh mesh(_mesh_init_ptr->comm());

	libMesh::MeshRefinement mesh_refiner(mesh);

	clog << "Building a 2D mesh with " << nx * ny << " squared elements in (0,1)x(0,1)" << endl;
	libMesh::MeshTools::Generation::build_square(mesh, nx, ny, 0., 1., 0., 1., LibmeshSquareType);

	auto& f_factory(BinaryTree::FunctionsFactory<2>::Instance());
	unique_ptr<BinaryTree::Functor<2>> smart_ptr = f_factory.create("x_squared_plus_y_squared");

	auto libmesh_ptr = mesh.elem_ptr(0);

	auto el = dynamic_cast<LibmeshBinary::LibmeshSquareClass*>(libmesh_ptr);
	ASSERT_NE(el, nullptr) << "Mesh element not recognized casting to LibmeshBinary::LibmeshSquareClass*";
	
	unique_ptr<LibmeshBinary::LibmeshSquareClass> smart_el (el);
	LibmeshBinary::FElement<2, LegendreType, LibmeshBinary::LibmeshSquareClass> f_el(move(smart_el));
	f_el.Init();

	auto order = f_el.QuadratureOrder();
	auto exactness_size = f_el.BasisSize(order);

	size_t iteration_end = (exactness_size < 100) ? exactness_size : 100;
	clog << "Checking the basis orthogonality on the (0,1)x(0,1) square:" << endl;
	for (size_t k(0); k < iteration_end; ++k)
		for (size_t i = 0; i + k <= iteration_end; ++i)
		{
			double I = f_el.L2Prod	(
												[&](const Point<2>& p){ return f_el.EvaluateBasisFunction(i, p);},
												[&](const Point<2>& p){ return f_el.EvaluateBasisFunction(k, p);}
											);

			if (i != k)
			{
				string s1 = "Legendre functions #"
							+ to_string(i)
							+ " and #"
							+ to_string(k)
							+ " L2 product = "
							+ to_string(I);
				string s2 = "Legendre functions #"
							+ to_string(i)
							+ " and #"
							+ to_string(k)
							+ " are not orthogonal: L2 product = "
							+ to_string(I);
				clog << s1 << endl;
				EXPECT_LT(I, 1E-10) << s2;
			}
		}


	clog << "Checking the basis normality on the (0,1)x(0,1) square :" << endl;
	size_t normality_size = f_el2.BasisSize(order/2);
	iteration_end = (normality_size < 100) ? normality_size : 100;

	size_t k1 = 0, k2 = 0;
	size_t deg = 0;

	for (size_t i(0); i < iteration_end; ++i)
	{
		double I = f_el.L2Prod	(
											[&](const Point<2>& p){ return f_el.EvaluateBasisFunction(i, p);},
											[&](const Point<2>& p){ return f_el.EvaluateBasisFunction(i, p);}
										);

		double K1 = (static_cast<double>(k1) + 0.5);
		double K2 = (static_cast<double>(k2) + 0.5);
		I *= K1*K2;

		auto err = abs(I - 1);

		string s1 = "Legendre function #"
					+ to_string(i)
					+ " norm = "
					+ to_string(I);
		string s2 = "Legendre function #"
						+ to_string(i)
						+ " is not normal: Integral = "
						+ to_string(I);
		clog << s1 << endl;
		EXPECT_LT(err, 1E-10) << s2;

		(k2 == deg) ? (k2 = 0, k1 = ++deg) : (--k1, ++k2);
	}
	//TODO: add checks on values not exactly integrated

	libmesh_ptr = f_el.ReleaseGeometry();
	clog << "SquareProjectionTest ended" << endl << endl;
}
#endif //TRY_IT

TEST_F(LibmeshTest, TriangleOrthogonality)
{
	clog << endl << "Starting TriangleOrthogonality" << endl;
	int nx = 1, ny = 1;
	libMesh::Mesh mesh(_mesh_init_ptr->comm());

	libMesh::MeshRefinement mesh_refiner(mesh);

	clog << "Building a 2D mesh with " << 2 * nx * ny << " triangular elements in (0,1)x(0,1)" << endl;
	libMesh::MeshTools::Generation::build_square(mesh, nx, ny, 0., 1., 0., 1., LibmeshTriangleType);

	auto libmesh_ptr = *(mesh.elements_begin());

	auto el = dynamic_cast<LibmeshBinary::LibmeshTriangleClass*>(libmesh_ptr);
	ASSERT_NE(el, nullptr) << "Mesh element not recognized casting to LibmeshBinary::LibmeshTriangleClass*";	
	
	unique_ptr<LibmeshBinary::LibmeshTriangleClass> smart_el (el);
	LibmeshBinary::FElement<2, WarpedType, LibmeshBinary::LibmeshTriangleClass> f_el(move(smart_el));
	f_el.Init();

	auto order = f_el.QuadratureOrder();
	auto exactness_size = f_el.BasisSize(order);

	size_t iteration_end = (exactness_size < 50) ? exactness_size : 50;
	clog << "Checking the basis orthogonality on the triangle co{(0,0),(1,0),(0,1)}:" << endl;

	for (size_t k(0); k < iteration_end; ++k)
		for (size_t i = 0; i + k <= iteration_end; ++i)
		{
			double I = f_el.L2Prod	(
												[&](const Point<2>& p){ return f_el.EvaluateBasisFunction(i, p);},
												[&](const Point<2>& p){ return f_el.EvaluateBasisFunction(k, p);}
											);

			if (i != k)
			{
				string s1 = "Warped functions #"
							+ to_string(i)
							+ " and #"
							+ to_string(k)
							+ " L2 product = "
							+ to_string(I);
				string s2 = "Warped functions #"
							+ to_string(i)
							+ " and #"
							+ to_string(k)
							+ " are not orthogonal: L2 product = "
							+ to_string(I);
				clog << s1 << endl;
				EXPECT_LT(I, 1E-10) << s2;
			}
		}
	//TODO: add checks on values not exactly integrated

	//to avoid segfault when libMesh::Mesh destructor is called
	libmesh_ptr = f_el.ReleaseGeometry();
	clog << "TriangleOrthogonality ended" << endl << endl;
};

TEST_F(LibmeshTest, TriangleProjectionTest)
{
	clog << endl << "Starting TriangleProjectionTest" << endl;
	int nx = 1, ny = 1;
	libMesh::Mesh mesh(_mesh_init_ptr->comm());

	libMesh::MeshRefinement mesh_refiner(mesh);

	clog << "Building a 2D mesh with " << 2 * nx * ny << " triangular elements in (0,1)x(0,1)" << endl;
	libMesh::MeshTools::Generation::build_square(mesh, nx, ny, 0., 1., 0., 1., LibmeshTriangleType);

	auto& f_factory(BinaryTree::FunctionsFactory<2>::Instance());
	unique_ptr<BinaryTree::Functor<2>> smart_ptr = f_factory.create("x_squared_plus_y_squared");
	BinaryTree::FunctionPtr<2> f_ptr(smart_ptr.release());

	LibmeshBinary::BinarityMap::MakeBinary<2> (mesh_refiner, f_ptr);
	ASSERT_TRUE(LibmeshBinary::BinarityMap::CheckBinarity(mesh)) << "After MakeBinary mesh is not binary!";

	auto el = dynamic_cast<LibmeshBinary::BinaryTreeElement<2, WarpedType, LibmeshBinary::LibmeshTriangleClass>*> (*(mesh.elements_begin()));
	ASSERT_NE(el, nullptr) << "Mesh element not recognized casting to LibmeshBinary::BinaryElement";

	clog << "Projecting x^2 + y^2 on the triangle co{(0,0),(1,0),(0,1)}" << endl;
	EXPECT_EQ(el->PLevel(), 0) << "p level not modified yet, expected zero but it values " + to_string(el->PLevel());

	auto order = (el->GetFElement()).QuadratureOrder();

	// quadrature exact till order - 2 since i'm doing the L2 product with x^2 + y^2
	for (size_t p = order - 2; p >= 2; --p)
	{
		el->PLevel(p);
		EXPECT_EQ(el->PLevel(), p) << "Element p level: expected " + to_string(p) + " is " + to_string(el->PLevel());
		double error = el->ProjectionError();
		
		//p >= 2, so x^2 + y^2 expected to be exactly integrated
		EXPECT_LT(error, 1E-10) << "x^2 wrongly projected on P" + to_string(p) + " : interpolation error = " + to_string(error);
		clog << "Errore di interpolazione su P" << el->PLevel() << " : " << error << endl;
	}

		el->PLevel(1);
		EXPECT_EQ(el->PLevel(), 1) << "Element p level: expected 1 is " + to_string(el->PLevel());
		double error = el->ProjectionError();
		clog << "Errore di interpolazione su P" << el->PLevel() << " : " << error << endl;

		el->PLevel(0);
		EXPECT_EQ(el->PLevel(), 0) << "Element p level: expected 0 is " + to_string(el->PLevel());
		error = el->ProjectionError();
		clog << "Errore di interpolazione su P" << el->PLevel() << " : " << error << endl;

//	//TODO: add checks on values not exactly integrated

	clog << "TriangleProjectionTest ended" << endl << endl;
};

TEST_F(LibmeshTest, LibmeshRefinement)
{
	clog << endl << "Starting LibmeshRefinement" << endl;

	size_t cont = 0;
	int n = 1;
	libMesh::Mesh mesh(_mesh_init_ptr->comm());

	// Build a 1D mesh with n elements from x=1 to x=2
	libMesh::MeshTools::Generation::build_line(mesh, n, 1., 2., LibmeshIntervalType);
	auto& element_ptr = *(mesh.elements_begin());

	ASSERT_NE(element_ptr, nullptr) << "Mesh element pointer is null";

	LibmeshBinary::LibmeshIntervalClass* copy_element_ptr = dynamic_cast<LibmeshBinary::LibmeshIntervalClass*>(element_ptr);
	ASSERT_NE(copy_element_ptr, nullptr) << "Element type not recognized casting to LibmeshBinary::LibmeshIntervalClass*";

	//I want to call the assignment operator
	LibmeshBinary::LibmeshIntervalClass copy_element = *copy_element_ptr;
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

TEST_F(LibmeshTest, TriangleBisection)
{
	clog << endl << "Starting TriangleBisection" << endl;

	int nx = 1, ny = 1;
	libMesh::Mesh mesh(_mesh_init_ptr->comm());

	libMesh::MeshRefinement mesh_refiner(mesh);

	clog << "Building a 2D mesh with " << 2 * nx * ny << " triangular elements in (0,1)x(0,1)" << endl;
	libMesh::MeshTools::Generation::build_square(mesh, nx, ny, 0., 1., 0., 1., LibmeshTriangleType);

	auto& f_factory(BinaryTree::FunctionsFactory<2>::Instance());
	unique_ptr<BinaryTree::Functor<2>> smart_ptr = f_factory.create("x_squared_plus_y_squared");
	BinaryTree::FunctionPtr<2> f_ptr(smart_ptr.release());

	LibmeshBinary::BinarityMap::MakeBinary<2> (mesh_refiner, f_ptr);
	ASSERT_TRUE(LibmeshBinary::BinarityMap::CheckBinarity(mesh)) << "After MakeBinary mesh is not binary!";

	LibmeshBinary::Triangle* el = nullptr;
	LibmeshBinary::Triangle* hansel = nullptr;
	LibmeshBinary::Triangle* gretel = nullptr;
	vector<Point<2>> nodes;

	auto iter = mesh.elements_begin();
	auto first_el_ptr = *iter;
	++iter;
	auto second_el_ptr = *iter;

	el = dynamic_cast<LibmeshBinary::Triangle*> (second_el_ptr);
	ASSERT_NE(el, nullptr) << "Mesh element not recognized casting to LibmeshBinary::BinaryElement";

	clog << "I bisect the second element" << endl;
	el->Bisect();
	clog << "Bisected element has " << el->n_children() << " children" << endl;
	EXPECT_EQ(el->n_children(), 2) << "The expected triangular binary element has " << el->n_children() << " children instead of 2" << endl;
	nodes = el->Nodes();
	clog << "Initial element nodes:" << endl;
	for (auto n : nodes)
		clog << n;
	clog << endl;

	EXPECT_LT(abs(nodes[0][0]), 1E-14);
	EXPECT_LT(abs(nodes[0][1]), 1E-14);
	EXPECT_LT(abs(nodes[1][0]) - 1, 1E-14);
	EXPECT_LT(abs(nodes[1][1]) - 1, 1E-14);
	EXPECT_LT(abs(nodes[2][0]), 1E-14);
	EXPECT_LT(abs(nodes[2][1]) - 1, 1E-14);

	hansel = dynamic_cast<LibmeshBinary::Triangle*>(el->Left());
	gretel = dynamic_cast<LibmeshBinary::Triangle*>(el->Right());
	ASSERT_NE(hansel, nullptr) << "Left child of bisected element is a nullptr";
	ASSERT_NE(gretel, nullptr) << "Right child of bisected element is a nullptr";

	nodes = hansel->Nodes();
	clog << "Left child nodes:" << endl;
	for (auto n : nodes)
		clog << n;
	clog << endl;

	EXPECT_LT(abs(nodes[0][0]), 1E-14);
	EXPECT_LT(abs(nodes[0][1]), 1E-14);
	EXPECT_LT(abs(nodes[1][0]) - 0.5, 1E-14);
	EXPECT_LT(abs(nodes[1][1]) - 0.5, 1E-14);
	EXPECT_LT(abs(nodes[2][0]), 1E-14);
	EXPECT_LT(abs(nodes[2][1]) - 1, 1E-14);

	nodes = gretel->Nodes();
	clog << "Right child nodes:" << endl;
	for (auto n : nodes)
		clog << n;
	clog << endl;

	EXPECT_LT(abs(nodes[0][0]) - 0.5, 1E-14);
	EXPECT_LT(abs(nodes[0][1]) - 0.5, 1E-14);
	EXPECT_LT(abs(nodes[1][0]) - 1, 1E-14);
	EXPECT_LT(abs(nodes[1][1]) - 1, 1E-14);
	EXPECT_LT(abs(nodes[2][0]), 1E-14);
	EXPECT_LT(abs(nodes[2][1]) - 1, 1E-14);

	el = dynamic_cast<LibmeshBinary::Triangle*> (first_el_ptr);
	ASSERT_NE(el, nullptr) << "Mesh element not recognized casting to LibmeshBinary::BinaryElement";

	clog << "I bisect the first element" << endl;
	el->Bisect();
	clog << "Bisected element has " << el->n_children() << " children" << endl;
	EXPECT_EQ(el->n_children(), 2) << "The expected triangular binary element has " << el->n_children() << " children instead of 2" << endl;
	nodes = el->Nodes();
	clog << "Initial element nodes:" << endl;
	for (auto n : nodes)
		clog << n;
	clog << endl;

	EXPECT_LT(abs(nodes[0][0]), 1E-14);
	EXPECT_LT(abs(nodes[0][1]), 1E-14);
	EXPECT_LT(abs(nodes[1][0]) - 1, 1E-14);
	EXPECT_LT(abs(nodes[1][1]), 1E-14);
	EXPECT_LT(abs(nodes[2][0]) - 1, 1E-14);
	EXPECT_LT(abs(nodes[2][1]) - 1, 1E-14);

	hansel = dynamic_cast<LibmeshBinary::Triangle*>(el->Left());
	gretel = dynamic_cast<LibmeshBinary::Triangle*>(el->Right());
	ASSERT_NE(hansel, nullptr) << "Left child of bisected element is a nullptr";
	ASSERT_NE(gretel, nullptr) << "Right child of bisected element is a nullptr";

	nodes = hansel->Nodes();
	clog << "Left child nodes:" << endl;
	for (auto n : nodes)
		clog << n;
	clog << endl;

	EXPECT_LT(abs(nodes[0][0]), 1E-14);
	EXPECT_LT(abs(nodes[0][1]), 1E-14);
	EXPECT_LT(abs(nodes[1][0]) - 1, 1E-14);
	EXPECT_LT(abs(nodes[1][1]), 1E-14);
	EXPECT_LT(abs(nodes[2][0]) - 0.5, 1E-14);
	EXPECT_LT(abs(nodes[2][1]) - 0.5, 1E-14);

	nodes = gretel->Nodes();
	clog << "Right child nodes:" << endl;
	for (auto n : nodes)
		clog << n;
	clog << endl;

	EXPECT_LT(abs(nodes[0][0]) - 0.5, 1E-14);
	EXPECT_LT(abs(nodes[0][1]) - 0.5, 1E-14);
	EXPECT_LT(abs(nodes[1][0]) - 1, 1E-14);
	EXPECT_LT(abs(nodes[1][1]), 1E-14);
	EXPECT_LT(abs(nodes[2][0]) - 1, 1E-14);
	EXPECT_LT(abs(nodes[2][1]) - 1, 1E-14);
  
	clog << "TriangleBisection ended" << endl << endl;
};

TEST_F(LibmeshTest, ManualBinaryRefinement)
{
	clog << endl << "Starting ManualBinaryRefinement" << endl;

	int n = 1;
	libMesh::Mesh mesh(_mesh_init_ptr->comm());

	// Build a 1D mesh with n elements from x=1 to x=2
	libMesh::MeshTools::Generation::build_line(mesh, n, 1., 2., LibmeshIntervalType);
	libMesh::MeshRefinement refiner(mesh);

	auto& f_factory(BinaryTree::FunctionsFactory<1>::Instance());
	unique_ptr<BinaryTree::Functor<1>> smart_ptr = f_factory.create("x_squared");
	BinaryTree::FunctionPtr<1> f_ptr (smart_ptr.release());

#define MAKE_OPTION
#ifdef MAKE_OPTION
	LibmeshBinary::BinarityMap::MakeBinary<1>(refiner, f_ptr);
	LibmeshBinary::Interval* I = dynamic_cast<LibmeshBinary::Interval*>(mesh.elem(0));
	ASSERT_NE (I, nullptr) << "MakeBinary has not worked properly!";

#else //MAKE_OPTION

	//construction ed initialization of a Binary object of type Interval
	auto iter = mesh.elements_begin();
	auto ptr = dynamic_cast<LibmeshBinary::LibmeshIntervalClass*>(*iter);
	ASSERT_NE(copy_element_ptr, nullptr) << "Element type not recognized casting to LibmeshBinary::LibmeshIntervalClass*";

	auto smart_ptr = unique_ptr<LibmeshBinary::LibmeshIntervalClass>(ptr);
	LibmeshBinary::Interval* I = new LibmeshBinary::Interval(move(smart_ptr), f_ptr, refiner);
	I->Init();
	*iter = I;

	//to check if I have a LibmeshGeometry method as expected
	EXPECT_TRUE (I->active());

#endif //MAKE_OPTION

	clog << "I directly call the Interval refine method" << endl;
	I->Bisect();
	EXPECT_TRUE(LibmeshBinary::BinarityMap::CheckBinarity(mesh));
	EXPECT_NE(I->Left(), nullptr) << "After refinement the interval has not left child";
	EXPECT_NE(I->Right(), nullptr) << "After refinement the interval has not right child";
	
	auto hansel = dynamic_cast<LibmeshBinary::Interval*>(I->Left());
	auto gretel = dynamic_cast<LibmeshBinary::Interval*>(I->Right());
	ASSERT_NE(hansel, nullptr) << "Binary element not recognized casting to LibmeshBinary::Interval*";
	ASSERT_NE(gretel, nullptr) << "Binary element not recognized casting to LibmeshBinary::Interval*";

	EXPECT_TRUE(hansel->active());
	EXPECT_TRUE(gretel->active());	
	EXPECT_FALSE(I->active());
#ifdef TRY_IT
	hansel->Activate();
	gretel->Activate();
	I->Deactivate();
#endif //TRY_IT
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
TEST_F(LibmeshTest, BinaryRefinement)
{
	clog << endl << "Starting BinaryRefinement" << endl;

	auto mesh_ptr = make_shared<libMesh::Mesh> (_mesh_init_ptr->comm());

	int n = 2;
	libMesh::MeshTools::Generation::build_line(*mesh_ptr, n, 0, 1, LibmeshIntervalType);
	LibmeshBinary::LibmeshRefiner<1> binary_refiner;
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
TEST_F(LibmeshTest, IOTest)
{
	clog << endl << "Starting libMesh file IO test" << endl;

	libMesh::Mesh mesh(_mesh_init_ptr->comm());

	// Build a 2D square (1,2)x(1,2) mesh with nx subdivisions along x axis and ny subdivisions along y axis
	int nx = 1, ny = 1;
	libMesh::MeshTools::Generation::build_square(mesh, nx, ny, 1., 2., 1., 2., LibmeshTriangleType);
	mesh.write("./mesh2d.msh");

	clog << "libMesh file IO test ended" << endl << endl;
};

//TODO:	WATCH OUT! The first polinomial not exactly integrated has zero norm,
//			since I'm integrating Jacobi polinomial over Gauss Lobatto quadrature nodes,
//			which are by definition the roots of the the over mentioned polinomial.

#ifdef TRY_IT
TEST_F(LibmeshTest, FirstNotExactElement)
{
	clog << endl << "Starting FirstNotExactElement" << endl;

	int n = 1;

	libMesh::Mesh mesh1(_mesh_init_ptr->comm());
	libMesh::MeshTools::Generation::build_line(mesh1, n, 0., 1., LibmeshIntervalType);

	auto iter = mesh1.elements_begin();
	LibmeshBinary::FElement<1, LegendreType, LibmeshBinary::LibmeshIntervalClass> f_el(iter);
	
	f_el.Init();

	size_t i = f_el.QuadratureOrder();
	i /= 2;
	i++;

	clog << "Let's check the normality of the element #" << i << " of the basis :" << endl;

	double I = f_el.L2Norm	(
										[&](const Point<1>& p){ return f_el.EvaluateBasisFunction(i, p);}
									);
	I *= I * (i + 0.5);
	//TODO: do something on this value
	clog << "Integral: " << I << endl;

	clog << "FirstNotExactElement ended" << endl << endl;
};
#endif //TRY_IT
