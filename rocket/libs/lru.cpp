#include "lru.h"
#include <list>
#include <map>
#include "logger.h"
#include <sstream>
using namespace std;
//using namespace boost;
typedef list<std::string>  str_list_t;
struct pos_cnt
{
    str_list_t::iterator    pos;
    size_t                  cnt;
};
typedef map<std::string,pos_cnt> key_it_map_t;
struct lru_impl
{

    lru_impl(size_t maxsize,size_t maxsec):_maxsize(maxsize),_maxsec(maxsec),_access(1),_cached(0){}
    void time_expire()
    {
        int expire =  _maxsize/ _maxsec ;
        if (expire * _maxsec   < _maxsize )
            expire ++ ;
        for ( int i = 0 ; i < expire ; ++i)
        {
            empty_head();
            remove_tail();
        }

        LOG_DEBUG(" expire  obj  cnt: " << expire  << " size: " << _list.size());
    }
    bool cache(const char * key,size_t cnt =1 )
    {
        return cache(string(key), cnt);
    }
    bool cache(const std::string& key,size_t cnt =1  )
    {/*{{{*/
        _access ++ ;
        key_it_map_t::iterator found = _dict.find(key); 
        if (found  == _dict.end())
        {
            //not found ;
            _list.push_front(key);
            pos_cnt hit ;
            hit.pos     =  _list.begin();
            hit.cnt     =  1;  
            _dict[key]  =  hit;
            remove_tail();
            LOG_DEBUG("first key: " <<  key << " cnt: " << cnt ) ;
            return false;
        }
        else
        {
            //found ;
            _cached ++;
            pos_cnt  hit =  found->second ;
            _list.erase(hit.pos);
            _list.push_front(key);
            hit.pos = _list.begin();
            hit.cnt ++ ;
            _dict[key] = hit;
            LOG_DEBUG("cached key: " <<  key << " cnt: " << cnt ) ;
            return   hit.cnt >= cnt ;
        }

    }/*}}}*/
    inline void  empty_head(size_t cnt = 1 )
    {
        for ( size_t i =0 ;  i< cnt  ; ++i)
        {
            _list.push_front("");
        }
    }
    inline void  remove_tail(size_t cnt = 1 )
    {

        if (_list.size() <  _maxsize) return ;
        if (_list.size() <= 0  ) return ;
        for ( size_t i =0 ;  i< cnt  ; ++i)
        {
            if (_list.size() <= 0  ) break;
            string earse_key = _list.back();
            _list.pop_back();
            if (earse_key != "")
            {
                _dict.erase(earse_key);
            }
        }
    }
    float ratio()
    {
        return (float)_cached / (float)_access  * 100 ;
    }
    str_list_t                 _list;
    key_it_map_t  _dict ;
    size_t                       _maxsize;
    size_t                       _maxsec;
    size_t                       _access;
    size_t                       _cached;
};

lru::lru (size_t maxsize, size_t maxsec):_impl(new lru_impl(maxsize,maxsec)){}
lru::~lru ()
{
    delete _impl;
}
bool lru::cache(const char * key, size_t cnt)
{
    return _impl->cache(key,cnt);
}

float lru::ratio()
{
    return _impl->ratio();
}
void lru::time_expire()
{
    return _impl->time_expire();
}
