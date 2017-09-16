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



BOOST_AUTO_TEST_CASE(test_log)
{
    log_kit::conf(L"test1" , ROOT_PATH,log_kit::debug);
    log_kit::conf(L"test2" , ROOT_PATH,log_kit::info);
    INS_LOG(log1,L"test1");
    INS_LOG(log2,L"test2");
    WLOG_DEBUG(log1) << "debug";
    WLOG_DEBUG(log2) << "debug";
    WLOG_INFO(log2) << "info";
    log_kit::clear();
}

