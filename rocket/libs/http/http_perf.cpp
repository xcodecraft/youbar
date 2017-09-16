#include "http/http.h"
#include <boost/foreach.hpp>
#include "error/error.h"
#include "log/log_sysl.h"
#include "iostream"

using namespace pylon ;
using namespace pylon::http ;
using namespace std;
using namespace boost ;

struct socket_stub
{
    socket_stub():_call_times(0) {}
    uint read(char* buf, size_t len )
    {
        static char* block[] = {
            "HTTP/1.1 200 OK\r\nContent-type: text/plain\r\nTransfer-encoding: chunked\r\n\r\n5\r\n",
            "hello\r\n6\r\n world\r\n0\r\n"};
        char* data = block[_call_times];
        strncpy(buf,data,len);
        ++ _call_times ;
        return strlen(data);
    }
    void reset()
    {
        _call_times =0 ;
    }
    uint  _call_times;
};

bool parse_response( http::parser& parser, socket_stub& socket, response& resp)
{
    buffer_t head_buf   = resp.alloc_buffer();
    size_t   read_len   = socket.read(head_buf.unused_begin(), head_buf.unused_size());
    head_buf.mark_used(read_len); 

    result_t header_rs  = parser.parse_header(head_buf,resp);
    if(!header_rs.is_end() ) return false;

    buffer_t body_buf = head_buf.right(header_rs.parsed_length());
    result_t body_rs;
    while(true)
    {   
        body_rs =   resp.is_chunked() ?  parser.parse_chunk(body_buf,resp ) : parser.parse_body(body_buf,resp );
        if (body_rs.is_lack())
        {
            buffer_t unproc_buf;
            unproc_buf           = body_buf.right(      body_rs.parsed_length()  );
            body_buf             = resp.extend_buffer(  unproc_buf,body_rs.lack_length());
            read_len             = socket.read(body_buf.unused_begin(), body_buf.unused_size());
            body_buf.mark_used(read_len) ;
            continue ;
        }
        break;
    }
    if(!body_rs.is_end())  return false;
    resp.decode();
    return true;
}
int main(void )
{

    parser  p;
    socket_stub socket;
    int max = 1000000; 
    //        int max = 1; 
    for (int i=0 ; i< max ; ++i)
    {
        socket.reset();
        response resp;
        if(parse_response(p,socket,resp))
        {
            if (i == 0)
            {
                buffer_t body = resp.body();
                cout << resp << endl;
                cout <<  body.used_size() << " : " <<  body.begin()<< endl ;
            }
            if (i%10000 == 0) cerr << "." ;
        }
    }
    return 0;
}
