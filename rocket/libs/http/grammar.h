#ifndef __GRAMMAR__HPP__ 
#define __GRAMMAR__HPP__ 
#include <boost/config/warning_disable.hpp>
#define BOOST_SPIRIT_USE_PHOENIX_V3
//#define BOOST_SPIRIT_UNICODE 
#include "http/http.h"
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_standard_wide.hpp>
#include <boost/spirit/include/qi_parse.hpp>
#include <boost/phoenix.hpp>

namespace pylon { 
    namespace http {

namespace qi        = boost::spirit::qi  ;
namespace ascii     = boost::spirit::ascii ;
namespace encoding  = boost::spirit::ascii ;
namespace px        = boost::phoenix ;
namespace fu        = boost::fusion ;
//typedef std::string::const_iterator  x_iterator ;
typedef const char*  x_iterator ;
struct http_grammar : qi::grammar<x_iterator   >
{/*{{{*/

    http_grammar();
    void bind_header(pylon::http::request& requ);
    void bind_header(pylon::http::response& resp);


    qi::rule<x_iterator,std::string() > _url ;
    qi::rule<x_iterator,std::string() > _nospace_str ;
    qi::rule<x_iterator,std::string() > _str ;
    qi::rule<x_iterator>                _END ;
    qi::rule<x_iterator>                _CRLF ;
    qi::rule<x_iterator>                _SPACE;
    qi::rule<x_iterator>                _header_sep;
    qi::rule<x_iterator,std::string() > _header_name ;
    qi::rule<x_iterator,std::string() > _header_value;
    qi::rule<x_iterator>                _header_line ;
    qi::rule<x_iterator>                _headers     ;
    qi::rule<x_iterator>                _chunk;
    qi::rule<x_iterator,std::string() > _ver ;
    qi::rule<x_iterator,uint(),encoding::space_type >  _cache_ctrl;
    qi::rule<x_iterator,uint() >       _chunk_header;

    //request 
    qi::rule<x_iterator,std::string() > _method ;
    qi::rule<x_iterator,std::string() > _method_list ;

    
    qi::rule<x_iterator> _requ_line ;
    qi::rule<x_iterator> _requ_header ;

    //response 
    qi::rule<x_iterator ,int() >         _status ;
    qi::rule<x_iterator,std::string() >  _reason ;
    qi::rule<x_iterator>                 _resp_line ;
    qi::rule<x_iterator>                 _resp_header ;

};
}}
#endif 
