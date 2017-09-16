#include "http/http.h"
#include "grammar.h"

using namespace std;
using namespace pylon;
using namespace pylon::http ;
using namespace boost;

namespace pylon { 
    namespace http {


http_grammar::http_grammar() 
    : http_grammar::base_type(_requ_header,"requ_header")
{
    using namespace qi::labels;
    _requ_line     = _method >> _SPACE >>  _url  >> _SPACE  >>  _ver >> _CRLF ;
    _headers        = *(_header_line) ;
    _requ_header    = _requ_line >> _headers >> _CRLF ;
    _nospace_str    = *( (qi::char_ - qi::space ) [ _val += _1 ])  ;
    _str            = *( (qi::char_  )  [ _val += _1 ])  ;
    _CRLF           = qi::omit["\r\n" ]  ;
    _END            = qi::char_("\r") | qi::char_("\n") ;
    _SPACE          = qi::omit[* qi::char_(' ') ]  ;
    _header_name    = *((qi::char_ - ":" ) [ _val += _1 ] ) ;
    _header_value   = *((qi::char_ - _END ) [ _val += _1 ] ) ;
    _header_sep     = qi::omit [qi::string(":") >> -qi::char_(' ') ] ;
    _method_list    = qi::string("GET") | qi::string("POST") | qi::string("HEAD") | qi::string("PUT") | qi::string("DELETE") ;
    _resp_line      = _ver >> _SPACE >> _status >> _SPACE >> _reason >> _CRLF;
    _resp_header    = _resp_line >> _headers >>  _CRLF ;

    _cache_ctrl     = qi::omit[ (qi::string("max-age") |  qi::string("max-stale") )  >> "="  ] >>  qi::uint_ ;
    _chunk_header   = qi::hex >> _CRLF ;

}
void http_grammar::bind_header(pylon::http::request& requ)
{
    using namespace qi::labels;

    _method      = _method_list [ px::ref( requ.method) = _1 ]  ;
    _url         = _nospace_str [ px::ref( requ.url) = _1 ] ;
    _ver         = _nospace_str [ px::ref( requ.version) = _1 ] ;
    _header_line = ( _header_name  >> _header_sep >> _header_value  >> _CRLF  )   [ px::bind(&request::set_header, &requ,_1,_2) ];




}

void http_grammar::bind_header(pylon::http::response& resp)
{
    using namespace qi::labels;


    _status         = qi::uint_ [ px::ref(resp.status) = _1 ] ;
    _reason         = _nospace_str [ px::ref( resp.reason) = _1 ] ;
    _ver            = _nospace_str [ px::ref( resp.version) = _1 ] ;
    _header_line    = ( _header_name  >> _header_sep >> _header_value >> _CRLF )   [ px::bind(&response::set_header, &resp,_1,_2) ];

}
}}

