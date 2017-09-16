#include <boost/config/warning_disable.hpp>
#define BOOST_SPIRIT_USE_PHOENIX_V3
#define BOOST_SPIRIT_UNICODE 
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/support_standard_wide.hpp>
#include <boost/spirit/include/karma.hpp>
#include <boost/spirit/include/qi_parse.hpp>
#include <boost/phoenix.hpp>
#include <boost/foreach.hpp>
#include <boost/fusion/include/std_pair.hpp>
#include "text/text.h"
#include "utf8.h"
#include "text/json_core.h"


#define SPTR_NODE_1(X,A)   px::construct<json::node::sptr>( px::new_< X >( A ) )  
#define SPTR_NODE(X)   px::construct<json::node::sptr>( px::new_< X >(  ) )  
#define LEAF_SPTR   _val=px::construct<json::node::sptr>( px::new_< json::node_value >( _1  ) )  

#define IMPL_OF(X)    template<> struct ImplOf<X> : public boost::shared_ptr<ImplOf<X> > 

namespace qi        = boost::spirit::qi  ;
namespace ascii     = boost::spirit::ascii ;

/** 
* @brief encoding 的定义,方便切换编码实现.
*/
namespace encoding  = boost::spirit::unicode;
namespace px        = boost::phoenix ;
typedef std::wstring  xstring ;

namespace  pylon { namespace json {
	/** 
	* @brief  定义json 的语法　分析器
	* @param  iterator  
	* @param  node::sptr()  处理结查为 node::sptr;
	* @param  encoding::space_type 
	* 
	*/
	template < typename iterator>
	struct json_grammar : qi::grammar<iterator , json::node::sptr() ,encoding::space_type   >
	{/*{{{*/
		json_grammar() : json_grammar::base_type(object)
		{/*{{{*/
			using namespace qi::labels;
			using qi::unicode::char_ ;
			using px::push_back ;
			using px::ref ;
			//                escape.add(L"\\\"",L"\"");
			// object 由 map 组成
			object =   map [ _val = SPTR_NODE_1(json::node_object, _1 )] ;
			// map 的_val 是　node::map_t 对象　, pair 对象通过 map::insert　加入map对象
			// px::insert(_val,_1) 是能过phoenix库　实现lazy调用
			map =   L'{'  >>  pair[ px::insert(_val,_1)] % L','  >>  '}' ;
			// node::pair_t 通过 fusion 进行自动处理
			pair=   ( string  >> qi::omit[ qi::char_(L':') ] >> value )  ;
			array  =   L'['  >>  (   leaf [ px::push_back(_val,_1) ]  ) % L','  >>   L']' ;
			value  =   (leaf | object | j_null )  [ _val = _1 ]  |  array   [ _val = SPTR_NODE_1(json::node_arr,_1 )]  ;
			leaf    =  qi::int_   [ LEAF_SPTR  ]  
			| string   [ LEAF_SPTR ]    
			| qi::float_ [ LEAF_SPTR ]    
			// 相同类型，才可以合并处理
			| ( qi::hex | qi::oct | qi::bin ) [ LEAF_SPTR ]    
			;

			j_bool  =  qi::string(L"true") [ _val =  true ]  | qi::string(L"false")  [ _val =  false  ]  ; 
			j_null  =  qi::string(L"null") [ _val =  SPTR_NODE(json::node_null)  ]   ;
			escape =   L'\\' >>  qi::unicode::char_  ;
			string  =   qi::omit[ char_('"')] 
			>>  qi::lexeme[   *(  escape  [ _val += _1 ]| (qi::unicode::char_  - L'"' ) [ _val += _1 ] )  ] 
			>>  qi::omit[ char_('"')]  ;

		}/*}}}*/

		//            qi::symbols<wchar_t const, wchar_t* > escape ;
		qi::rule<iterator,json::node::sptr()      ,encoding::space_type> object ;
		qi::rule<iterator,json::node_object::value_type()     ,encoding::space_type> map  ;
		qi::rule<iterator,json::node_object::pair()    ,encoding::space_type> pair ;
		qi::rule<iterator,json::node_arr::value_type()  ,encoding::space_type> array  ;
		qi::rule<iterator,json::node::sptr()      ,encoding::space_type> value  ;
		qi::rule<iterator,json::node::sptr()      ,encoding::space_type> j_null ;
		qi::rule<iterator,json::node::sptr()      ,encoding::space_type> leaf ;
		qi::rule<iterator,std::wstring()          ,encoding::space_type> string ;
		qi::rule<iterator,wchar_t()                                    > escape ;
		qi::rule<iterator,bool()                  ,encoding::space_type> j_bool ;
	};/*}}}*/



	struct parser_impl
	{/*{{{*/


		bool parse(std::wstring c,json::node::sptr& n )
		{
			std::wstring::const_iterator iter = c.begin();
			std::wstring::const_iterator end  = c.end();
			bool r = qi::phrase_parse(iter,end,_grammar,encoding::space,n);
			return r ;
		}


		bool parse_utf8(std::string c,json::node::sptr& node) 
		{ 
			std::wstring  wc ;
			utf8::utf8to16(c.begin(), c.end(), back_inserter(wc)); 
			return parse(wc,node); 
		} 
		json_grammar<std::wstring::const_iterator>   _grammar ;
	};/*}}}*/

	//define PIMPL_IMPL(CLS,FUN1) 
	parser::parser():_pimpl(new parser_impl)
	{ }
	bool parser::parse(std::wstring c , json::node::sptr& node)
	{
		return _pimpl->parse(c,node);
	}

	bool parser::parse_utf8(std::string c , json::node::sptr& node) 
	{ 
		return _pimpl->parse_utf8(c,node); 
	} 

	node::sptr node::xpath(const std::wstring& path)
	{/*{{{*/
		using namespace qi::labels;
		typedef std::wstring::const_iterator  wstrc_iterator;
		wstrc_iterator  iter = path.begin();
		wstrc_iterator end  = path.end();
		qi::rule<wstrc_iterator,node::path_arr_t(),encoding::space_type> xpath ;
		qi::rule<wstrc_iterator,std::wstring(),encoding::space_type> obj_key ;
		qi::rule<wstrc_iterator,unsigned int(),encoding::space_type> arr_key ;

		obj_key = +(qi::unicode::char_ - L'/')[ _val += _1 ] ;
		arr_key = L"[" >> qi::uint_  >>  L']'  ;
		xpath  =   L"/" >> (arr_key [ px::push_back(_val,_1) ] | obj_key [ px::push_back(_val,_1)] ) % L'/' ;
		path_arr_t  path_arr;
		bool r = qi::phrase_parse(iter,end,xpath,encoding::space,path_arr);

		std::wcout <<   std::endl  << " xpath: " ;
		if (r ) 
		{
			return this->xpath_impl(path_arr);
		}
		return node::sptr();
	}/*}}}*/


}
}
