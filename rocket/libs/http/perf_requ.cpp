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
        static char* block[] = {" GET /test/hi-there.txt HTTP/1.1\r\nAccept: text/*\r\nHost:www.36.cn \r\n\r\n" };
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
int main(void )
{

    parser  p;
    socket_stub socket;
    int max = 1000000; 
    for (int i=0 ; i< max ; ++i)
    {
        socket.reset();
        request requ;
        buffer_t head_buf= requ.alloc_buffer();
        size_t   read_len   = socket.read(head_buf.unused_begin(), head_buf.unused_size());
        head_buf.mark_used (read_len); 
        result_t hr   = p.parse_header(head_buf,requ);
        if(hr.is_end() )
        {
            if (i == 0)
            {
                cout << requ << endl;
            }
            if (i%10000 == 0) cerr << "." ;
        }
    }
    return 0;
}
