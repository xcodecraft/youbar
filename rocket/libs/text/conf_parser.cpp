#include <boost/config/warning_disable.hpp>
#define BOOST_SPIRIT_USE_PHOENIX_V3
#define BOOST_SPIRIT_UNICODE 
#include "text/conf_core.h"
#include "conf_grammar.h"
#include "utf8.h"


using namespace std;

namespace  pylon { namespace rigger {

    struct parser::impl
    {/*{{{*/

        bool parse(std::wstring c,conf::sptr& n )
        {
            std::wstring::const_iterator iter = c.begin();
            std::wstring::const_iterator end  = c.end();
            bool r = qi::phrase_parse(iter,end,_grammar,encoding::space,n);
            return r ;
        }


        bool parse_utf8(std::string c,conf::sptr& conf) 
        { 
            std::wstring  wc ;
            utf8::utf8to16(c.begin(), c.end(), back_inserter(wc)); 
            return parse(wc,conf); 
        } 

        conf_grammar   _grammar ;
    };/*}}}*/

    parser::parser():_pimpl(new parser::impl)
    { }
    parser*  parser::ins()
    {
        static  s_parser;
        return &s_parser;
    }
    bool parser::parse(std::wstring c , conf::sptr& conf)
    {
        return ins()->_pimpl->parse(c,conf);
    }

    bool parser::parse_utf8(std::string c , conf::sptr& conf) 
    { 
        return ins()->_pimpl->parse_utf8(c,conf); 
    } 

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
    conf*		conf::xpath(const std::wstring& path)
    {
        path_arr_t  path_arr;
        bool r = xpath_split(path,path_arr);

        path_arr_t  search_path = path_arr ;
        if (r ) 
        {
            conf*  found =  NULL;
            conf*  host = this;
            while(true)
            {
                found = host->xpath_impl(search_path);
                if(found) break;
                search_path = path_arr ;
                host =  host->holder();
                if(host == NULL)  break;
            }
            return found;
        }
        return NULL;
    }

    bool conf_space::get_val(const std::wstring& key,conf::value_type& val )
    {
        path_arr_t  path_arr;
        bool r = xpath_split(key,path_arr);

        path_arr_t  search_path = path_arr ;
        if (r ) 
        {
            conf_space*  host=this;
            while (true )
            {
                if(host->get_val_impl(search_path,val)  ) return true ;

                if( !host->get_host(host))  return false ;
                search_path = path_arr;
            }
        }
        return false ;
    }

    namespace exp_proc {
        struct use_strvar
        {
            typedef std::wstring result_type;
            template <typename Arg1, typename Arg2,typename Arg3>
                std::wstring operator()(Arg1 arg1,Arg1 arg2, Arg2 arg3 ,Arg3 arg4) const
                { 
                    conf_space*  space	= arg3;
                    pylon::logger* l	= arg4;
                    conf::value_type found ;
                    if(space->get_val(arg2,found))
                    {
                        return   arg1 +  boost::get<std::wstring>(found) ;
                    }
                    else
                    {
                        WLOG_INFO5(l) << L"${"  << arg2 << L"} not found value" ;
                        return arg1 + wstring(L"__UNFOUND[" ) + arg2 + wstring(L"]__") ;
                    }
                }
        };
    }

    conf_value::vptr parser::exp_cacul(conf::value_type& v, conf_space* space,pylon::logger* l)
    {
        static conf_grammar   s_grammar;
        conf_value::vptr result ;
        std::wstring* c = boost::get<std::wstring>(&v);
        if (c != NULL)
        {
            s_grammar.var_cacul_init<exp_proc::use_strvar>(space,l);
            s_grammar.var_cacul(*c,result);
        }
        return result ;
    }


}
}
