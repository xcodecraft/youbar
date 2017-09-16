#include "../utf8.h"
#include "text/xjson.h"
#define BOOST_TEST_DYN_LINK 
#define BOOST_SPIRIT_USE_PHOENIX_V3 
#define BOOST_SPIRIT_UNICODE 
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_standard_wide.hpp>
#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/qi_parse.hpp>
#include <boost/phoenix.hpp>
#include <boost/fusion/include/std_pair.hpp>
//#define BOOST_TEST_MODULE TEST_SPIRIT
#include <boost/test/unit_test.hpp>
using namespace pylon::json ;
using namespace std;
namespace qi        = boost::spirit::qi  ;
namespace ascii     = boost::spirit::ascii ;
namespace encoding  = boost::spirit::unicode;
namespace phoenix   = boost::phoenix ;
namespace px        = boost::phoenix ;
BOOST_AUTO_TEST_CASE( parse_chinese)
{
        using namespace qi::labels;
        std::wstring test=L"中国" ;
        std::wstring found ;
        qi::rule<wstring::iterator,wstring(),encoding::space_type> unicode_string;
        unicode_string = * qi::unicode::char_ [_val += _1 ]  ; 
        if( qi::phrase_parse(test.begin(),test.end(),
                    unicode_string,
                    encoding::space,
                    found)
          )
        {
            BOOST_CHECK(true);
            std::cout << "OK:" << utf16to8(found) << std::endl  ;
        }
        else
        {
            BOOST_CHECK(false);
        }
}


BOOST_AUTO_TEST_CASE( parse_escape)
{
            using namespace qi::labels;
            typedef std::string::iterator  iterator ;
            qi::rule<iterator ,std::string() , ascii::space_type     > r_string ;
            qi::rule<iterator, char() > es_char;
            es_char  = '\\' >> qi::char_("\"");
            r_string =  qi::char_('"')  >>  +( es_char [ _val += _1 ]    |  (qi::char_ - "\"")  [ _val += _1 ]  ) >> qi::char_('"') ;

            std::string test="\"china\\\"cs\"" ;
            std::string found ;
            if( qi::phrase_parse(test.begin(),test.end(),
                        r_string,
                        ascii::space,
                        found)
              )
        {
            BOOST_CHECK(true);
            std::cout << "OK:" << found << std::endl  ;
            return ;
        }
        BOOST_CHECK(false);

}
