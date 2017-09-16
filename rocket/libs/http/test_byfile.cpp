#include "http/http.h"
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>
#include "error/error.h"
#include <fstream> 
#include <iostream> 


using namespace pylon ;
using namespace pylon::http ;
using namespace std;
using namespace boost ;

bool parse_file(parser& p, const char* name)
{/*{{{*/

    response resp;
    ifstream data_file(name);
    buffer_t head_buf   = resp.alloc_buffer();
    size_t   read_len   = data_file.readsome(head_buf.unused_begin(), head_buf.unused_size());
    head_buf.mark_used(read_len); 

    result_t h_res    = p.parse_header(head_buf,resp);
    if(!h_res.is_end() ) 
    {
        std::cout <<  "header: " <<  head_buf.begin() << endl;
        return false ;
    }

    buffer_t cur_buf = head_buf.right(h_res.parsed_length());
    result_t c_res;
    while(true)
    {   
        c_res =   resp.is_chunked() ?  p.parse_chunk(cur_buf,resp ) : p.parse_body(cur_buf,resp );
        if (c_res.is_lack())
        {
            buffer_t unparsed    = cur_buf.right(c_res.parsed_length());
            cur_buf              = resp.extend_buffer(unparsed,c_res.lack_length());
            uint read_len        = data_file.readsome(cur_buf.unused_begin(), cur_buf.unused_size());
            cur_buf.mark_used(read_len) ;
            continue ;
        }
        break;
    }
    if(c_res.is_bad())
    {
        std::cout << " parse  failed! " << name << endl ;
        return false;
    }
    return true ;
}/*}}}*/

BOOST_AUTO_TEST_CASE( test_by_file)
{
    parser  p;
    BOOST_CHECK(parse_file(p,"../test_data/http/resp_1.txt"));
    BOOST_CHECK(parse_file(p,"../test_data/http/resp_2.txt"));
//    BOOST_CHECK(parse_file(p,"/home/zuowenjian/devspace/rocket/test_data/http/resp_3.txt"));
}
