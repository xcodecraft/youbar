#include <boost/config/warning_disable.hpp>
#define BOOST_SPIRIT_USE_PHOENIX_V3
#define BOOST_SPIRIT_UNICODE 
#include "text/rigger.h"
#include "utf8.h"
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_standard_wide.hpp>
#include <boost/spirit/include/qi_parse.hpp>
#include <boost/phoenix.hpp>
#include <boost/foreach.hpp>

using namespace std;
namespace qi        = boost::spirit::qi  ;
namespace ascii     = boost::spirit::ascii ;

/** 
 * @brief encoding 的定义,方便切换编码实现.
 */
namespace encoding  = boost::spirit::unicode;
namespace px        = boost::phoenix ;
namespace fu        = boost::fusion ;
typedef std::wstring  xstring ;

namespace  pylon { namespace rigger {

//    struct parser::impl
//    {/*{{{*/

//    };/*}}}*/


    bool parser::xpath_split(const std::wstring& path, conf::path_arr_t& arr)
    {
        using namespace qi::labels;
        typedef std::wstring::const_iterator  wstrc_iterator;
        wstrc_iterator  iter = path.begin();
        wstrc_iterator	end  = path.end();
        qi::rule<wstrc_iterator,conf::path_arr_t(),encoding::space_type> xpath ;
        qi::rule<wstrc_iterator,std::wstring(),encoding::space_type> obj_key ;
        qi::rule<wstrc_iterator,unsigned int(),encoding::space_type> arr_key ;

        obj_key = +(qi::unicode::char_ - L'.')[ _val += _1 ] ;
        xpath	=  (obj_key [ px::push_back(_val,_1)] ) % L'.' ;
        bool r = qi::phrase_parse(iter,end,xpath,encoding::space,arr);
        return r ;
    }


}
}
