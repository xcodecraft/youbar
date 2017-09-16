
#include <boost/pool/singleton_pool.hpp>
#include <boost/pool/object_pool.hpp>
#include "mem_ctrl.h"
#include <list>
#include <boost/foreach.hpp>

using namespace std;
namespace pylon { 
    namespace http {
        template < typename T > struct pool_able
        {
            typedef boost::object_pool<T>  pool;
        };
        struct block_1k : pool_able<block_1k> { char _buf[1024]; };
        struct block_2k : pool_able<block_2k> { char _buf[1024*2]; };
        struct block_4k : pool_able<block_4k> { char _buf[1024*4]; };
        struct block_8k : pool_able<block_8k> { char _buf[1024*8]; };
        struct block_32k: pool_able<block_32k> { char _buf[1024*32]; };
        struct block_64k: pool_able<block_64k> { char _buf[1024*64]; };

        typedef std::list<char *> cptr_list_t;


        struct alloc_able::impl
        {
            block_1k::pool _pools_1k;
            block_2k::pool _pools_2k;
            block_4k::pool _pools_4k;
            block_8k::pool _pools_8k;
            block_32k::pool _pools_32k;
            block_64k::pool _pools_64k;
            cptr_list_t     _big_mems;
            ~impl()
            {
                BOOST_FOREACH( char* buf , _big_mems)
                {
                    delete[] buf;
                }
            }
            buffer_t alloc_buffer(size_t len )
            {/*{{{*/
                size_t fix_len = 1024 ;
                char * buf    = NULL;
                if ( len <= 1024) {
                    fix_len = 1024;
                    buf = (_pools_1k.malloc())->_buf;
                }
                else if( len <= 4096 )
                {
                    fix_len = 4096;
                    block_4k *pblock =  _pools_4k.malloc();
                    buf = pblock->_buf;
                }
                else if(len <= 1024 * 8 )
                {
                    fix_len = 4096;
                    block_8k *pblock = _pools_8k.malloc();
                    buf = pblock->_buf;
                }
                else if(len <= 1024 * 32 )
                {
                    fix_len = 1024 * 32 ;
                    buf = (_pools_32k.malloc())->_buf;
                }
                else if(len <= 1024 * 64 )
                {
                    fix_len = 1024 * 64 ;
                    block_64k* pblock =  _pools_64k.malloc();
                    buf = pblock->_buf;
                }
                else
                {
                    fix_len = len ;
                    buf  = new char[len];
                    _big_mems.push_back(buf);
                }
                buffer_t bufobj(fix_len,buf);
                memset(bufobj.begin(),0,fix_len);
                return bufobj;
            }/*}}}*/
        };
        alloc_able::alloc_able():_pimpl(new alloc_able::impl){}
#define CLS alloc_able 
        IMPL_METHOD1(buffer_t,alloc_buffer,size_t);
    }
}
