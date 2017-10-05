#include "BasicConfiguration.h"
	
using namespace std;

Logfile::Logfile(const string& filename) : _ofs(filename, ios_base::app)
														,_out_buf(cout.rdbuf(_ofs.rdbuf()))
														,_log_buf(clog.rdbuf(_ofs.rdbuf()))
														,_err_buf(cerr.rdbuf(_ofs.rdbuf()))
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
#ifdef MYDEBUG
	cerr << "Buffer rediretti" << endl;
#endif //MYDEBUG
};

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

