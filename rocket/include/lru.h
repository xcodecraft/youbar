#ifndef __LRU_H__
#define __LRU_H__
#include "utls.h"
struct lru_impl;
class lru : public smarty_ptr<lru>
{
    public:
        lru(size_t maxsize, size_t maxsec);
        ~lru ();
        bool    cache(const char * key,size_t cnt=1);
        float   ratio();
        void    time_expire();
    private:
        lru_impl* _impl;
};
#endif
