#include "text/rigger.h"
#include <boost/foreach.hpp>
#define BOOST_TEST_MODULE  test_rg_base 
#include <boost/test/included/unit_test.hpp>
#include  "text/text.h"
#include "error/error.h"
#include <fstream> 
using namespace pylon ;
using namespace pylon::rigger;
using namespace std;

BOOST_AUTO_TEST_CASE( conf_rg_py)
{

    conf_object* res_t1= new conf_object(L"res_t1");
    res_t1->append_value(L"X1",1);
    res_t1->append_value(L"X2",L"str_x2");
    res_t1->append_value(L"X3",3.14);

    conf_object* res_t2= new conf_object(L"res_t2");
    res_t2->append_value(L"Y1",1);
    res_t2->append_value(L"Y2",L"str_y2");
    res_t2->append_value(L"Y3",3.14);

    conf_module* utls= new conf_module(L"utls");
    utls->append(res_t1);
    utls->append(res_t2);

    module_space::append(utls);



    conf_system admin(L"admin");
    conf* found;
    found = module_space::xpath(L"utls.res_t1");
    admin.append(found);
    found = module_space::xpath(L"utls.res_t2");
    admin.append(found);
    admin.call(conf::positive,"conf",a3,log1);
}
