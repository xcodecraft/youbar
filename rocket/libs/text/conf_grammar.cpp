#include "conf_grammar.h"
#include <boost/config/warning_disable.hpp>
#define BOOST_SPIRIT_USE_PHOENIX_V3
#define BOOST_SPIRIT_UNICODE 
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_standard_wide.hpp>
#include <boost/spirit/include/qi_parse.hpp>
#include <boost/phoenix.hpp>
#include <boost/foreach.hpp>
#include "text/rigger.h"






namespace qi        = boost::spirit::qi  ;
namespace ascii     = boost::spirit::ascii ;

/** 
 * @brief encoding 的定义,方便切换编码实现.
 */
namespace encoding  = boost::spirit::unicode;
namespace px        = boost::phoenix ;
namespace fu        = boost::fusion ;
typedef std::wstring  xstring ;



//#define CONF_OBJ2(X,A1,A2)	 px::construct<conf_holder>(px::new_< X >( A1,A2 ) )
#define CONF_OBJ2(X,A1,A2)	 px::new_< X >( A1,A2 ) 


namespace  pylon { namespace rigger {


    /** 
     * @brief  定义conf 的语法　分析器
     * @param  iterator  
     * @param  conf::sptr()  处理结查为 conf::sptr;
     * @param  encoding::space_type 
     * 




     : _env
     {
     undebug     { debug  : 0, log_level  : 1 ,  php_error:      "" }
     debug       { debug  : 1, log_level  : 0 ,  php_error:      "" }
     xx ::debug (undebug) {}

     }

     conf_object ::=  ( key  | ('::' parents ) )  | '( )' | '{'  (conf_value | ',')*  '}'

     key			::= string 
     parent		::=  ( key | ',' )* 
     conf_value	::=  ( key ':' value )
     value			::= string | int  
     string ::=
     int    ::=
     inject :: =  ( conf_value | key ) *

*/



    conf_grammar::conf_grammar() : conf_grammar::base_type(u_module_space,"module_space")
    {/*{{{*/
        using namespace qi::labels;
        using qi::unicode::char_ ;
        using px::push_back ;
        using px::ref ;
        using qi::_1 ;
        using qi::_2 ;
        using qi::_3 ;
        using qi::_4 ;
        using qi::on_error;
        using qi::fail;

        u_module_space    =  *( u_module [ px::bind(module_space::append, _1) ] ) ;
        u_module        =  ( L"module" >>  key >>  u_items ) [ _val = CONF_OBJ2(conf_module,_1,_2) ]  ;
        u_object		=  ( u_attr  >>  u_items  ) [ _val = CONF_OBJ2(conf_object,_1,_2) ]   ;
        u_attr          =  ( key >> -u_args >>  -u_parent   ) ;
        u_items			=  L'{'  >>   (    *( u_value | u_object  | qi::omit[ comment ] )  )  >>  L'}' ;

        u_value			=  ( key  >> L':' >> value >> qi::omit[ -qi::unicode::char_(L',') ])  [ _val = CONF_OBJ2(conf_value,_1,_2) ] ;
        u_parent			=  L"::"  >>     (key )[ _val += _1 ]  % ( char_(L',') [ _val += _1 ]) ;
        value			=  (qi::int_ | var | string ) [ _val = _1 ]  ;
//        comment			=  L'#' >> * (qi::unicode::char_ - L"\n" ) >> L"\n";


        escape =   L'\\' >>  qi::unicode::char_  ;
        string =   qi::omit[ char_('"')]
            >>  qi::lexeme[   *(  escape  [ _val += _1 ]| (qi::unicode::char_  - L'"' ) [ _val += _1 ] )  ] 
            >>  qi::omit[ char_('"')]  ;

        u_args          =   qi::omit[ char_('(')]
            >>  qi::lexeme[   *(  escape  [ _val += _1 ]| (qi::unicode::char_  - L')' ) [ _val += _1 ] )  ] 
            >>  qi::omit[ char_(')')]  ;
        key		=  qi::lexeme[   *(  qi::unicode::alnum | qi::unicode::char_(L'_') | qi::unicode::char_(L'.') )[ _val += _1 ]   ]  ;

        var		= L"${" >> key [ _val = px::construct<var_ref>(_1) ] >> L"}" ;



        // "${abc}/${abc}"
        //            u_object.name("conf_object");
        //            u_value.name("conf_value");


        on_error<fail>
            (
             u_object, 
             std::wcout  << px::val(L"ERROR")   <<  std::endl 
             /*
                std::cout	<< px::val("Error! Expecting ") 
                << _4 << px::val(" here: \"") 
                << px::construct<std::string>(_3, _2)   
                << px::val("\"") << std::endl
                << std::endl
                */
            );

    }/*}}}*/

    bool conf_grammar::parse(std::wstring& c)
    {
        std::wstring::const_iterator iter = c.begin();
        std::wstring::const_iterator end  = c.end();
        conf_holder none=NULL;
        bool r = qi::phrase_parse(iter,end,*this,encoding::space,none);
        return r ;
    }

//    bool conf_grammar::var_cacul(std::wstring c ,   conf_value::vptr & x )
//    {
//        std::wstring::const_iterator iter = c.begin();
//        std::wstring::const_iterator end  = c.end();
//        return qi::phrase_parse(iter,end,var_line,encoding::space, x );
//    }


//    bool conf_grammar::test_object(std::wstring c , conf::sptr& x)  
//    {
//        std::wstringstream buf ;
//        buf << L"_g { " << c << L" } " ;
//        std::wstring content = buf.str();
//        std::wstring::const_iterator iter = content.begin();
//        std::wstring::const_iterator end  = content.end();
//        return qi::phrase_parse(iter,end,u_object,encoding::space, x );
//    }

}
}
