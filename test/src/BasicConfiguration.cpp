#include "BasicConfiguration.h"
	
using namespace std;


BasicTest::BasicTest() : _out("test.log"){};

BasicTest::~BasicTest()
{
#ifdef DESTRUCTOR_ALERT
	clog << "Sono nel distruttore di BasicTest" << endl;
#endif //DESTRUCTOR_ALERT
};

void BasicTest::SetUp()
{
#ifdef MYDEBUG
	clog << "Sono nella SetUp di BasicTest" << endl;
#endif //MYDEBUG
};

void BasicTest::TearDown()
{
#ifdef MYDEBUG
	clog << "Sono nella TearDown di BasicTest" << endl;
#endif //MYDEBUG
};

