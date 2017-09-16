
#if defined(_MSC_VER)
#pragma warning( disable : 4275 )
#pragma warning( disable : 4251 )
#endif
#ifndef __PYLON_ERROR_HPP__
#define __PYLON_ERROR_HPP__
#include "port.h"
#include <stdexcept>
#include <boost/exception/all.hpp>
#include <string>
#include <boost/assert.hpp>
namespace pylon
{

    template < typename T > 
    class basic_bug_error : public virtual boost::exception, public virtual std::logic_error
    { 
        std::basic_string<T> _msg;
        public:
            basic_bug_error():std::logic_error(""){}
            basic_bug_error(const T * what):std::logic_error(""),_msg(what){} ;
            virtual ~basic_bug_error()throw() {};
            const T* what(){return _msg.c_str();}
    };  
    typedef basic_bug_error<char >   bug_error; 
    typedef basic_bug_error<wchar_t> wbug_error; 


    template < typename T > 
    class PYLON_API basic_run_error : virtual public boost::exception, virtual public std::runtime_error
    { 
        std::basic_string<T> _msg;
        public:
            basic_run_error():std::runtime_error(""){}
            basic_run_error(const T * what):std::runtime_error(""),_msg(what){} ;
            virtual ~basic_run_error()throw() {};
            const T* what(){return _msg.c_str();}
    };  

    typedef basic_run_error<char >   run_error; 
    typedef basic_run_error<wchar_t> wrun_error; 

    typedef boost::error_info<struct tag_errno,unsigned long >              errno_info; 
    typedef boost::error_info<struct tag_function,char const *>             function_info;
    typedef boost::error_info<struct tag_express,char const *>              express_info;
    typedef boost::error_info<struct tag_function ,wchar_t const *>         wfunction_info;
    typedef boost::error_info<struct tag_express  ,wchar_t const *>         wexpress_info;
    typedef boost::error_info<struct tag_line,int >                         line_info;
    typedef boost::tuple<express_info,function_info,line_info,errno_info>   err_info;

//    typedef boost::tuple<express_info,wfunction_info,wline_info,errno_info>   werr_info;

	struct  tag_expect{};
	struct  tag_result{};
	struct  tag_unexpect{};

    template < typename ERR_TYPE>
        inline bool  ensure_success(bool ret, ERR_TYPE err ,const char*   exp_info,const char * fun_name ,int line )
        {
            if (!ret)
            {
                throw err <<  err_info(exp_info,fun_name,line,0);
            }
            return ret;
        }
        inline bool  ensure_success(bool ret, const char*   exp_info,const char * fun_name ,int line )
        {
            if (!ret)
            {
                bug_error err(exp_info);
                err <<  err_info(exp_info,fun_name,line,0);
                throw err ;
            }
            return ret;
        }

    template < typename T, typename ERR_TYPE>
        inline T expect_value( T result , T succed , ERR_TYPE err, const char*  exp_info ,const char * fun_name ,int line , const wchar_t* msg = L"")
        {
            if ( result != succed )
			{
				//ERR_TYPE err(msg);
				typedef boost::error_info<struct tag_expect,T>   expect_info  ;
				typedef boost::error_info<struct tag_result,T>   result_info  ;
				typedef boost::tuple<express_info,function_info,expect_info,result_info>  err_info_1 ;
				throw err <<  err_info_1(exp_info,fun_name,succed,result);
			}
            return result ;
        }

    template < typename T, typename ERR_TYPE>
        inline T expect_not_value( T result , T failed, ERR_TYPE err, const char*  exp_info ,const char * fun_name ,int line)
        {
            if ( result == failed )
			{

				typedef boost::error_info<struct tag_unexpect,T>		unexpect_info  ;
				typedef boost::error_info<struct tag_result,T>			result_info  ;
				typedef boost::tuple<express_info,function_info,unexpect_info,result_info>  err_info_2;
				throw err <<  err_info_2(exp_info,fun_name,failed,result);
			}
            return result ;
        }


    template < typename T, typename X,typename ERR_TYPE>
        inline T container_have( T result , X& container ,ERR_TYPE err, const char*  exp_info ,const char * fun_name ,int line)
        {
            if ( result == container.end() )
                throw err <<  err_info(exp_info,fun_name,line,0);
            return result ;
        }

#define MUST_SUC(EXP) \
    pylon::ensure_success(EXP,#EXP,__FUNCTION__,__LINE__);  

#define MUST_SUC_EX(EXP,ERR) \
    ensure_success(EXP,ERR,#EXP,__FUNCTION__,__LINE__,MSG);  

#define EXPECT(EXP,SUC,ERR) \
    pylon::expect_value(EXP,SUC,ERR,#EXP,__FUNCTION__,__LINE__,L"not expect value" );  

#define EXPECT_NOT(EXP,SUC,ERR) \
    pylon::expect_not_value(EXP,SUC,ERR,#EXP,__FUNCTION__,__LINE__);  


#define MUST_HAVE(EXP,CTN,ERR) \
    pylon::container_have(EXP,CTN,ERR,#EXP,__FUNCTION__,__LINE__);  


#define THROW_ERR(ERR,MSG) \
    throw ERR<<  err_info("",__FUNCTION__,__LINE__,MSG);

#define SUB_EXCEPTION(CLS,PARENT)     \
    class  CLS : public PARENT { public:  CLS(const wchar_t * what):PARENT(what){} CLS(){} } ;
}
#endif 
