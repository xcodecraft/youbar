
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <list>
#include "as_timer.h"
#include <boost/bind.hpp>
#include "logger.h"
#include "lru.h"

using namespace std;
using namespace boost;

typedef std::list<event::sptr>              event_list_t ;
typedef std::map<std::string,event_list_t> event_maplist_t ;

class method_base 
{/*{{{*/
    protected:
        method_base(boost::asio::io_service& io,const char *name ,reducer_method::sptr  next ,size_t maxcnt , size_t maxsec)
            :_name(name),_maxcnt(maxcnt),_maxsec(maxsec),_next(next),_timer(io), _stoped(false)
        {
            LOG_DEBUG("obj:" << (long)this  << " event: " << name << " method obj create ");
        }
        void stop()
        {
            LOG_DEBUG("obj:" << (long)this << " event: " << this->_name << " proc stop! ");
            _stoped = true;
            _timer.cancel();
        }
        ~method_base()
        {
            LOG_DEBUG("obj:" << (long)this << " event: " << this->_name << " method obj delete ");
        }
        const char*     _name;
        size_t          _maxcnt;
        size_t          _maxsec;
//        outer::sptr     _out ;
        reducer_method::sptr _next;
        time_t_timer   _timer;
        bool           _stoped ; 

};/*}}}*/


class out_method  : public reducer_method 
{
    public :
        out_method(outer::sptr o):_out(o) { }
        void proc(event::sptr e )
        {
            LOG_DEBUG("classname: " <<typeid(*_out).name());
            _out->out(*e);
        }
        void stop() { }
        outer::sptr _out;
        static reducer_method::sptr pack(outer::sptr o)
        {
            return reducer_method::sptr(new out_method(o));
        }
};
class pack_method : public reducer_method , public  method_base
{/*{{{*/
    public:
        pack_method(boost::asio::io_service& io,const char *name ,reducer_method::sptr  next ,size_t maxcnt , size_t maxsec)
            :method_base(io,name,next,maxcnt,maxsec)
        {
            LOG_DEBUG("create proc pack  event: " << name );
            time_do();
        }
        ~pack_method()
        {
            out_event();
        }
        void proc(event::sptr e )
        {
            LOG_DEBUG(e->name);
            _packs.push_back(e);
            if (_packs.size() >= _maxcnt)
            {
                out_event();
            }

        }
        void stop()
        {
            method_base::stop();
        }
        void time_do()
        {
            LOG_DEBUG("obj: " << (long) this << " pkg size: " << _packs.size() );
            if( _packs.size() > 0 )
            {
                LOG_DEBUG( " time out ! pkg_size: "   << _packs.size() );
                out_event();
            }
            if(_stoped) return ;
            _timer.expires_from_now(_maxsec);
            _timer.async_wait(boost::bind(&pack_method::time_do,this));
        }
        void out_event()
        {

//            event::sptr pack(new event);
            _event.reset(new event);
            _event->name = _name;
            _event->cnt  = _packs.size();
            _event->proc += " | pack";
            string data ; 
            for(event_list_t::iterator it = _packs.begin(); it != _packs.end(); it++)
            {
                if (it == _packs.begin())
                {
                    data += (*it)->data ;
                }
                else
                {
                    data  +=  "," + (*it)->data  ;
                }

            }
            _event->data = "[ " + data + " ]";
            _packs.clear();
//            _out->out(*_event);
            _next->proc(_event);
            LOG_DEBUG(_event->data);
        }
    private:
        event_list_t _packs;
        event::sptr  _event;

};/*}}}*/
class empty_method :  public reducer_method
{/*{{{*/
    public :
        empty_method(outer::sptr  o) :_out(o)
    { }
        void proc(event::sptr e )
        {
            LOG_DEBUG("empty_method out: " <<typeid(*_out).name());
            _out->out(*e);

        }
        void stop(){}
    private:
        outer::sptr  _out ;
};/*}}}*/


class uniq_method : public reducer_method, public method_base
{/*{{{*/
    public :
        uniq_method(boost::asio::io_service& io,const char *name ,reducer_method::sptr  next  ,size_t maxcnt , size_t maxsec)
            :method_base(io,name,next,maxcnt,maxsec),_lru(maxcnt,maxsec)
        {
            LOG_DEBUG("create proc uniq event: " << name );
            time_do();
        }
        void proc(event::sptr e )
        {
            if (!_lru.cache(e->key.c_str(),1))
            {
                out_event(e);
            }
            else
            {
            }
        }
        void stop()
        {
            method_base::stop();
        }

        void time_do()
        {
            LOG_DEBUG("obj: " << (long)this << " name: " <<  _name <<  " uniq ratio : " << _lru.ratio()  << "%" );
            _lru.time_expire();
            if(_stoped) return ;
            _timer.expires_from_now(1);
            _timer.async_wait(boost::bind(&uniq_method::time_do,this));
        }
        void out_event(event::sptr e)
        {
            e->proc += " | uniq";
            _next->proc(e);
        }
    private:
        lru             _lru;
};/*}}}*/

class ignore_method : public reducer_method
{
    void proc(event::sptr e ){}
    void stop(){}
};
