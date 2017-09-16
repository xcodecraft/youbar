#include "error/error.h"
#include "log/log.h"
#define BOOST_TEST_MODULE TEST_ERROR_INS 
#include <boost/test/included/unit_test.hpp>
using namespace pylon ;

using namespace std;

#if defined OS_UNIX
    #define ROOT_PATH   L"/tmp"
    #define C         
#else
    #define ROOT_PATH   L"D:/"
    #define C       L      
#endif





BOOST_AUTO_TEST_CASE( my_test)
{
    try
    {
        MUST_SUC(1 == 2);
    }
    catch(bug_error& e)
    {
        cout << e.what() << endl ;
        cout << boost::diagnostic_information(e) ;
    }

    try
    {
        bug_error err;
        int x= EXPECT(1 , 2, err);
        BOOST_CHECK(x);
    }
    catch(bug_error& e)
    {
        cout << e.what() << endl ;	
        cout << boost::diagnostic_information(e) ;
    }

    try
    {
        bug_error err;
        int x= EXPECT_NOT(1,1,err);
        BOOST_CHECK(x);
    }
    catch(bug_error& e)
    {
        cout << e.what() << endl ;	
        cout << boost::diagnostic_information(e) ;
    }
}
