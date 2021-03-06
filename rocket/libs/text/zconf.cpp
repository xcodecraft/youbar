#include <boost/config/warning_disable.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <list>
#include "utls.h"
#include "text/conf.h"
#include "text/conf_core.h"
#include "text/conf_cmd.h"
#include "text/text.h"
#include <boost/foreach.hpp>
#include "error/error.h"
#include <fstream>
#include "conf_impl.h"
#include <boost/fusion/sequence/intrinsic/at_c.hpp>
#include <boost/fusion/include/at_c.hpp>

using namespace std;
namespace  pylon { namespace rigger
    {

        std::wstring conf::str()
        {
            std::wstringstream s ;
            this->to_conf(s);
            return s.str();
        }
        void conf_value::to_conf(std::wostream& o)
        {
            o << _value ;
        }
        conf* conf_value::xpath_impl( path_arr_t& key_arr) 
        {
            return  NULL;
        }
        void conf_value::clear()
        {}

        void conf_value::export_val(pylon::logger* l,conf_object* host) 
        {
            BOOST_ASSERT(host != NULL);
            host->assign(_name,_value,l);
//            \WLOG_INFO2(l) <<  object->name() << L"."  << _name  << L" : "  << _value;

        }
        void conf_value::use(pylon::logger* l)
        {/*{{{*/
            conf_object*  object = NULL;
            if (this->get_host_r(object))
            {
                var_ref* v  = boost::get<var_ref>(&_value);
                conf::value_type new_val =  _value;
                if (v != NULL)
                {
                    object->get_val(v->name,new_val);
                }
                else
                {
                    conf_value::vptr res =  parser::exp_cacul(_value,object,l);
                    if (res) 
                    {
                        new_val = res->_value;
                    }
                }
                WLOG_DEBUG(l) <<   new_val <<  " [" << _value << L"]";
                _value = new_val ;
            }
            else
            {
                wbug_error e(L" not found host space" );
                throw e;
            }
        }/*}}}*/

        struct conf_box;
        typedef void (*box_call_t) (conf_box * ,const std::wstring&)  ;
        struct conf_box
        {/*{{{*/
            typedef std::vector<conf_object*> obj_vector;
            typedef std::pair<std::wstring, conf::sptr>  pair;
            typedef std::map<std::wstring, conf::sptr>   map;
            wstring					_name ;
            keyword_t				_keyword;
            map						_dict; 
            conf::sptr_vector		_arr;
            conf*					_holder;
            conf_box( const wstring& name ,keyword_t k) : _name(name),_keyword(k),_holder(0) {}
            virtual ~conf_box(){}

            void  init( conf* holder,   const conf::sptr_vector& items )
            {
                BOOST_FOREACH(conf::sptr x , items)
                {
                    append(x->name(),x);
                    x->set_holder(holder);
                }
            }
            virtual void merge(conf* host ,conf_box* other)
            {
                BOOST_FOREACH(conf::sptr x , other->_arr)
                {
                    append(x->name(),x);
                    x->set_holder(host);
                }

            }
            bool get_val(const std::wstring& key,conf::value_type& val )
            {
                map::iterator it = _dict.find(key);
                if(it != _dict.end())
                {
                    conf_value* pval = dynamic_cast<conf_value*>(it->second.get());
                    if( pval)
                    {
                        val = pval->_value;
                        return true ;
                    }
                }
                return false ;
            }
            virtual void clear()
            {
                BOOST_FOREACH(conf::sptr i , _arr)
                {
                    i->clear();
                }
                _dict.clear();
                _arr.clear();
                _holder = NULL;
            }
            virtual void extend()
            {
                BOOST_FOREACH(conf::sptr i , _arr)
                {
                    i->extend();
                }
            }
            virtual void call(conf::order o ,const char* cmd,pylon::logger* l)
            {

				if (o == conf::reverse) 
				{
					BOOST_REVERSE_FOREACH(conf::sptr i , _arr)
					{
						i->call(o,cmd,l);
					}
				}
				else
				{
					BOOST_FOREACH(conf::sptr i , _arr)
					{
						i->call(o,cmd,l);
					}
				}
            }
            virtual void use(pylon::logger* l)
            {
                BOOST_FOREACH(conf::sptr i , _arr)
                {
                    i->use(l);
                }

            }
            void to_conf( std::wostream& o)
            {
                /*
                   o <<  _name  ; 
                   for( obj_vector::iterator it = _parents.begin() ; it != _parents.end(); ++it)
                   {
                   if(it == _parents.begin())
                   {
                   o << " : " << (*it)->name() ;
                   }
                   else
                   {
                   o << ", "  << (*it)->name() ;
                   }
                   }
                   o <<  L" { " ;
                   for( conf::sptr_vector::iterator it = _arr.begin() ; it != _arr.end(); ++it)
                   {
                   if (it != _arr.begin())
                   o << ", " ;
                   o << '"'  << (*it)->name() << '"' ;
                   o << " : " ;
                   (*it)->to_conf(o) ;
                   }
                   o << " }" ;
                   */
            }
            virtual conf* xpath_impl(  conf::path_arr_t& key_arr) 
            {
                if ( key_arr.size() >= 1 )
                {
                    conf::path_type key   = key_arr.front(); 
                    key_arr.pop_front();
                    conf*  result=NULL;
                    if( get_sub(key,result))
                    {
                        if ( key_arr.size()  ==  0 )
                        {
                            return  result;
                        }
                        return result->xpath_impl(key_arr);
                    }

                }
                return  NULL;
            }
            template < typename T> 
                bool get_sub(wstring& key, T& v)
                {
                    map::iterator found_it=  _dict.find(key);
                    if (found_it ==  _dict.end())
                    {
                        return conf::sptr();
                    }
                    v = dynamic_cast<T>( (found_it->second).get()) ;
                    return v!= NULL;
                }

//            template < typename T> 
                void do_cmd(  conf::path_arr_t& key_arr , box_call_t found_do , box_call_t unfound_do )
            {

                if ( key_arr.size() >= 1 )
                {
                    conf::path_type key   = key_arr.front(); 
                    key_arr.pop_front();
                    conf_space *  result=NULL;
                    if( get_sub(key,result))
                    {
                        if ( key_arr.size()  ==  0 )
                        {
                            found_do(this,key);
                            return ;
                        }
                        else{
                            //return result->_pimpl.do_cmd(key_arr,found_do,unfound_do);
                        }
                    }
                    else
                    {
                        unfound_do( this,key);
                    }

                }

            }

            void append( conf::sptr value )
            {
                wstringstream n;
                n << L"_" << _arr.size()  ;
                _arr.push_back(value);
                _dict[n.str() ]  = value; 
            }
            void append(const std::wstring& name, conf::sptr value )
            {
                if ( name.size() != 0 )
                {
					map::iterator it = _dict.find(name);
					if (it!= _dict.end())
					{
						_dict[name]->merge(value);
					}
					else
					{
						_dict[name]  = value; 
						_arr.push_back(value);
					}
                }
                else
                {
                    wstringstream n;
                    n << L"_" << _arr.size()  ;
                    _arr.push_back(value);
                    _dict[n.str() ]  = value; 

                }
            }

            void export_val(pylon::logger* l,conf_object * host ) 
            {

                BOOST_FOREACH(conf::sptr i , _arr)
                {
                    i->export_val(l,host);
                }
            }
        };/*}}}*/


        struct conf_object::impl   : public conf_box
        {/*{{{*/
            typedef std::map<wstring, conf::value_type> dict_t;
            conf_object::key_vector _parent_names;
            obj_vector				_parents;
            conf_object*            _host;
            dict_t			_var_dict;
            impl( const wstring& name ) : conf_box(name,keyword_t()){}

            void  init( conf* holder,  const conf_object::key_vector& parents, const conf::sptr_vector& items )
            {/*{{{*/
                _parent_names  = parents ;
                BOOST_FOREACH(conf::sptr x , items)
                {
                    append(x->name(),x);
                    x->set_holder(holder);
                }
            }/*}}}*/
            void use(pylon::logger* l)
            {/*{{{*/
                BOOST_FOREACH(conf_object* i , _parents)
                {
                    i->use(l);
                }
                conf_box::use(l);

            }/*}}}*/
            void call_cmd(conf_object* host ,const char* cmd ,pylon::logger* l)
            {/*{{{*/
                WLOG_INFO1(l) << L"call "  << cmd  ;
                if (strcmp(cmd,"config") == 0 )
                {
                    WLOG_INFO1(l) << L"call config"  ;
                    host->config(); 
                }
                else  if(strcmp(cmd,"start") == 0 )
                {
                    host->start();
                }
                else if (strcmp(cmd,"stop") == 0 )
                {
                    host->stop();
                }
                else if (strcmp(cmd,"reload") == 0 )
                {
                    host->reload();
                }
                else if (strcmp(cmd,"clean") == 0 )
                {
                    host->clean();
                }
                else if (strcmp(cmd,"data") == 0 )
                {
                    host->data();
                }
            }/*}}}*/
			void call(conf_object* host ,conf::order o ,const char* cmd ,pylon::logger* l)
            {/*{{{*/


				if (o == conf::reverse) 
				{
                    call_cmd(host,cmd,l);
					conf_box::call(o,cmd,l);
					BOOST_REVERSE_FOREACH(conf_object* i , _parents)
					{
						i->call(o,cmd,l);
					}
				}
				else
				{
					BOOST_FOREACH(conf_object* i , _parents)
					{
						i->call(o,cmd,l);
					}
					conf_box::call(o,cmd,l);
                    call_cmd(host,cmd,l);
				}

            }/*}}}*/

            void extend()
            {/*{{{*/
                BOOST_FOREACH( std::wstring key , _parent_names )
                {
                    if (_holder)
                    {
                        conf_object* found = NULL;
                        if(_holder->xpath_t(key,found))
                        {
                            this->inherit(found);
                        }
                        else
                        {
                            wstringstream info ;
                            info << L" unfound   "  << key  << L" or is not object"  ;
                            throw pylon::wbug_error(info.str().c_str());
                        }
                    }
                }
                conf_box::extend();
            }/*}}}*/
			wstring name()
			{
				return _name ;
			}
			wstring fullname()
			{/*{{{*/
				wstringstream x;
				BOOST_FOREACH( wstring i , _parent_names)
				{
					x << i << L".";
				}
				x << _name ;
				return x.str();
			}/*}}}*/
            void  inherit(conf_object* parent)
            {
                _parents.push_back(parent);
            }
            bool get_val( const std::wstring& key,conf::value_type& val )
            {/*{{{*/
                if (!conf_box::get_val(key,val))
                {
                    BOOST_REVERSE_FOREACH( conf_object* parent, _parents)
                    {
                        if ( parent->get_val(key,val)) return true ;
                    }
                }
                return false;
            }/*}}}*/

            
            void export_val(pylon::logger* l,conf_object * host ) 
            {
					BOOST_FOREACH(conf_object* i , _parents)
					{
                        i->export_val(l,host);
					}
					conf_box::export_val(l,host);
            }

        };/*}}}*/

        conf_object::conf_object( const wstring& name  ) :_pimpl(new conf_object::impl(name )){}
        conf_object::conf_object() :_pimpl(new conf_object::impl(L"")){}

        conf_object:: conf_object( conf_object::attr& attr, const conf::sptr_vector& items)
            :_pimpl(new conf_object::impl(L"")) 
        { 
            if( boost::fusion::at_c<0>(attr) )
                _pimpl->_name = *boost::fusion::at_c<0>(attr);

            conf_object::key_vector parents;
            _pimpl->init(this,parents, items);
        }
        void conf_object::clear()
        {
            _pimpl->clear();
        }
        wstring conf_object::name()
        {
            return _pimpl->name();
        }

        void conf_object::export_val(pylon::logger* l,conf_object * host = NULL) 
        {

            //子对象不导出
            if (host == NULL) return ;

            conf_object*  object = NULL;
            if (this->get_host_r(object))
            {
                _pimpl->export_val(l,object);
            }
            else
            {
                wbug_error e(L" not found host object" );
                throw e;
            }
        }

        void  conf_space::assign(const std::wstring& key , conf::value_type val,pylon::logger* l)
        {
            _pimpl->_var_dict[key] = val ;
            WLOG_INFO(l) <<  name() << L"."  << key << L" : "  << val ;
        }

        bool  conf_object::get_val(const std::wstring& key ,conf::value_type& val)
        {
            return _pimpl->get_val(key,val);
        }
        void conf_object::set_holder(conf* u)
        {
            _pimpl->_holder = u ;
        }
        conf* conf_object::holder()
        {
            return _pimpl->_holder;
        }

        void conf_object::extend()
        {
            _pimpl->extend();
        }
        void conf_object::use(pylon::logger* l)
        {
			WLOG_DEBUG(l) << name() << L" use begin"  ;
            _pimpl->use(l);
			WLOG_DEBUG(l) << name() << L" use end"  ;
        }

        void conf_object::append(const std::wstring& name, conf::sptr value )
        {
            _pimpl->append(name,value);
        }

        void conf_object::append(conf::sptr value )
        {
            _pimpl->append(value);
        }
        void conf_object::call(conf::order o ,const char* cmd ,pylon::logger* l)
        {
				_pimpl->call(this,o,cmd,l);
        }
        
        void conf_object::to_conf( std::wostream& o) 
        {
            _pimpl->to_conf(o);
        }
        conf* conf_object::xpath_impl( path_arr_t& key_arr) 
        {
            return _pimpl->xpath_impl(key_arr);
        }
        conf::sptr conf_object::operator[] (const std::wstring& key) 
        {
            return _pimpl->_dict[key];
        }
        conf_object::const_iterator conf_object::begin() const 
        {
            return _pimpl->_arr.begin();
        }
        conf_object::const_iterator conf_object::end() const 
        {
            return _pimpl->_arr.end();
        }

        conf_object::iterator conf_object::begin()
        {
            return _pimpl->_arr.begin();
        }
        conf_object::iterator conf_object::end()
        {
            return _pimpl->_arr.end();
        }
        void  conf_object::inherit(conf_object* parent)
        {
            _pimpl->inherit(parent);
        }


        struct conf_space::impl : public conf_box
        {/*{{{*/
            typedef std::map<wstring, conf::value_type> dict_t;
            dict_t			_var_dict;
            impl( const wstring& name ) : conf_box(name,keyword_t()){
			}
            impl( const wstring& name ,keyword_t k) : conf_box(name,k) {
			}
            virtual ~impl(){}
            bool get_val(const std::wstring& key,conf::value_type& val )
            {
                dict_t::iterator found = _var_dict.find(key);
                if (found != _var_dict.end())
                {
                    val = found->second;
                    return true ;
                }
                return false ;
            }
        };/*}}}*/
        void box_nothing( conf_box* box, const wstring& key)
        {}
        void append_space( conf_box* box, const wstring& key)
        {
            box->append(key,conf::sptr(new conf_space(key)));
        }

        conf_space::conf_space( const string& name  ) :_pimpl(new conf_space::impl(pylon::utf8to16(name ))){}
        conf_space::conf_space( const wstring& name  ) :_pimpl(new conf_space::impl(name )){}
        conf_space:: conf_space(const std::wstring& name , const conf::sptr_vector& items,keyword_t k)
            :_pimpl(new conf_space::impl(name,k)) 
        { 
            _pimpl->init(this, items);
        }


        void conf_space::regist_obj(const std::wstring& key , conf::sptr obj)
        {
            _pimpl->append(key,obj);
        }
        bool conf_space::get_val_impl(conf::path_arr_t& key_arr, conf::value_type& val )
        {

            if ( key_arr.size() >= 1 )
            {
                conf::path_type key   = key_arr.front(); 
                key_arr.pop_front();
                if(_pimpl->get_val(key,val)) return true ;

                conf_space* sub = NULL;
                if (_pimpl->get_sub(key,sub))
                {
                    return sub->get_val_impl(key_arr,val);
                }
                else
                {
                    return false;
                }
            }
            return false ;
        }
        void  conf_space::assign(const std::wstring& key , conf::value_type val,pylon::logger* l)
        {
            if (_pimpl->_keyword && _pimpl->_keyword.get() == L"regist") 
            {
                _pimpl->_var_dict[key] = val ;
                WLOG_INFO(l) <<  name() << L"."  << key << L" : "  << val ;
            }
        }
        void conf_space::clear()
        {
            return _pimpl->clear();
        }
        void conf_space::make_space(const wchar_t* path)
        {
            path_arr_t  arr;
            if(parser::xpath_split(path,arr))
            {
                box_call_t found = &box_nothing ;
                box_call_t unfound = &append_space ;
                //_pimpl->do_cmd<conf_space>(arr,found,unfound);
                _pimpl->do_cmd(arr,found,unfound);
            }
        }
        void conf_space::call(conf::order o , const char* cmd,pylon::logger* l)
        {
            return _pimpl->call(o,cmd,l);
        }
        void conf_space::merge(conf::sptr other)
        {
			conf_space*  space = dynamic_cast<conf_space*>(other.get());
			if(space)
				_pimpl->merge(this,space->_pimpl.get());
			else
			{
				wstringstream err_info ;
				err_info  << L"merge sapce faild, other not is sapce :"  <<  name() ;
				throw pylon::wbug_error(err_info.str().c_str());
			}
        }
        wstring conf_space::name()
        {
            return _pimpl->_name;
        }

        void conf_space::set_holder(conf* u)
        {
            _pimpl->_holder = u ;
        }
        conf* conf_space::holder()
        {
            return _pimpl->_holder;
        }

        void conf_space::extend()
        {
            _pimpl->extend();
        }
        void conf_space::to_conf( std::wostream& o) 
        {
            _pimpl->to_conf(o);
        }
        conf* conf_space::xpath_impl( path_arr_t& key_arr) 
        {
            return _pimpl->xpath_impl(key_arr);
        }
        void conf_space::use(pylon::logger* l)
        {
            _pimpl->use(l);
        }

        std::wostream & operator << (std::wostream& out  , const var_ref& v )
        {
            out << v.name;
            return out ;
        }

        zconf::zconf():_root(new conf_system(L"_g")) 
        {
            pylon::logger* l = pylon::log_kit::log_ins(L"rigger");
            pylon::log_kit::conf(L"rigger",L"run",log_kit::debug);
            WLOG_INFO(l) << L"begin" ;
        }
        zconf::~zconf()
        {
            pylon::log_kit::clear();
        }
        conf* zconf::root()
        {
            return _root.get();
        }
        bool zconf::import(const char* file)
        {
            ifstream conf_file(file); 
            const int max_buf = 1024 * 50 ;
            char buf[max_buf] ;
            memset(buf,0,max_buf);
            conf_file.read(buf,max_buf);
            stringstream content;
            content << " space g { " << buf <<  " } "  ;
            conf::sptr x ;
            if(_parser.parse_utf8(content.str(),x))
            {
                _root->merge(x);
                return true;
            }
            return false;
        }

        void zconf::link()
        {
            _root->extend();
        }
        void zconf::use(const char* xpath)
        {
            pylon::logger*  l = pylon::log_kit::log_ins(L"rigger");
            wstring wpath = pylon::utf8to16(xpath);
            WLOG_INFO(l) << L"use "  << wpath ;
            conf*  found = _root->xpath(wpath);
            if (found)
            {
                found->use(l);
            }
        }
		void zconf::call2(int o,const char* cmd, const char* xpath)
        {
            call(conf::order(o),cmd,xpath);
        }
		void zconf::call(conf::order o,const char* cmd, const char* xpath)
        {
            wstring wpath = pylon::utf8to16(xpath);
            conf*  found = _root->xpath(wpath);
            if (found)
            {
				pylon::logger*  l = pylon::log_kit::log_ins(L"rigger");
                found->call(o,cmd,l);
            }
        }
        void zconf::regist_obj(const char* xpath,conf_object::sptr obj )
        {
            wstring wpath = pylon::utf8to16(xpath);
            _root->make_space(wpath.c_str());
            conf*  found = _root->xpath(wpath);
            if (found)
            {
				conf_system* cs = dynamic_cast<conf_space*>(found);
				if(cs)
				{
					cs->regist_obj(obj->name(),obj);
				}
            }
        }

    }
}
