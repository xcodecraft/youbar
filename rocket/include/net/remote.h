// remote.cpp : 定义 DLL 应用程序的导出函数。
//
#ifndef __PYLON_REMOTE_HPP__
#define __PYLON_REMOTE_HPP__
#include "port.h"
#include "pimpl.h"
#include "utls.h"
#include "error/log.h"

namespace pylon
{
    class  PYLON_API progress_report
    {/*{{{*/
        public:
            long _total_size;
            long _cur_size;
            progress_report()
            {
                _total_size = 0;
                _cur_size = 0;
            }
            void set_total_size(long total_size)
            {
                _total_size = total_size;
            }
            void set_cur_size(long cur_size)
            { 
                _cur_size = cur_size;
            }
            void add_size(long cur_size)
            {
                _cur_size += cur_size;
            }
    };/*}}}*/


    class   PYLON_API call_result
    {/*{{{*/
        public:
            call_result();
            call_result(bool ret);
            void no_err();
            void set_err(const std::string& reason);
            void set_err(int err_code,const std::string& reason);
            operator bool () const
            {
                return _result;
            }
            std::stringstream* reason_stream();
            std::string reason();

            bool				_result;
            int					_err_code;
            //std::string		_reason;
            boost::shared_ptr<std::stringstream> _reason;

    };/*}}}*/


    class  PYLON_API remote_accessor : public smarty_ptr<remote_accessor> 
    {/*{{{*/
        public:
            virtual call_result get (const char * url, std::ostream& out,logger* logger=NULL ,int timeout=2,int port=80) = 0;	
            virtual call_result post(const char * url ,char* postname,  std::istream& content,std::ostream& outs,logger* logger=NULL, int timeout=10) = 0;
            virtual call_result post(const char * url ,char* postname,  const char *filename,std::ostream& outs,logger* logger=NULL, int timeout=10)=0;
            virtual ~remote_accessor(){};
    };/*}}}*/

    class  PYLON_API curl_accessor : public remote_accessor 
    {/*{{{*/
        public:
            curl_accessor(progress_report* ob = NULL);
            bool set_cookie(std::string cookie_file);
            bool set_cookiestr(std::string cookie_str);
            bool set_user_agent(std::string user_agent);
            bool set_referer(std::string referer);
            std::string get_store_cookie();
            virtual call_result get(const char* url, std::ostream& out , logger* logger=NULL ,int timeout=2 ,int port=80) ;	
            virtual call_result post(const char * url ,char* postname , std::istream& content,std::ostream& outs, logger* logger =NULL, int timeout=10 );
            virtual call_result post(const char * url ,char* postname ,const char *filename,std::ostream& outs ,logger* logger=NULL,int timeout=10);
            virtual call_result post(const char * url ,const char * input_field,std::ostream& outs,logger* logger=NULL,int timeout=10);
			PIMPL_DEF ;
    };/*}}}*/

	/*
    class  local_cache_racs_proxy : public remote_accessor
    {
        remote_accessor*		_ori_svc;
        std::wstring				_cache_path;
        int							_cache_sec;
        public:
        local_cache_racs_proxy(remote_accessor* ori_svc, const std::wstring& cache_path, int  cache_sec=600)
            :_cache_path(cache_path),_ori_svc(ori_svc),_cache_sec(cache_sec)
        { }
        virtual call_result get(const char* url, std::ostream& out , logger* logger=NULL, int timeout=20 );
        virtual call_result post(const char * url ,char* postname , std::istream& content,std::ostream& outs, logger* logger=NULL,int timeout=10);
        virtual call_result post(const char * url ,char* postname ,const char *filename,std::ostream& outs , logger* logger=NULL,int timeout=10);
        void get_from_cache(const wchar_t* data_path, std::ostream& out );
        std::wstring  std_cache_file_name(const char * url);


    };
	*/
}
#endif
