#include "http/http.h"
#include <boost/foreach.hpp>
//#define BOOST_TEST_MODULE  test_gram
#include <boost/test/unit_test.hpp>
#include  "text/text.h"
#include  "grammar.h"
#include "error/error.h"
#include <fstream> 
using namespace pylon ;
using namespace pylon::http ;
using namespace std;
using namespace boost ;

const char* char_end=0;
    template < typename T, typename X> bool parse( const std::string& c,  T rule , X& x )
{

    const char* begin = c.c_str();
    const char* end  = begin + c.length();
    return qi::phrase_parse(begin,end,rule,encoding::space,x  );
}
    template < typename T > bool parse( const std::string& c,  T rule  )
{
    const char* begin = c.c_str();
    const char* end  = begin + c.length();
    return qi::phrase_parse(begin,end,rule,encoding::space  );
//    return true ;
}

BOOST_AUTO_TEST_CASE( base_grammar)
{
    http_grammar g;
    uint sec ;
    BOOST_CHECK(parse("max-age = 600 ",g._cache_ctrl,sec)) ;
    BOOST_CHECK(parse("max-stale = 600 ",g._cache_ctrl,sec)) ;
    BOOST_CHECK(sec == 600 );
}
BOOST_AUTO_TEST_CASE( parse_grammar)
{
    http_grammar g;
    request requ;
    g.bind_header(requ);
    BOOST_CHECK(parse("GET",g._method)) ;
    BOOST_CHECK(parse("HTTP/1.0",g._ver)) ;
    BOOST_CHECK(requ.method  == "GET");
    BOOST_CHECK(requ.version == "HTTP/1.0"  );

    BOOST_CHECK(parse("Accept: text\r\n11",g._header_line)) ;
    BOOST_CHECK(parse("Accept: text\r\n",g._header_line)) ;
    string m;
    BOOST_CHECK(requ.get_header("Accept",m));
    cout << "[header] Accept:" <<  m  << endl;
    BOOST_CHECK(m  == "text" ) ;

    BOOST_CHECK(parse("/test/hi-there.txt  1",g._url)) ;
    cout << requ.url << endl ;
    BOOST_CHECK(parse("GET /test/hi-there.txt HTTP/1.1\r\n",g._requ_line)) ;
    BOOST_CHECK(parse("Accept: text/*\r\nHost: www.360.cn \r\n",g._headers)) ;
    cout << requ << endl;

    request requ1;
//    g._request = &requ1 ;
    g.bind_header(requ1);

    char* r= "GET /test/hi-there.txt HTTP/1.1\r\nAccept: text/*\r\nHost:www.36.cn \r\n\r\n";
    BOOST_CHECK(parse(r,g._requ_header)) ;
    BOOST_CHECK(requ1.method == "GET") ;
    cout << requ1 << endl ;

//    for(int i = 0 ; i < 100000 ; i++ )
//    {
//        request requ2;
//        g.bind(requ2);
//        parse(r,g._requ_header);
//    }
}
BOOST_AUTO_TEST_CASE( resp_grammar)
{
    
    http_grammar g;
    response  resp;
    g.bind_header(resp);
    char* r= "HTTP/1.0 200 OK\r\n";
    BOOST_CHECK(parse(r,g._resp_line)) ;
    cout << resp << endl ;
    r = "HTTP/1.0 200 OK\r\nContent-type: text/plain\r\nContent-length: 19\r\n\r\nHi! I'm a message!";

    response  resp1;
    g.bind_header(resp1);
    BOOST_CHECK(parse(r,g._resp_header)) ;
    cout << resp1 << endl ;
    uint len = 0 ;
    BOOST_CHECK(parse("11\r\n",g._chunk_header,len));
    cout << "chunk_size: " << len ;
}

BOOST_AUTO_TEST_CASE( parse_requ)
{
//    char* r= "GET /test/hi-there.txt HTTP/1.1\r\n Accept: text/*\r\n Host:www.36
//        .cn \n\n";
//    parser p;
//    request  requ;
//    p.parse_requ(r,&requ);
}
