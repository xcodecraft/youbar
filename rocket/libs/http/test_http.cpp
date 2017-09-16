#include "http/http.h"
#include <boost/foreach.hpp>
#define BOOST_TEST_MODULE  test_http
#include <boost/test/unit_test.hpp>
#include  "text/text.h"
#include "error/error.h"
#include <fstream> 
#include "log/log_sysl.h"
#include <iostream>
#include <sstream>
using namespace pylon ;
using namespace pylon::http ;
using namespace std;
using namespace boost ;

template <typename T> buffer_t build_data(const char* data, T& resp)
{
    buffer_t buf = resp.alloc_buffer(1024*10);
    buf.mark_used( strlen(data) );
    strcpy(buf.begin(), data);
    return  buf;
}
BOOST_AUTO_TEST_CASE( get_request)
{
    request requ;
    requ.use_get("wan.360.cn","/v");

    stringstream s;
    s << requ ;
    BOOST_CHECK_EQUAL(s.str() ,"GET /v HTTP/1.1\r\nHost: wan.360.cn\r\n\r\n");
    cout << requ << endl;
}
BOOST_AUTO_TEST_CASE( test_parse_requ)
{
    log_kit::init("http_test","",log_kit::debug);
    request  requ;
    parser  p;
    buffer_t buf = build_data("GET /test/hi-there.txt HTTP/1.1\r\nAccept: text/*\r\nHost:www.36.cn \r\n\r\n" , requ);
    BOOST_CHECK(p.parse_header(buf,requ).is_end());
    cout << requ << endl;

}
BOOST_AUTO_TEST_CASE( test_parse_resp)
{
    log_kit::init("http_test","",log_kit::debug);
    response resp;
    parser  p;
    buffer_t buf = resp.alloc_buffer();
    strcpy(buf.begin(),"HTTP/1.1 200 OK\r\nContent-type: text/plain\r\nContent-length: 18\r\n\r\n");
    BOOST_CHECK(p.parse_header(buf,resp).is_end());

    std::cout << "-----------------\n";
    std::cout << resp << endl ;
    if (resp.trans_encoding == response::te_none)
    {

        buffer_t buf = build_data( "Hi! I'm a message!", resp);
        BOOST_CHECK(p.parse_body(buf,resp).is_end());
        resp.decode();
        std::cout << "body: " << resp.body().begin() << endl;
    }



}

BOOST_AUTO_TEST_CASE( test_parse2)
{
    log_kit::init("http_test","",log_kit::debug);
    response resp;
    parser  p;
    char* data ="HTTP/1.1 200 OK\r\nContent-type: text/plain\r\nTransfer-encoding: chunked\r\n\r\n";
    buffer_t   buf = build_data(data,resp);
    BOOST_CHECK(p.parse_header(buf,resp).is_end());

    int runtag =0;
    if (resp.trans_encoding == response::chunked)
    {
        data="5\r\nhe";
        runtag  += 1;
        bool   stop = false;
        int times=0;
        while(!stop)
        {
           buffer_t         buf = build_data(data,resp);
           result_t r   = p.parse_chunk(buf,resp );
           switch(r.status())
           {
               case  result_t::LACK:
                   if(times == 0)
                   {
                       data="5\r\nhello\r\n";
                       runtag += 10 ;
                   }
                   if(times == 1)
                   {
                       data ="5\r\n my  \r\n6\r\n world\r\n0\r\n";
                       runtag += 100 ;
                   }
                   times ++ ;
                   break;
               case result_t::BAD:
                   stop = true;
                   break;
               case result_t::END :
                   runtag +=1000 ;
                   stop = true;
                   break;
           }
        }
        std::cout << "runtag : " << runtag  << endl;
        BOOST_CHECK(runtag == 1111);
        resp.decode();
        buffer_t body = resp.body();
        std::cout << "------------ parse chunk -----------\n";
        std::cout << resp << endl ;
        std::cout << "body: " << body.begin() << " used:" << body.used_size() << endl ;
        std::cout << "------------ parse chunk end -----------\n";
        BOOST_CHECK(memcmp(body.begin(),"hello my   world",body.used_size()) == 0 );
    }
}

BOOST_AUTO_TEST_CASE( bad_chunk)
{
    log_kit::init("http_test","",log_kit::debug);
    response resp;
    parser  p;
    buffer_t buf = resp.alloc_buffer();
    strcpy(buf.begin(),"HTTP/1.1 200 OK\r\nContent-type: text/plain\r\nTransfer-encoding: chunked\r\nCache-control: max-age = 600\r\n\r\n");
    BOOST_CHECK(p.parse_header(buf,resp).is_end());
    BOOST_CHECK(resp.cache_sec() == 600 );
    char * data ="";
    if (resp.trans_encoding == response::chunked)
    {
        buffer_t buf = resp.alloc_buffer(1024*10);

        data ="4\r\nhello\r\n6\r\n world\r\n";
        buf.mark_used( strlen(data));
        strcpy(buf.begin(), data);
        BOOST_CHECK(p.parse_chunk(buf,resp).is_bad());


        data ="5\r\nhello\r\n5\r\n world\r\n";
        buf.mark_used(  strlen(data));
        strcpy(buf.begin(), data);
        BOOST_CHECK(p.parse_chunk(buf,resp).is_bad());

        data ="5\rhello\r\n6\r\n world\r\n";
        buf.mark_used( strlen(data));
        strcpy(buf.begin(), data);
        BOOST_CHECK(p.parse_chunk(buf,resp).is_bad());

        resp.decode();
        cout << "body:" << resp.body().begin() << endl;
    }
}
