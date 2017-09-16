#include "wan/gdict.h"
#include "error/log.h"
#include <boost/format.hpp>
#define BOOST_TEST_MODULE MyTest
#include <boost/test/included/unit_test.hpp>
using namespace std    ;
using namespace pylon  ;
using namespace boost  ;
using namespace wan360 ;


#if defined OS_UNIX
#define ROOT_PATH   L"/tmp"
#else
#define ROOT_PATH   L"D:/"
#endif
BOOST_AUTO_TEST_CASE( gdict_x)
{
	gdict gt;
	json::node::sptr data ;
	log_def::conf(L"test1" , ROOT_PATH,log_def::debug);
	INS_LOG(log1,L"test1");
	if(gt.list_zone_by_gkey("2310","sxd", data, log1))
	{
		node::sptr f = data->xpath(L"result");
		wstring result = node_cast<wstring>(f);
	}

}