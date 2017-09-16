#include "http/http.h"
#include <boost/foreach.hpp>
#include <map>
#include <list>
#include "log/log_sysl.h"

using namespace std;
//using namespace boost;
namespace pylon { 
    namespace http {

        buffer_t buffer_t::right(size_t split_len)
        {
            buffer_t buf(_len - split_len ,_begin + split_len   );
            buf._used = _used > split_len ?  _used - split_len : 0;
            return buf;
        }
        void buffer_t::copy(const buffer_t& other)
        {
            memcpy(_begin,other._begin,other._used);
            _used = other._used ;
        }
        void buffer_t::copy_until(const buffer_t& other,const std::string& separator)
        {
            std::string buf(other._begin);
            std::string::size_type index = buf.find(separator);
            if(std::string::npos == index)
            {
                copy(other);
            }
            else
            {
                size_t copy_len = index + 1;
                memcpy(_begin,other._begin,copy_len);
                _used = copy_len ;
            }
        }
        void buffer_t::copy_from(const buffer_t& other,const std::string& separator)
        {
            std::string buf(other._begin);
            std::string::size_type index = buf.find(separator);
            if(std::string::npos == index)
            {
                return;
            }
            else
            {
                size_t skip = index + 1;
                size_t copy_len = other._used - skip;
                memcpy(_begin,other._begin+skip,copy_len);
                _used = copy_len ;
            }
        }
        bool result_t::check_lack(size_t need , size_t have)
        {/*{{{*/
            if( need > have)
            {
                _status = result_t::LACK ;
                _lack_len = need - have;
                return true ;
            }
            return false;
        }/*}}}*/
        bool result_t::is_end()
        {
            return  _status == result_t::END;
        }
        bool result_t::is_bad()
        {
            return  _status == result_t::BAD;
        }
        bool result_t::is_lack()
        {
            return  _status == result_t::LACK;
        }
        void result_t::to_end( size_t parsed) 
        { 
            _parsed_len += parsed;
            _status = result_t::END;
        }
        void result_t::to_lack(size_t lack)  
        { 
            _lack_len   += lack; 
            _status = result_t::LACK;
        }
        void result_t::to_bad() 
        { 
            _status = result_t::BAD;
        }
        void result_t::parsed(size_t parsed)
        {
            _parsed_len += parsed; 
        }


    }

}

