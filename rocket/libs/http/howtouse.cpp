#include "http/http.h"
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>
#include  "text/text.h"
#include "error/error.h"
#include <fstream> 
using namespace pylon ;
using namespace pylon::http ;
using namespace std;
using namespace boost ;


struct socket_stub
{
    socket_stub():_call_times(0)
                  uint read(char* buf, size_t len )
    {
        static char* block[] = {"HTTP/1.1 200 OK\r\nContent-type: text/plain\r\nTransfer-encoding: chunked\r\n\r\n5\r\n",
            "hello\r\n6\r\n world\r\n0\r\n"};
        char* data = block[_call_times];
        strncpy(buf,data,len);
        ++ _call_times ;
        return strlen(data);
    }
    uint  _call_times;
}
BOOST_AUTO_TEST_CASE(ex_parse_response)
{
    parser  p;
    socket_stub socket;

    response resp;

    buffer_t head_buf= resp.alloc_buffer();
    head_buf.used = socket.read(head_buf.ptr, head_buf.len );
    result_t hr   =p.parse_header(head_buf,resp);
    result_t c_res;
    if(is_end(hr) )
    {
        buffer_t::sptr old_buf;
        buffer_t::sptr cur_buf( new buffer_t(head_buf.right(hr.used_len)));
        while(1)
        {   
            if(  resp.is_chunked() ) {
                c_res   = p.parse_chunk(*cur_buf,resp );
            }
            else {
                c_res   = p.parse_body(*cur_buf,resp );
            }
            if (is_lack(c_res))
            {
                    old_buf = cur_buf;
                    cur_buf.reset(new buffer_t( resp.alloc_buffer()));
                    cur_buf->copy(old_buf->right(c_res.used_len));
                    cur_buf->used =socket.read(cur_buf->used_end(), cur_buf->unuse_size());
                    continue ;
            }
            break;
        }
        if(is_end(c_res))
        {
            resp.decode();
            buffer_t body = resp.body();
            cout << resp << endl;
            cout <<  body.used << ":" <<  body.ptr << endl ;
        }
    }

}
