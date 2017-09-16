#ifndef __JSON_CORE__HPP__ 
#define __JSON_CORE__HPP__ 
#include <string>
#include <map>
#include <vector>
#include <list>
#include <boost/variant.hpp>
#include "port.h"
#include "pimpl.h"
#include "utls.h"


namespace  pylon { namespace json
    {

		struct  PYLON_API node  :  smarty_ptr<node>  
        {
            typedef boost::variant<float ,bool,int,unsigned int ,std::wstring, node::sptr > value_type  ;
            typedef boost::variant<unsigned int ,std::wstring  > path_type  ;
            typedef std::list<path_type>              path_arr_t ;
            virtual void to_json(std::wostream& o){} ;
            std::wstring str();
            virtual ~node(){}
            /** 
             * @brief xpath : /key1/[key2]/key3/ 
             * 
             * @param std::wstring 
             * 
             * @return 
             */
            node::sptr xpath(const std::wstring&);
            virtual node::sptr xpath_impl(path_arr_t& key_arr) { return node::sptr() ; }

        };
        struct PYLON_API node_value : public node 
        {

            node::value_type _value ;
            node_value(){}
            node_value(const std::wstring& v ):_value(v) {}
            node_value(int v ):_value(v) {}
            node_value(unsigned int v ):_value(v) {}
            node_value(bool v ):_value(v) {}
            node_value(float v ):_value(v) {}
            virtual void to_json(std::wostream& o) ;
            virtual node::sptr xpath_impl(path_arr_t& key_arr) ;

        };
        struct PYLON_API node_arr  : public node  //, noncopyable
        {
            typedef std::vector<node::sptr>             value_type;
            typedef std::vector<node::sptr>::iterator   iterator ;
            typedef std::vector<node::sptr>::const_iterator   const_iterator ;
			iterator begin();
			const_iterator begin() const ;
			iterator end();
			const_iterator end() const ;
			node::sptr operator[] (unsigned int index) ;
            node_arr( node_arr::value_type & v ) ;
            virtual void to_json(std::wostream& o) ;
            virtual node::sptr xpath_impl(path_arr_t& key_arr) ;
            PIMPL_DEF;
			//friend struct parser_impl ;
			//friend struct json_garammer;
        };
        struct PYLON_API node_object : public node  //  , noncopyable
        {
            typedef std::map<std::wstring, node::sptr>   value_type;
            typedef std::pair<std::wstring, node::sptr>  pair;
            typedef std::map<std::wstring, node::sptr>::iterator   iterator;
            typedef std::map<std::wstring, node::sptr>::const_iterator   const_iterator;

			iterator begin();
			const_iterator begin() const ;
			iterator end();
			const_iterator end() const ;
			node::sptr operator[] (const std::wstring& key) ;
            node_object( node_object::value_type& v ) ;
            void to_json( std::wostream& o) ;
            virtual node::sptr xpath_impl(path_arr_t& key_arr) ;
            PIMPL_DEF;
			//friend struct parser_impl ;
			//friend struct json_garammer;
        };
        struct PYLON_API node_null: public node
        {
            void to_json( std::wostream& o) ;
            virtual node::sptr xpath_impl(path_arr_t& key_arr) ;
        };

		struct parser_impl ;
        class PYLON_API parser  
        {
            public:
                parser();
                bool parse(std::wstring c ,node::sptr& n);
                bool parse_utf8(std::string c ,node::sptr& n);
			private: 
				boost::shared_ptr< parser_impl>  _pimpl;
        };





    }


}
#endif
