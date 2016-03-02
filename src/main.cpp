#include <iostream>
//#include <array>
//#include <cmath> //pow

#include "Tensorize.h"
#include "Point.h" 
using namespace std;
using namespace Geometry;


int main()
{
	Tensor<double, 2> test(vector<double>(3,1),vector<double>(2,2));
	cout << test.size() << endl;
	
	Point<1> p_test({1});	
	Point<1> p_copy(p_test);
	Point<1> p1 = p_copy;
	array<double, 2> init ({2,3});
	Point<2> p2(init);
	Point<3> p3 = p1 * p2;
	p1.print();
	p2.print();
	p3.print();
	return 0;
};

