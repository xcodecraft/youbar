#ifndef __RIGGER__HPP__ 
#define __RIGGER__HPP__ 
#include "pimpl.h"
#include "utls.h"
#include "log/log.h"
#include "error/error.h"
//#include "conf_cmd.h"
#include <vector>
#include <list>
#include <map>
#include <boost/optional.hpp>
#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/variant.hpp>
#include <boost/pool/singleton_pool.hpp>
#include <boost/pool/object_pool.hpp>

//namespace opt = boost::optional ;
namespace  pylon { 
    namespace rigger {

        struct PYLON_API var_ref  
        {/*{{{*/
            var_ref(){}
            var_ref(const std::wstring& n):name(n){}
            std::wstring     name;
        };/*}}}*/
        typedef boost::optional<std::wstring> keyword_t;
        std::wostream & operator << (std::wostream& out  , const var_ref& v );


        typedef boost::variant<float ,int,std::wstring> args_type  ;
        class conf_args : public std::map< std::wstring, args_type>
        {/*{{{*/
            public:
                typedef std::pair< std::wstring,args_type> pair;
                void merge(const conf_args*);
        };/*}}}*/

        struct PYLON_API conf
        {/*{{{*/
            enum order{positive,reverse} ;
            typedef boost::variant<float ,int,std::wstring, conf* , var_ref> value_type  ;
            typedef std::vector<conf* >								    sptr_vector;
            typedef std::wstring										path_type  ;
            typedef std::list<path_type>								path_arr_t ;

            virtual void to_conf(std::wostream& o, std::wstring prefix){} ;
            virtual ~conf(){}
            virtual conf* holder(){ return 0 ; } 
            virtual void  set_holder(conf* u) {};
            virtual void  clear() {} ;
            virtual void  merge(conf* other) { throw pylon::wbug_error(L" can't merge" ); };
            virtual std::wstring name() { return std::wstring(); } 
            virtual void  call(conf::order o ,const char* cmd ,const conf_args* args, pylon::logger* l){}
            virtual conf* xpath(conf::path_arr_t arr){ return NULL; }

        };/*}}}*/

        struct cmd
        {
            virtual void  call(const wchar_t* name, conf::order o ,const char* cmd ,const conf_args* args, pylon::logger* l) =0 ;
            virtual ~cmd(){}
        };

        struct PYLON_API conf_base  : public conf 
        {/*{{{*/
            std::wstring    _name;
            conf*           _holder; 
            conf_base(const std::wstring& name):_name(name),_holder(0){}
            virtual std::wstring name() { return _name; }
            void    set_name(const std::wstring& name){ _name = name ; }
            virtual conf* holder() { return _holder ;}   
            virtual void  set_holder(conf* u) { _holder = u; } 
        };/*}}}*/
        
        template <typename T> struct mem_pool
        {
            void* operator new(size_t size) 
            {
                typedef boost::singleton_pool<T, sizeof(T)> pool;
                void* p = pool::malloc();
//                std::cout << "\n****  operator new :" << typeid(T).name() << std::endl ;
                return  p; 
            }
            void  operator delete(void * ) {}
        };
        struct PYLON_API conf_value : public conf_base , mem_pool<conf_value> 
        {/*{{{*/
            conf::value_type _value ;
            typedef boost::shared_ptr<conf_value>  vptr;
            conf_value(const std::wstring& n, conf::value_type v ):conf_base(n),_value(v){}
            virtual void to_conf(std::wostream& o, std::wstring prefix) ;
            virtual void clear()  ;
        };/*}}}*/

        struct PYLON_API conf_object : public conf_base , mem_pool<conf_object>
        {/*{{{*/

            typedef boost::fusion::vector < std::wstring , boost::optional<std::wstring> ,  boost::optional<std::wstring> > attr;
            typedef std::vector<std::wstring>            key_vector;
            typedef conf::sptr_vector::iterator          iterator;
            typedef conf::sptr_vector::const_iterator    const_iterator;
            void            inherit(conf_object* parent);
            iterator        begin();
            iterator        end();
            conf*      operator[] (const std::wstring& key) ;
            explicit        conf_object( const std::wstring& name  ) ;
            explicit        conf_object(conf_object::attr& attrs, const conf::sptr_vector& items);
            bool            get_val(const std::wstring& key,conf::value_type& val );
            virtual void    to_conf(std::wostream& o, std::wstring prefix) ;
            void            set_default_args(const conf_args*);
            conf_args*      default_args();
            void            append( conf* value );
            void            bind_cmd(cmd* );

            template< typename T > void append_value(std::wstring key ,T value)
            {
                this->append(new conf_value(key,value));
            }
//            virtual std::wstring name() ;
            virtual void  clear();
            virtual void  call(conf::order o ,const char* cmd ,const conf_args* args, pylon::logger* l);

            virtual conf* xpath(conf::path_arr_t arr);
            PIMPL_DEF;
        };/*}}}*/

        struct PYLON_API conf_module  : public conf_base , mem_pool<conf_module> 
        {/*{{{*/
            virtual void  append( conf* value );
            virtual conf* xpath(conf::path_arr_t arr);
            virtual void    to_conf(std::wostream& o, std::wstring prefix) ;
            explicit        conf_module( const std::wstring& name  ) ;
            explicit        conf_module( const std::wstring& name , conf::sptr_vector items ) ;
            PIMPL_DEF;
        };/*}}}*/

        struct PYLON_API module_space 
        {/*{{{*/
            static void  append( conf* value );
            static conf* xpath(const std::wstring&);
            static void  to_conf(std::wostream& o, std::wstring prefix) ;
            static void regist_obj(const std::wstring& module, conf* value );
            static void regist_obj2(const std::string& module, conf* value );
        };/*}}}*/
        struct PYLON_API conf_system : public mem_pool<conf_system>
        {/*{{{*/
            conf_system( const std::wstring& name  );
            void  append( conf* value );
            void  call(conf::order o ,const char* cmd ,const conf_args* args, pylon::logger* l);
            PIMPL_DEF;
        };/*}}}*/

        struct parser
        {
            static bool xpath_split(const std::wstring& path, conf::path_arr_t& arr);
        };
        struct rigger
        {
            static void init();
            static void clear();
        };

    }


}
#endif
