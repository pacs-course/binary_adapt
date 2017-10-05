#include "DinamicLoadConfiguration.h"

#include "Functor.h"
//#include "MyFunctors.h"

using namespace std;

//TODO: complete the test with the assignment operator and the direct construction
//TEST_F(LoadTest, MuParser)
//{
//	clog << endl << "Starting MuParser" << endl;

//	//testing the getpot initialization
//	auto& one_f_factory(BinaryTree::FunctionsFactory<1>::Instance());
//	//TODO: here I have SEGFAULT because of the conflict between Getpot and libMesh
//	unique_ptr<BinaryTree::Functor<1>> par_fun_one_d_ptr = move(one_f_factory.create("mu_parser_expr"));

//	array<double, 1> val;
//	val[0] = 2;
//	double result = (*par_fun_one_d_ptr)(val);
//	//i'm supposing the expression is x^2, the default one
//	double error = fabs(result - 4);
//	EXPECT_LT(error, 1E-4)	<< "MuParser functor 1D evaluation failed:" << endl
//									<< "expected: 4" << endl
//									<< "result:" << result << endl;

//	auto& two_f_factory(BinaryTree::FunctionsFactory<2>::Instance());
//	unique_ptr<BinaryTree::Functor<2>> par_fun_two_d_ptr = move(two_f_factory.create("mu_parser_expr"));

//	array<double, 2> vals;
//	vals[0] = 2;
//	vals[1] = 3;
//	result = (*par_fun_two_d_ptr)(vals);
//	//I'm supposing the expression is x*y, the default one
//	error = fabs(result - 6);
//	EXPECT_LT(error, 1E-4)	<< "MuParser functor 2D evaluation failed:" << endl
//									<< "expected: 6" << endl
//									<< "result:" << result << endl;

//	clog << "MuParser ended" << endl << endl;
//};

