#ifndef __CONF_GRAMM_HPP__ 
#define __CONF_GRAMM_HPP__ 
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

#define SPTR_UNIT(X)			px::construct<conf::sptr>( px::new_< X >(  ) )  
#define SPTR_UNIT_1(X,A)		px::construct<conf::sptr>( px::new_< X >( A ) )  
#define SPTR_UNIT_2(X,A1,A2)	px::construct<conf::sptr>( px::new_< X >( A1,A2 ) )  
#define SPTR_UNIT_3(X,A1,A2,A3)	px::construct<conf::sptr>( px::new_< X >( A1,A2,A3 ) )  
#define LEAF_SPTR			_val=px::construct<conf::sptr>( px::new_< conf_value >( _1  ) )  
#define NEW_UNIT_VALUE(A1,A2)			px::construct<conf_value::vptr>( px::new_< conf_value >(A1,A2 ) )  
#define NEW_UNIT_OBJ(A)	px::construct<conf_object::optr>( px::new_< conf_object >( A ) )  

namespace  pylon { namespace rigger {


    /** 
     * @brief  定义conf 的语法　分析器
     * @param  iterator  
     * @param  conf::sptr()  处理结查为 conf::sptr;
     * @param  encoding::space_type 
     * 



     module utls {     
         udo { sudo : True }
         conf_tpl(PRJ_ROOT: "" , SRC:"" ) :: res.file_tpl {
                 tpl         :  "${PRJ_ROOT}/conf/options/tpl_${SRC}"
                 dst         :  "${PRJ_ROOT}/conf/used/${SRC}"
         }       
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

//    struct conf_holder
//    {
//        conf_holder():ptr(0){}
//        conf_holder(conf* p):ptr(p){}
//        conf* ptr;
//    };

    typedef conf*  conf_holder ;
    typedef std::wstring::const_iterator  conf_iterator;
    struct conf_grammar : qi::grammar<conf_iterator, conf_holder(),encoding::space_type   >
    {/*{{{*/

        conf_grammar();
//        template <  typename F_STR> 
//            void var_cacul_init(conf_space* space,pylon::logger* l)
//            {

//                using namespace qi::labels;
//                using qi::_1 ;
//                using qi::_2 ;
//                using qi::_val ;
//                px::function<F_STR>   use_f_str ;
//                str_var		= L"${" >> key [ qi::_val = use_f_str(qi::_val,_1,px::val(space),px::val(l)) ] >> L"}" ;
//                no_var		= *(  escape [ qi::_val += _1 ] | ((qi::unicode::char_  - L'$') - L'"'  ) [ qi::_val += _1 ] ) ;
//                str_line  =   *( no_var  >> str_var ) ;
//                var_line  =   str_line [ qi::_val = NEW_UNIT_VALUE(px::val(L""),_1) ]  ;
//            }

//        bool var_cacul(std::wstring c ,   conf_value::vptr & x ) ;

        qi::rule<conf_iterator,conf_holder(),encoding::space_type>			u_module_space ;
        qi::rule<conf_iterator,conf_holder() ,encoding::space_type>			u_module ;
        qi::rule<conf_iterator,conf_holder() ,encoding::space_type>			u_object ;
        qi::rule<conf_iterator,conf::sptr_vector() ,encoding::space_type>	u_items;
        qi::rule<conf_iterator,conf_object::attr(),encoding::space_type>			u_attr ;
        qi::rule<conf_iterator,std::wstring(),encoding::space_type>			    u_args ;
        qi::rule<conf_iterator,conf::value_type(), encoding::space_type>			value  ;
        qi::rule<conf_iterator,std::wstring(),  encoding::space_type>            u_parent ;
        qi::rule<conf_iterator,conf_holder(),encoding::space_type>				u_value  ;
        qi::rule<conf_iterator,wchar_t()           >								escape ;
        qi::rule<conf_iterator,std::wstring(),encoding::space_type>				key ;
        qi::rule<conf_iterator,std::wstring() ,encoding::space_type>				string ;
        qi::rule<conf_iterator,std::wstring()					>				comment ;

        qi::rule<conf_iterator,var_ref(),encoding::space_type>					var ;
        qi::rule<conf_iterator,std::wstring(),encoding::space_type>				str_var ;
        qi::rule<conf_iterator,std::wstring(),encoding::space_type>				no_var ;
        qi::rule<conf_iterator,std::wstring(),encoding::space_type>				str_line ;
        qi::rule<conf_iterator,conf_value::vptr(), encoding::space_type>			var_line;
        template < typename T, typename X> 
            bool test_impl(std::wstring c ,  T rule , X& x )
            {
                std::wstring::const_iterator iter = c.begin();
                std::wstring::const_iterator end  = c.end();
                return qi::phrase_parse(iter,end,rule,encoding::space, x );
            }

        template < typename T, typename X> 
            bool conf_test(std::wstring c ,  T rule , conf**  x )
            {
                std::wstring::const_iterator iter = c.begin();
                std::wstring::const_iterator end  = c.end();
                return qi::phrase_parse(iter,end,rule,encoding::space, x );
            }

        bool parse(std::wstring& c);
//        bool test_object(std::wstring c , conf::sptr& x)  ;
    };/*}}}*/

}
}
#endif
