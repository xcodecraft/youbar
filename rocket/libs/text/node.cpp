#include <boost/config/warning_disable.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include "utls.h"
#include "text/json.h"
using namespace std;
namespace  pylon { namespace json
    {
        std::wstring node::str()
        {
            std::wstringstream s ;
            this->to_json(s);
            return s.str();
        }
        void node_value::to_json(std::wostream& o)
        {
            o << _value ;
        }
        node::sptr node_value::xpath_impl(path_arr_t& key_arr) 
        {
            return node::sptr();
        }
        void node_null::to_json(std::wostream& o)
        {
            o <<  "null" ;
        }

        node::sptr node_null::xpath_impl(path_arr_t& key_arr) 
        {
            return node::sptr();
        }
        struct node_arr::impl 
        {/*{{{*/
            void to_json(std::wostream& o)
            {
                o<< '[' ;
                for( iterator it = _value.begin() ; it != _value.end(); ++it)
                {
                    if (it != _value.begin())
                        o << ", " ;
                    (*it)->to_json(o) ;
                }
                o << ']' ;
            }
            node::sptr xpath_impl(path_arr_t& key_arr) 
            {
                if ( key_arr.size() >= 1 )
                {
                    unsigned int key = boost::get<unsigned int >(key_arr.front());
                    key_arr.pop_front();
                    if ( key > _value.size())
                    {
                        return node::sptr();
                    }
                    node::sptr found = _value[key];
                    if ( key_arr.size()  ==  0 )
                    {
                        return  found ;
                    }
                    return found->xpath_impl(key_arr);
                }
                return node::sptr();
            }

            impl(node_arr::value_type& v ):_value(v){}
            value_type  _value; 
        };/*}}}*/

        node_arr::node_arr( node_arr::value_type& v ) : _pimpl(new node_arr::impl(v)) { }

		node_arr::iterator node_arr::begin()
		{
			return _pimpl->_value.begin();
		}
		node_arr::iterator node_arr::end()
		{
			return _pimpl->_value.end();
		}

		node_arr::const_iterator node_arr::begin() const 
		{
			return _pimpl->_value.begin();
		}
		node_arr::const_iterator node_arr::end() const 
		{
			return _pimpl->_value.end();
		}
		node::sptr node_arr::operator[] (unsigned int i) 
		{
			return _pimpl->_value[i];
		}
        void node_arr::to_json( std::wostream& o) 
        {
            _pimpl->to_json(o);
        }
        node::sptr node_arr::xpath_impl(path_arr_t& key_arr) 
        {
            return _pimpl->xpath_impl(key_arr);
        }
        struct node_object::impl
        {/*{{{*/
            value_type     _value; 
            impl( node_object::value_type& v ) : _value(v){}
            void to_json( std::wostream& o)
            {
                o << "{ "  ;
                for( iterator it = _value.begin() ; it != _value.end(); ++it)
                {
                    if (it != _value.begin())
                        o << ", " ;
                    o << '"'  << it->first   << '"' ;
                    o << " : " ;
                    (it->second)->to_json(o) ;
                }
                o << "}" ;
            }
            node::sptr xpath_impl(path_arr_t& key_arr) 
            {
                if ( key_arr.size() >= 1 )
                {
                    path_type t   = key_arr.front(); 
                    wstring&  key = boost::get<wstring>(t);
                    key_arr.pop_front();
                    iterator found_it=  _value.find(key);
                    if (found_it ==  _value.end())
                    {
                        return node::sptr();
                    }
                    node::sptr found =  found_it->second ;
                    if ( key_arr.size()  ==  0 )
                    {
                        return  found ;
                    }
                    return found->xpath_impl(key_arr);
                }
                return node::sptr();
            }
        };/*}}}*/

        node_object::node_object( node_object::value_type& v ) :_pimpl(new node_object::impl(v)){}
        void node_object::to_json( std::wostream& o) 
        {
            _pimpl->to_json(o);
        }
        node::sptr node_object::xpath_impl(path_arr_t& key_arr) 
        {
            return _pimpl->xpath_impl(key_arr);
        }
		node::sptr node_object::operator[] (const std::wstring& key) 
		{
			return _pimpl->_value[key];
		}
		node_object::const_iterator node_object::begin() const 
		{
			return _pimpl->_value.begin();
		}
		node_object::const_iterator node_object::end() const 
		{
			return _pimpl->_value.end();
		}

		node_object::iterator node_object::begin()
		{
			return _pimpl->_value.begin();
		}
		node_object::iterator node_object::end()
		{
			return _pimpl->_value.end();
		}
    }
}
