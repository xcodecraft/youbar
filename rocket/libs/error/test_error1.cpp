//#include "error/error.h"
#define BOOST_TEST_DYN_LINK 
#define BOOST_TEST_MODULE MyTest2
#include <boost/test/unit_test.hpp>
using namespace std;

BOOST_AUTO_TEST_CASE( first_test)
{

	//using namespace pylon ;
    wstring u16 = L"PYLON¿ÉÒÔÓÃÃ´" ;
	wcout << u16 << endl;
	/*
	try
	{
		NO_ERR(false,bug_error);
	}
	catch(bug_error& e)
	{
		wcout << e.what() << endl ;
	}
	*/
}
