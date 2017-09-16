#include "text/conf_core.h"
#include <boost/foreach.hpp>
#define BOOST_TEST_MODULE  conf_test_conf
#include <boost/test/included/unit_test.hpp>
#include  "text/text.h"
#include "error/error.h"
#include <fstream> 
using namespace pylon ;
using namespace pylon::rigger;
using namespace std;

//template< typename T >
//void append_value(conf_object::optr o,const wstring& key, T value)
//{
//    o->append(key,conf::sptr(new conf_value(key,value)));
//}
BOOST_AUTO_TEST_CASE( conf_object_test)
{

//    conf_object::optr x1( new conf_object(L"debug"));
//    append_value(x1,L"debugon",1);
//    append_value(x1,L"log_level",1);
//    append_value(x1,L"log_error",L"WARNNING");

//组合对象
//    conf_object::optr x2( new conf_object(L"tools"));
//    x2->append(L"debug",x1);



//    conf_object::optr x2( new conf_object(L"nodebug"));
//    x2->append(L"log_level",conf::sptr(new conf_value(L"log_level",0)));
//    x2->append(L"php_error",conf::sptr(new conf_value(L"php_error",L"ERROR")));
//    x2->append_value(L"debug",0);
//    x1.to_conf(wcout);
//    conf_object::optr y1( new conf_object(L"dev"));
//    y1->inherit(x1.get());
//    y1->append_value(L"host",L"127.0.0.1");
//    y1->to_conf(wcout);
	//conf_object::optr x3( new conf_object(L"online"));
}

