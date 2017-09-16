#ifndef __MEM_CTRL__HPP__ 
#define __MEM_CTRL__HPP__ 
#include <pimpl.h>
#include "http/http.h"
#include <boost/pool/singleton_pool.hpp>



namespace pylon { 
    namespace http {
        struct alloc_able 
        {
            buffer_t alloc_buffer(size_t len );
            struct impl;  
            boost::shared_ptr< impl>  _pimpl;
            alloc_able();
        };

        template <typename T> struct mem_pool
        {
            void* operator new(size_t size) 
            {
                typedef boost::singleton_pool<T, sizeof(T)> pool;
                void* p = pool::malloc();
                return  p; 
            }
            void  operator delete(void * p) {
                typedef boost::singleton_pool<T, sizeof(T)> pool;
                pool::free(p);
            }
        };
    }
}
#endif 
