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

class test_cmd : public cmd
{/*{{{*/
    public:
        conf_args  _args;
        void  call(const wchar_t* name ,conf::order o ,const char* cmd ,const conf_args* args, pylon::logger* l)
        {
            _args = *args;
            wcout << name << "::" << cmd << endl;
            BOOST_FOREACH( conf_args::pair x , *args)
            {
                wcout << "\t" << x.first << L": " << x.second << endl ;
            }
        }

};/*}}}*/
BOOST_AUTO_TEST_CASE( conf_object_test)
{
    wstring split_format(L"-------------------------------------\n");

    conf_object* x1 =  new conf_object(L"debug");
    x1->append_value(L"debugon",1);
    x1->append_value(L"log_level",1);
    x1->append_value(L"PATH",L"${HOME}/my");
    x1->append_value(L"log_error",L"WARNNING");

//组合对象
    conf_object* x2= new conf_object(L"tools");
    x2->append(x1);
    conf_args a1;
    a1[L"HOME"] = L"";
    a1[L"DEBUG"] = L"xxx";
    x1->set_default_args(&a1);

    conf_args a2;
    a2[L"HOME"]=L"/home/";
    a2[L"PRJ_NAME"]=L"rocket";
    INS_LOG(log1,L"test");

    test_cmd c1,c2,c3;
    x1->bind_cmd(&c1);
    x2->bind_cmd(&c2);
    x2->call(conf::positive,"conf",&a2,log1);
    BOOST_CHECK(c1._args[L"HOME"] == a2[L"HOME"]);
    BOOST_CHECK(c2._args[L"HOME"] == a2[L"HOME"]);

    //继承
    conf_object* x3= new conf_object(L"my_tools");
    x3->inherit(x2);
    x3->bind_cmd(&c3);
    wcout << split_format ;
    x3->call(conf::positive,"conf",&a2,log1);
    conf::path_arr_t p;
    wcout << split_format ;
    x2->to_conf(wcout,L"");
    wcout << split_format ;
    x3->to_conf(wcout,L"");
    parser::xpath_split(L"debug",p);
    conf* found = x2->xpath(p);
    BOOST_CHECK(found );

    conf_module* y1=  new conf_module(L"utls");
    y1->append(x3);
    y1->append(x2);
    wcout << split_format ;

    y1->to_conf(wcout,L"");

    conf::path_arr_t p2;
    parser::xpath_split(L"tools.debug",p2);
    found = y1->xpath(p2);

    module_space::append(y1);
    wcout << split_format ;
    module_space::to_conf(wcout,L"");
    found = module_space::xpath(L"utls.tools.debug");
    BOOST_CHECK(found );

    conf_system admin(L"admin");
    admin.append(x2);
    admin.append(x2);
    conf_args a3;
    admin.call(conf::positive,"conf",&a3,log1);

}

