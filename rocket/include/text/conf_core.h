#ifndef __CONF_CORE__HPP__ 
#define __CONF_CORE__HPP__ 
#include <string>
#include <vector>
#include <list>
#include <boost/variant.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/include/vector.hpp>
#include "port.h"
#include "pimpl.h"
#include "utls.h"
#include "log/log.h"
#include "error/error.h"
#include "conf_cmd.h"
#include <boost/optional.hpp>


namespace  pylon { 
    namespace rigger {

        struct PYLON_API var_ref  
        {
            var_ref(){}
            var_ref(const std::wstring& n):name(n){}
            std::wstring     name;
        };
        typedef boost::optional<std::wstring> keyword_t;
        std::wostream & operator << (std::wostream& out  , const var_ref& v );

        struct PYLON_API conf:  smarty_ptr<conf>  
        {/*{{{*/
            enum order{positive,reverse} ;
            typedef boost::variant<float ,int,std::wstring, conf::sptr , var_ref> value_type  ;
            typedef std::vector<conf::sptr>								sptr_vector;
            typedef std::wstring										path_type  ;
            typedef std::list<path_type>								path_arr_t ;
            virtual void to_conf(std::wostream& o){} ;
            std::wstring str();
            virtual ~conf(){}
            virtual void  extend(){}
            virtual void  call(conf::order o ,const char* cmd ,const conf_args& args, pylon::logger* l);
            virtual conf* holder(){ return 0 ; } 
            virtual void  set_holder(conf* u) {};
            virtual void  clear() {} ;
            virtual void  merge(conf::sptr other) { throw pylon::wbug_error(L" can't merge" ); };
            virtual void  use(pylon::logger* l) {} ;
            virtual void  export_val(pylon::logger* l,conf_object* host=NULL) {} ;
            virtual std::wstring name() { return std::wstring(); } 
            /** 
             * @brief xpath : env.debug.log_level 
             *				  env.debug.log_level._1  
             * 
             * @param std::wstring 
             * 
             * @return 
             */
//            conf*	xpath(const std::wstring&);
//            template < typename T>
//                bool	xpath_t(const std::wstring& path, T& v)
//                {
//                    conf* found = xpath( path);
//                    v = dynamic_cast<T>( found) ;
//                    return v != NULL;
//                }
            virtual conf*  xpath_impl(path_arr_t& key_arr) { return NULL ; }

            template < typename T>
                bool get_host(T& v )
                {/*{{{*/
                    v = dynamic_cast<T>(this->holder());
                    return v!= NULL;
                }/*}}}*/
            template < typename T>
                bool get_host_r(T& v )
                {/*{{{*/
                    conf* host = this->holder();
                    while ( host != NULL)	
                    {
                        v = dynamic_cast<T>(host);
                        if ( v != NULL) return true ;
                        host = host->holder();
                    }
                    return false;

                }/*}}}*/
        };/*}}}*/
        struct PYLON_API conf_value : public conf 
        {/*{{{*/


            std::wstring     _name;
            conf::value_type _value ;
            conf*            _holder; 
            typedef boost::shared_ptr<conf_value>  vptr;

            conf_value(){}
            conf_value(const std::wstring& n, const std::wstring& v ):_name(n),_value(v),_holder(0) {}
            conf_value(const std::wstring& n, const wchar_t* v ):_name(n),_value(std::wstring(v)),_holder(0) {}
            conf_value(const std::wstring& n, int v ):_name(n),_value(v),_holder(0) {}
            conf_value(const std::wstring& n, float v ):_name(n),_value(v),_holder(0) {}
            conf_value(const std::wstring& n, conf::value_type v ):_name(n),_value(v),_holder(0) {}
            virtual std::wstring name() { return _name; }
            virtual void to_conf(std::wostream& o) ;
            conf*  holder() { return _holder ;}   
            virtual void export_val(pylon::logger* l,conf_object* host=NULL);
            virtual void use(pylon::logger* l);
            virtual void  set_holder(conf* u) { _holder = u; } 
            virtual conf* xpath_impl( path_arr_t& key_arr) ;
            virtual void  clear()  ;

        };/*}}}*/

        struct PYLON_API conf_object : public conf   
        {/*{{{*/
            typedef boost::fusion::vector < boost::optional<std::wstring> , boost::optional<std::wstring>, boost::optional<std::wstring> > attr;
            typedef boost::shared_ptr<conf_object>       optr ;
            typedef std::vector<std::wstring>            key_vector;
            typedef conf::sptr_vector::iterator          iterator;
            typedef conf::sptr_vector::const_iterator    const_iterator;
            void            inherit(conf_object* parent);
            iterator        begin();
            const_iterator  begin() const ;
            iterator        end();
            const_iterator  end() const ;
            conf::sptr      operator[] (const std::wstring& key) ;
            explicit  conf_object( const std::wstring& name  ) ;
            explicit  conf_object() ;
            explicit  conf_object(conf_object::attr& attrs, const conf::sptr_vector& items);
            virtual void export_val(pylon::logger* l,conf_object* host=NULL);
//            void assign(const std::wstring& key , conf::value_type val,pylon::logger* l);
            bool get_val(const std::wstring& key,conf::value_type& val );
            bool get_val_impl( path_arr_t& key_arr, conf::value_type& val) ;

            virtual void    extend();
            virtual void  call(conf::order o ,const char* cmd ,const conf_args& args, pylon::logger* l);

            virtual void config(){}
            virtual void start(){} 
            virtual void stop(){} 
            virtual void reload(){}
            virtual void clean(){}
            virtual void data(){} 

            void            append(const std::wstring& name, conf::sptr value );
            void            append( conf::sptr value );
            conf*           holder() ;
            void            set_holder(conf* u);

            template< typename T >
                void append_value(const std::wstring& key, T value)
                {
                    this->append(key,conf::sptr(new conf_value(key,value)));
                }
            virtual std::wstring name() ;
            virtual void use(pylon::logger* l) ;
            virtual void to_conf(std::wostream& o) ;
            virtual conf* xpath_impl(path_arr_t& key_arr) ;
            virtual void  clear();
            PIMPL_DEF;
        };/*}}}*/
//        struct PYLON_API conf_module : public  conf 
//        {
//        };
//        struct PYLON_API conf_system : public conf
//        {/*{{{*/
//            typedef boost::shared_ptr<conf_system> optr ;
//            explicit  conf_system( const std::string& name  ) ;
//            explicit  conf_system( const std::wstring& name  ) ;
//            explicit  conf_system(const std::wstring& name ,  const conf::sptr_vector& items, keyword_t k);
//            void assign(const std::wstring& key , conf::value_type val,pylon::logger* l);
//            bool get_val(const std::wstring& key,conf::value_type& val );
//            bool get_val_impl( path_arr_t& key_arr, conf::value_type& val) ;
//            virtual std::wstring name() ;
//            virtual void extend();
//            virtual void use(pylon::logger* l) ;
//            virtual void to_conf(std::wostream& o) ;
//            virtual void call(conf::order o ,const char* cmd ,pylon::logger* l );
//            virtual conf* xpath_impl(path_arr_t& key_arr) ;
//            void regist_obj(const std::wstring& key ,conf::sptr obj);
//            conf*  holder() ;
//            void  set_holder(conf* u);
//            void  make_space(const wchar_t* path);
//            virtual void  clear()  ;
//            virtual void merge(conf::sptr other);
//            PIMPL_DEF;
//        };/*}}}*/

//        class PYLON_API parser  
//        {
//            public:
//                parser();
//                static parser* ins();
//                static bool parse(std::wstring c ,conf::sptr& n);
//                static bool parse_utf8(std::string c ,conf::sptr& n);
//                static conf_value::vptr exp_cacul(conf::value_type& v, conf_object * object ,pylon::logger* l);
//                static bool xpath_split(const std::wstring& path, conf::path_arr_t& arr);
//                PIMPL_DEF;
//        };

//        class zconf
//        {/*{{{*/
//            public:
//                zconf();
//                ~zconf();
//                conf* root();
//                bool import(const char* file);
//                void link();
//                void use(const char* xpath);
//                void regist_obj(const char* xpath,conf_object::sptr);
//                void call(conf::order o, const char* cmd, const char* xpath);
//                void call2(int o, const char* cmd, const char* xpath);
//            private:
//                conf_space::optr _root;
//                parser     _parser;
//        };/*}}}*/

    }


}
#endif
