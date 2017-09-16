#include "text/rigger.h"
#include <boost/foreach.hpp>
#define BOOST_TEST_MODULE  conf_test_conf
#include <boost/test/included/unit_test.hpp>
#include  "../conf_grammar.h"
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
BOOST_AUTO_TEST_CASE( grammar_test)
{

    conf_grammar  g;


    std::wstring x ;
    BOOST_CHECK(g.test_impl(L" dev " ,g.key,x)) ;
    BOOST_CHECK(x == wstring(L"dev")) ;
    conf::value_type x1;
    BOOST_CHECK(g.test_impl(L"\"| dev |\" " ,g.value,x1));
    wcout << L"x1: " << x1 <<endl ;
    BOOST_CHECK(boost::get<wstring>(x1) == wstring(L"| dev |")) ;

    BOOST_CHECK(g.test_impl(L"234"  ,g.value,x1));
    BOOST_CHECK(boost::get<int>(x1) == 234) ;

    conf_holder  x2=NULL;
    BOOST_CHECK(g.test_impl(L"dev : \"123 \" ",g.u_value,x2));
    BOOST_CHECK(x2 != NULL) ;
    conf_value* x21 = dynamic_cast<conf_value*>( x2) ;
    BOOST_CHECK(x21!=NULL) ;
    BOOST_CHECK(boost::get<wstring>(x21->_value) == L"123 " ) ;

    wstring x3 ;
    BOOST_CHECK(g.test_impl(L" :: debug,nodebug ",g.u_parent,x3));
    BOOST_CHECK(x3 == wstring(L"debug,nodebug")) ;


    wstring x4;
    BOOST_CHECK(g.test_impl(L"(A:1,B:2)",g.u_args,x4));
    BOOST_CHECK(x4 == wstring(L"A:1,B:2")) ;
    wcout<< x4 << endl;

    x4=L"";
    BOOST_CHECK(g.test_impl(L"(A:1,debug)",g.u_args,x4));
    BOOST_CHECK(x4 == wstring(L"A:1,debug")) ;
    wcout<< x4 << endl;
    
    conf_object::attr a1 ;
    BOOST_CHECK(g.test_impl(L"env",g.u_attr,a1));
    wcout << L"<0> : " << boost::fusion::at_c<0>(a1) << endl;

    conf_object::attr a2;
    BOOST_CHECK(g.test_impl(L"env(a :1 ,b:2) ",g.u_attr,a2));
    wcout << L"<0> : " << boost::fusion::at_c<0>(a2) << endl  ;
    wcout << L"<1> : " << boost::fusion::at_c<1>(a2).get() << endl ;

    conf_object::attr a3;
    BOOST_CHECK(g.test_impl(L"env(a :1 ,b:2) :: online,debug  ",g.u_attr,a3));
    wcout << L"<0> : " << boost::fusion::at_c<0>(a3) << endl  ;
    wcout << L"<1> : " << boost::fusion::at_c<1>(a3).get() << endl ;
    wcout << L"<2> : " << boost::fusion::at_c<2>(a3).get() << endl ;

    conf::sptr_vector v1;
    BOOST_CHECK(g.test_impl(L"{}",g.u_items,v1));
    BOOST_CHECK(v1.size() == 0 );


    conf::sptr_vector v2;
    BOOST_CHECK(g.test_impl(L"{ A: 1 }",g.u_items,v2));
    BOOST_CHECK(v2.size() == 1 );

    conf_holder x5=NULL ;
    BOOST_CHECK(g.test_impl(L"env {}  ",g.u_object,x5));
    BOOST_CHECK(x5 != NULL) ;
    wcout << x5->name() << endl; 

    x5=NULL ;
    BOOST_CHECK(g.test_impl(L"env_1 (a:1 ,b:2) :: online,debug { x:1,y:2 }  ",g.u_object,x5));
    BOOST_CHECK(x5 != NULL) ;
    wcout << x5->name() << endl; 


    conf_holder x6=NULL ;
    BOOST_CHECK(g.test_impl(L"module utls { env (a:1 ,b:2) :: online,debug { x:1,y:2 }  } ",g.u_module,x6));
    BOOST_CHECK(x6 != NULL) ;
    wcout << x6->name() << endl; 


    wstring c1 = L"module utls { env (a:1 ,b:2) :: online,debug { x:1,y:2 }  }   module utls2 { }  ";

    pylon::rigger::rigger::init();
    BOOST_CHECK(g.parse(c1));
    wcout << module_space::xpath(L"utls")->name() << endl;
    wcout << module_space::xpath(L"utls.env")->name() << endl;


    c1 = L"module utls3 { env (a:1 ,b:2) :: online,debug { x:1,y:2 }  }   module utls4 { }  ";

    pylon::rigger::rigger::init();
    BOOST_CHECK(g.parse(c1));
    wcout << module_space::xpath(L"utls3")->name() << endl;
    wcout << module_space::xpath(L"utls3.env")->name() << endl;

	
}

namespace conf_test
{
    struct use_var_str
    {
        typedef std::wstring result_type;
        template <typename Arg1, typename Arg2>
            std::wstring operator()(Arg1 arg1,Arg1 arg2, Arg2 arg3 ) const
            { 
                return   arg1 + std::wstring(L"****") ;
            }
    };

}

BOOST_AUTO_TEST_CASE( var_proc)
{
//    conf_grammar<std::wstring::const_iterator>   g;
//    g.var_cacul_init<conf_test::use_var_str>(NULL,NULL);
//    conf_value::vptr x5;
//    wstring y;
//    BOOST_CHECK(g.var_cacul(L"${debug}",x5));
//    y=boost::get<wstring>(x5->_value);

//    BOOST_CHECK(g.var_cacul(L"xxx$",x5));
//    y=boost::get<wstring>(x5->_value);
//    BOOST_CHECK(g.var_cacul(L"xxx${debug}yyyy${debug}",x5));
//    y=boost::get<wstring>(x5->_value);


}

