#include "text/conf_core.h"
#include "../conf_impl.h"
#include <boost/foreach.hpp>
#define BOOST_TEST_MODULE  conf_test_parser
#include <boost/test/included/unit_test.hpp>
#include  "text/text.h"
#include "error/error.h"
#include <fstream> 
using namespace pylon ;
using namespace pylon::rigger;
using namespace std;


BOOST_AUTO_TEST_CASE( conf_parse)
{
    return ;
    char* fname="../libs/text/test/x.conf";
    ifstream conf_file(fname); 
    char buf[10240] ;

    memset(buf,0,10240);
    conf_file.read(buf,10240);
    parser p;
    stringstream content;
    content << " space g { " << buf <<  " } "  ;
    conf::sptr x ;
    BOOST_CHECK(p.parse_utf8(content.str(),x));

    pylon::logger* l = pylon::log_kit::log_ins(L"test");
    pylon::log_kit::conf(L"test",L"text_conf",log_kit::debug);
    try
    {
        x->extend();
        x->xpath(L"eve.dev")->export_val(l);
        x->xpath(L"env.dev")->use(l);
        x->xpath(L"admin")->use(l);
        x->clear();
    }
    catch( wbug_error& e)
    {
        wcout << e.what() << endl;
    }
    pylon::log_kit::clear();



}

BOOST_AUTO_TEST_CASE( zconf_test)
{
//    try
//    {
//        char* fname="../libs/text/test/y.conf";
//        zconf zc;
//        zc.import(fname);
//        zc.link();
//        zc.use("env.dev");
//        zc.use("admin");
//        zc.call(conf::positive,"config","admin");
//        zc.call(conf::reverse,"clean","admin");
//    }
//    catch(wbug_error& e )
//    {
//        wcout << e.what() << endl;
//    }

}
