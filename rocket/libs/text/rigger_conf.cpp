#include <boost/config/warning_disable.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <list>
#include "utls.h"
#include "text/rigger.h"
#include "text/text.h"
#include "rigger_impl.h"
#include <boost/foreach.hpp>
#include "error/error.h"
#include <fstream>
#include <boost/fusion/include/at_c.hpp>

using namespace std;
namespace  pylon { namespace rigger
    {



        struct conf_object::impl   : public conf_box , mem_pool<conf_object::impl>
        {/*{{{*/
            typedef std::map<wstring, conf::value_type> dict_t;
            conf_object::key_vector _parent_names;
            obj_vector				_parents;
            dict_t			_var_dict;
            cmd*            _cmd;
            impl( conf* host ) : conf_box(keyword_t(),host),_cmd(0){}

            void  init( conf* holder,  const conf_object::key_vector& parents, const conf::sptr_vector& items )
            {/*{{{*/
                _parent_names  = parents ;
                BOOST_FOREACH(conf* x , items)
                {
                    append(x);
                    x->set_holder(holder);
                }
            }/*}}}*/
            void bind_cmd(cmd* c)
            {/*{{{*/
                _cmd = c;
            }/*}}}*/

            void  inherit(conf_object* parent)
            {
                _parents.push_back(parent);
            }

            void to_conf( std::wostream& o,std::wstring prefix)
            {
                o<< _host->name();
                bool first_item = true;
                BOOST_FOREACH(conf* c , _parents)
                {
                    if(first_item)
                        o<< L" : " << c->name() ; 
                    else
                        o << L", " <<  c->name();
                    first_item = false;
                }

                conf_box::to_conf(o,prefix);
            }

            void  call(conf::order o ,const char* cmd ,const conf_args* args, pylon::logger* l)
            {/*{{{*/
                if(o == conf::positive)
                {
                    BOOST_FOREACH(conf_object* op , _parents)
                    {
                        op->call(o,cmd,args,l);
                    }
                    conf_box::call(o,cmd,args,l);
                    if(_cmd)
                        _cmd->call(_host->name().c_str(),o,cmd,args,l);
                }
                else
                {
                    if(_cmd)
                        _cmd->call(_host->name().c_str(),o,cmd,args,l);
                    conf_box::call(o,cmd,args,l);
                    BOOST_REVERSE_FOREACH (conf_object* op , _parents)
                    {
                        op->call(o,cmd,args,l);
                    }

                }
            }/*}}}*/


        };/*}}}*/

        conf_object::conf_object( const wstring& name  ) :conf_base(name),_pimpl(new conf_object::impl(this )){}

        conf_object::conf_object( conf_object::attr& attrs , const conf::sptr_vector& items ) 
            :conf_base(boost::fusion::at_c<0>(attrs) ),
             _pimpl(new conf_object::impl(this )){
//                std::wstring name =   boost::fusion::at_c<0>(attrs) ;
             }

        void  conf_object::call(conf::order o ,const char* cmd ,const conf_args* args, pylon::logger* l)
        {
            _pimpl->call(o,cmd,args,l);
        }
#define CLS conf_object 

        IMPL_METHOD(void,clear) ;

        IMPL_METHOD1(void,set_default_args,const conf_args* ) ;
        IMPL_METHOD(conf_args* ,default_args);

        IMPL_METHOD1(void,bind_cmd,cmd* ) ;

        IMPL_METHOD1(conf* ,xpath,conf::path_arr_t ) ;

        IMPL_METHOD1(void,append, conf* ) ;
        IMPL_METHOD2(void,to_conf,wostream&,std::wstring ) ;

        conf* conf_object::operator[] (const std::wstring& key) 
        {
            return _pimpl->_dict[key];
        }

        conf_object::iterator conf_object::begin()
        {
            return _pimpl->_arr.begin();
        }
        conf_object::iterator conf_object::end()
        {
            return _pimpl->_arr.end();
        }
        IMPL_METHOD1(void,inherit,conf_object*);


        struct conf_module::impl : public conf_box,mem_pool<conf_module::impl>
        {
            impl( conf* host ) : conf_box(keyword_t(),host){}

            void to_conf( std::wostream& o,std::wstring prefix)
            {
                o<< L"module " << _host->name() ;
                conf_box::to_conf(o,prefix);
            }
        };

//DEF_CLS(conf_module)

#ifdef CLS  
#undef CLS   
#endif   
#define CLS conf_module 
        conf_module::conf_module( const wstring& name  ) :conf_base(name),_pimpl(new conf_module::impl(this )){}
        conf_module::conf_module( const std::wstring& name , conf::sptr_vector items ) 
            :conf_base(name),_pimpl(new conf_module::impl(this ))
        {
            BOOST_FOREACH(conf* i , items)
            {
                append(i);
            }
        }

        IMPL_METHOD1(conf* ,xpath,conf::path_arr_t ) ;

        IMPL_METHOD1(void,append,conf* ) ;
        IMPL_METHOD2(void,to_conf,wostream&,std::wstring ) ;

        struct module_space_impl : public conf_box
        {
            module_space_impl() : conf_box(keyword_t(),0){}

            void to_conf( std::wostream& o,std::wstring prefix)
            {
                conf_box::to_conf(o,prefix);
            }
        };
        static module_space_impl  s_mspace;

        void  module_space::append(conf* value )
        {
            s_mspace.append(value);
        }
        conf* module_space::xpath(const std::wstring& path)
        {

            INS_LOG(l,L"rigger");
            conf::path_arr_t p;
            parser::xpath_split(path,p);

            conf* found = s_mspace.xpath(p);
            if(found == NULL)
            {
                WLOG_WARN(l) << L"xpath  unfound : " << path;
            }
            return found ;
        }

        void  module_space::to_conf(std::wostream& o, std::wstring prefix) 
        {
            s_mspace.to_conf(o,prefix);
        }
        void module_space::regist_obj(const std::wstring& module, conf* value )
        {
            INS_LOG(l,L"rigger");
            conf* m = xpath(module);
            if(m == NULL)
            {
                WLOG_DEBUG(l) << L"create new module : " << module ;
                m = new conf_module(module);
                append(m);
            }

            conf_module* mp = dynamic_cast<conf_module*>(m);
            MUST_SUC(mp!=NULL);
            mp->append(value);

        }

        void module_space::regist_obj2(const std::string& module, conf* value )
        {
            wstring uname = utf8to16(module);
            regist_obj(uname,value);
        }
    }
}
