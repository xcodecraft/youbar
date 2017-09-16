#include "log/log_sysl.h"
#include <syslog.h>
#include <sstream>
#include <map>
#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
using namespace std;
static string s_prj("unknow") ;

void log2sys(log_kit::level_t lev,const char* name, const string& msg)
{

    int level = 0;
    switch(lev)
    {
        case log_kit::debug:
            level= LOG_DEBUG;
            break;
        case log_kit::info:
            level= LOG_INFO;
            break;
        case log_kit::error:
            level= LOG_ERR;
            break;
        default:
            return ;
    }
    openlog(name ,LOG_PID,LOG_LOCAL6);
    syslog(level,msg.c_str());
    closelog();
}


struct logger::impl 
{/*{{{*/
    typedef std::basic_string<char >               string_t ;
    typedef std::basic_stringstream<char >         stream_t ;
    typedef  boost::shared_ptr<stream_t>           stream_sptr;

    impl(const char * name ):_stream(new stream_t ),_name(name),_level(log_kit::undef){}
    basic_ostream<char > * stream(log_kit::level_t lev)
    {
        (*_stream) << "tag[" ;
        if (_all_tag.size() != 0)
        {
            (*_stream) <<  _all_tag   ;

            if (_tag.size() != 0)
                (*_stream) <<   "," << _tag  ;
        }
        else
        {
            if (_tag.size() != 0)
                (*_stream) <<   _tag   ;
        }
        (*_stream) <<    "] ";
        return _stream.get();
    }
    void   log_stream()
    {
        string_t msg= _stream->str();

        log_kit::level_t cur_level = _level == log_kit::undef ? _all_level: _level ;
        string fullname =  s_prj  + "/" + _name ;

        log2sys(cur_level,fullname.c_str(),msg);
        if (_extra == log_kit::console || _all_extra == log_kit::console )
        {
            std::cout << fullname << " : "  << msg << endl;
        }
//        else if (_extra == log_kit::html  || _all_extra == log_kit::html )
//        {
//            cout << fullname << " : "  << msg << "<br>" << endl;
//        }
        
        _stream.reset(new stream_t);
    }
    inline void level(log_kit::level_t level)
    {
        _level      = level; 
    }
    inline void tag( const char* tag )
    {
        _tag = tag ;
    }
    inline void out(log_kit::outer_t extra )
    {
        _extra      = extra ;
    }
    stream_sptr   _stream;
    string        _name ;
    string        _tag  ;
    log_kit::level_t _level;
    log_kit::outer_t _extra;
    static   log_kit::level_t _all_level;
    static   log_kit::outer_t _all_extra;
    static   string           _all_tag;
};/*}}}*/
log_kit::level_t logger::impl::_all_level = log_kit::error;
log_kit::outer_t logger::impl::_all_extra = log_kit::none;
string           logger::impl::_all_tag   ;

struct log_kit::impl  
{
    public :
        typedef  std::map<string  ,logger*>   logger_map_t;
        typedef  std::pair<string ,logger*>   logger_pair_t;
        impl(){}
        logger* log_ins(const char* name)
        {   
            logger_map_t::iterator found =  _log_dict.find(name);
            if (found != _log_dict.end())
            {
                return found->second;
            }
            else
            {
                logger* one= new logger(name);
                _log_dict[name] = one ;
                return one ;
            }
        }

        void clear()
        {
            BOOST_FOREACH(logger_pair_t kv , _log_dict)
            {
                delete kv.second ;
            }
            _log_dict.clear();
        }
        logger_map_t _log_dict;
};


logger::logger(const char* name): _pimpl(new impl(name))
{}
logger::~logger()
{
    delete _pimpl;
}
ostream* logger::stream(log_kit::level_t lev)
{
    return _pimpl->stream(lev);
}
void   logger::log_stream()
{
    _pimpl->log_stream();
}
void logger::level(log_kit::level_t l)
{
    _pimpl->level(l);
} 
void logger::tag(const char* tag )
{
    _pimpl->tag(tag);
} 
void logger::out(log_kit::outer_t extra)
{
    _pimpl->out(extra);
} 

bool logger::need_log(int level) 
{ 
    if (level >= logger::impl::_all_level ) return true ;
    if (level >= _pimpl->_level )  return  true ;
    return false;
}
void logger::debug(const char* msg)
{
    if(need_log(log_kit::debug)) 
    {
        *stream(log_kit::debug) <<"[debug] " <<  msg ;
        log_stream();
    }
}
void logger::info(const char* msg)
{
    if(need_log(log_kit::info)) 
    {
        *stream(log_kit::info) <<"[info] " <<  msg ;
        log_stream();
    }
}

void logger::warn(const char* msg)
{
    if(need_log(log_kit::warn)) 
    {
        *stream(log_kit::warn) <<"[warn] " <<  msg ;
        log_stream();
    }
}

void logger::error(const char* msg)
{
    if(need_log(log_kit::error)) 
    {
        *stream(log_kit::error) <<"[error] " <<  msg ;
        log_stream();
    }
}

log_kit::impl* impl_ins()
{
    static log_kit::impl   s_impl_ins;
    return &s_impl_ins ;

}

void log_kit::init(const char * prj,const char* tag , log_kit::level_t l)
{
    s_prj = prj;
    logger::impl::_all_tag   = tag ;
    logger::impl::_all_level = l ;
}
logger* log_kit::log_ins(const char* name)
{
    return impl_ins()->log_ins(name);
}

void  log_kit::level(const char* name , log_kit::level_t l)
{
    log_ins(name)->level(l);
}

void  log_kit::tag(const char* name , const char* tag )
{
    log_ins(name)->tag(tag);
}
void  log_kit::out(const char* name , log_kit::outer_t extra_out)
{
    log_ins(name)->out(extra_out);
}

void log_kit::clear()
{
    impl_ins()->clear();
}
