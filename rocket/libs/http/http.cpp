#include "http/http.h"
#include <boost/foreach.hpp>
#include <map>
#include <list>
#include "log/log_sysl.h"
#include "mem_ctrl.h"
#include <boost/algorithm/string.hpp>

using namespace std;
//using namespace boost;

namespace pylon { 
    namespace http {


        struct header_t
        {
            header_t(){};
            header_t(const string& n, const string&v):name(n),value(v){}
            string name;
            string value;
        };
        typedef boost::shared_ptr<char> char_sptr;
        typedef std::pair<const std::string, header_t >  head_pair_t;
        typedef std::map <const std::string, header_t >  head_map_t;


        struct impl_base  : alloc_able 
        {/*{{{*/
            typedef std::list<buffer_t>  buffer_list;
            head_map_t      _head_map;
            buffer_list     _chunks;
            size_t          _chunk_total_len;
            logger*         _logger;

            impl_base():_chunk_total_len(0)
            {
                _logger  =log_kit::log_ins("http");
            }

            void append_chunk(const buffer_t& chunk)
            {
                _chunk_total_len += chunk.used_size() ;
                _chunks.push_back(chunk);
            }


            bool get_header(const std::string& name, std::string& value )
            {
                string key = name ;
                boost::to_upper(key);
                head_map_t::iterator found  = _head_map.find(key) ;
                if( found != _head_map.end())
                {
                    value = found->second.value ;
                    return true ;
                }
                return false ;
            }
            void set_header(const std::string& name, const std::string& value )
            {
                string key = name ;
                boost::to_upper(key);
                LOG_DEBUG_S(_logger) << "header [" << name  << ": " << value  << "]" ;
                _head_map[key] = header_t(name,value);
            }
            ostream& operator <<( std::ostream& out)
            {
                BOOST_FOREACH(head_pair_t pair , _head_map)
                {
                    out << pair.second.name << ": " << pair.second.value << "\r\n"  ;
                } 
                if (_head_map.size() == 0)  cout << "\r\n";
                return out;
            }
        };/*}}}*/
        struct request::impl : public impl_base  ,public mem_pool<request::impl> 
        {/*{{{*/
            impl(request* h):_host(h){}
            request* _host;

        };/*}}}*/
        request::request():_pimpl(new request::impl(this)){}
        std::ostream& operator <<( std::ostream& out,const request& req)
        {/*{{{*/
            out << req.method  << " " << req.url  <<   " " << req.version  << "\r\n" ;
            req._pimpl->operator<<(out);
            out <<"\r\n";
            return out;
        }/*}}}*/
        bool request::header_check(parser* cur_parser)
        {/*{{{*/
            return true;
        }/*}}}*/

        void request::use_get(const char* host , const char* u)
        {/*{{{*/
            method = "GET";
            url    = u;
            version = "HTTP/1.1";
            set_header("Host",host);
        }/*}}}*/
#define CLS request 
        IMPL_METHOD2(bool,get_header,const std::string& , std::string&)  ;
        IMPL_METHOD2(void,set_header,const std::string& , const std::string&)  ;
        IMPL_METHOD1(buffer_t ,alloc_buffer,size_t)  ;



        struct response::impl : public impl_base , public mem_pool<response::impl> 
        {/*{{{*/

            typedef boost::shared_ptr<buffer_t>  buffer_t_sptr ;
            buffer_t_sptr _body;
            int           _cache_sec ;
            response*     _host;
            impl(response* h ):impl_base(),_cache_sec(0),_host(h){}
            buffer_t body()
            {
                if(_body)
                    return *(_body);
                return buffer_t();
            }
            void set_body(const buffer_t& data)
            {
                _body.reset(new buffer_t(data));
            }
            int cache_sec()
            {
                return _cache_sec ;
            }
            void parse_cache_control(parser* p)
            {
                string val;
                if (get_header("Cache-control",val))
                {
                    uint sec;
                    if(p->cache_ctrl(val,sec)) _cache_sec = sec; 
                }

            }

        };/*}}}*/
        response::response():
            content_length(0),trans_encoding(te_none),content_encoding(ce_none),_pimpl(new response::impl(this)){}
        std::ostream& operator <<( std::ostream& out,const response& res)
        {/*{{{*/
            out << res.version << " " << res.status <<   " " << res.reason << "\r\n" ;
            res._pimpl->operator<<(out);
            return out;
        }/*}}}*/
        void response::decode()
        {/*{{{*/
            if (trans_encoding == chunked )
            {
                buffer_t total_chunk = alloc_buffer(_pimpl->_chunk_total_len +1 );    
                char* data_beg = total_chunk.begin();
                BOOST_FOREACH(buffer_t chunk , _pimpl->_chunks)
                {
                    memcpy(data_beg,chunk.begin(),chunk.used_size());
                    data_beg += chunk.used_size() ;
                }
                total_chunk.mark_used( _pimpl->_chunk_total_len);
                if(content_encoding == ce_none)
                {
                    set_body(total_chunk);
                }
            }

        }/*}}}*/

        buffer_t  response::extend_buffer(buffer_t ori,size_t extend_len)
        {
            size_t buf_len = ori.used_size() + extend_len ;
            buffer_t new_buf= this->alloc_buffer(buf_len);
            new_buf.copy(ori);
            return new_buf ;
        }
#define UNIMPL_HEADER(NAME,VAL)  LOG_ERROR_S(_pimpl->_logger) << "unimplment  header " << NAME  << "[" << VAL <<"]";  return false ;
        bool response::header_check(parser* cur_parser)
        {/*{{{*/
            if  ((status > 100  && status <200 ) ||
                    status == 204  ||
                    status == 304 
                )
            {
                content_length = 0;
            }
            string encoding ;
            if (_pimpl->get_header("Transfer-encoding",encoding))
            {
                if( encoding == "chunked" )
                {
                    trans_encoding = chunked;
                    content_length = 0;
                }
                else
                {
                    UNIMPL_HEADER("Transfer-encoding",encoding);
                }
            }
            string clen;
            if (_pimpl->get_header("Content-length",clen))
            {
                content_length = atoi( clen.c_str());
            }
            string ce;
            if (_pimpl->get_header("Content-encoding",ce))
            {
                if (ce == "gzip") {
                    content_encoding = gzip;
                }
                else if (ce == "compress") {
                    content_encoding = compress;
                    UNIMPL_HEADER("Content-encoding",ce);
                }
                else if (ce == "deflate") {
                    content_encoding = deflate;
                    UNIMPL_HEADER("Content-encoding",ce);
                }
                else if (ce == "identity") { 
                    content_encoding = identity;
                }
                else {
                    UNIMPL_HEADER("Content-encoding",ce);
                }
            }
            string value;
            if (_pimpl->get_header("Trailer",value))
            {

                UNIMPL_HEADER("Trailer",value);
            }
            _pimpl->parse_cache_control(cur_parser);
            return true;

        }/*}}}*/


#undef  CLS
#define CLS response 
        IMPL_METHOD2(bool,get_header,const std::string& , std::string&)  ;
        IMPL_METHOD2(void,set_header,const std::string& , const std::string&)  ;
        IMPL_METHOD1(buffer_t ,alloc_buffer,size_t)  ;
        IMPL_METHOD(buffer_t,body);
        IMPL_METHOD(int ,cache_sec);
        IMPL_METHOD1(void,set_body,const buffer_t&) ;
        IMPL_METHOD1(void,append_chunk,const buffer_t&) ;

    }
}
