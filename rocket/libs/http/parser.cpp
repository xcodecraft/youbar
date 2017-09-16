#include "http/http.h"
#include "grammar.h"
#include "log/log_sysl.h"
using namespace std;
using namespace pylon;
using namespace boost;




namespace pylon
{
    namespace http {

        struct parser::impl
        {

            http_grammar _g;
            logger*      _logger;
            parser*      _host;
            impl(parser* host):_host(host)
            {
                _logger  =log_kit::log_ins("http");
            }
            result_t parse_header(buffer_t data ,request&  requ)
            {
                result_t ret;
                _g.bind_header(requ);
                const char* iterator = data.const_begin();
                bool parse_suc      = qi::phrase_parse(iterator,data.const_end(),_g._requ_header,encoding::space  );
                if(parse_suc && requ.header_check(_host))
                {
                    ret.to_end(data.begin_distance(iterator));
                }
                return  ret ;
            }
//            result_t chunk_
            result_t parse_header(buffer_t data, response& resp)
            {/*{{{*/
                result_t ret;
                _g.bind_header(resp);
                const char* iterator = data.const_begin();
                bool parse_suc       = qi::phrase_parse(iterator,data.const_end(),_g._resp_header,encoding::space  );
                if(parse_suc && resp.header_check(_host))
                {
                    ret.to_end(data.begin_distance(iterator));
                }
                return  ret ;
            }/*}}}*/
            result_t parse_body(buffer_t data, response& resp)
            {/*{{{*/
                result_t ret;
                if(data.used_size() < resp.content_length)
                {
                    ret.to_lack(resp.content_length);
                    return ret;
                }
                if(resp.content_length !=0 )
                {
                    //TODO remove memcpy;
                    buffer_t body   = resp.alloc_buffer(resp.content_length);
                    memcpy(body.begin(),data.begin(),resp.content_length);
                    body.mark_used(       resp.content_length);
                    resp.set_body(body);
                    ret.to_end(resp.content_length);
                    return ret;
                }
                ret.to_bad();
//                ret.status      = result_t::BAD;
                return ret;
            }/*}}}*/
            result_t parse_chunk(const buffer_t& data, response& resp)
            {/*{{{*/
                const char*  iterator = data.begin();
                result_t     ret;
                size_t       chunk_size = 1024;
                while(1)
                {
                    if (iterator >=  data.used_end()) {
                        ret.to_lack(chunk_size );
                        return  ret ;
                    }

                    //parse length;
                    uint data_len =0 ;

                    if(!qi::parse(iterator,(const char*)data.used_end(),_g._chunk_header,data_len  ))
                    {
                        LOG_ERROR_S(_logger) << "bad chuck head or unknow"; 
                        ret.to_bad();
                        return  ret ;
                    }
                    //parse data;
                    size_t space_len = data.used_end() - iterator ;
                    // 2 include \r\n;
                    if( ret.check_lack(data_len + 2 ,space_len))
                    {
                        return ret;
                    }

                    if (strncmp(iterator + data_len ,"\r\n",2) !=0 )   
                    {
                        LOG_ERROR_S(_logger) << "bad chuck end CRLF "; 
                        ret.to_bad();
                        return  ret ;
                    }
                    buffer_t chunk_data(data_len,(char*)iterator);
                    chunk_data.mark_used(data_len );
                    resp.append_chunk(chunk_data);
                    iterator += (data_len + 2 );
                    ret.parsed(data.begin_distance(iterator));
                    // is end ?
                    if (strncmp(iterator,"0\r\n",3) ==0 )  {
                        ret.to_end(data.begin_distance(iterator +3 ));
                        return  ret ;
                    }
                }
                ret.to_bad();
                return  ret ;
            }/*}}}*/

            bool  cache_ctrl(const string& str, uint& sec)
            {
                const char* begin = str.c_str();
                const char* end  = begin + str.length();
                return qi::phrase_parse(begin,end,_g._cache_ctrl,encoding::space,sec);
            }
        };
        parser::parser():_pimpl(new parser::impl(this)){}

#define CLS parser
        IMPL_METHOD2(result_t,parse_header,buffer_t,request&) ;
        IMPL_METHOD2(result_t,parse_header,buffer_t,response&) ;
        IMPL_METHOD2(result_t,parse_chunk,buffer_t,response&) ;
        IMPL_METHOD2(result_t,parse_body,buffer_t,response&) ;
        IMPL_METHOD2(bool,cache_ctrl,const std::string&,uint&) ;
    }
}
