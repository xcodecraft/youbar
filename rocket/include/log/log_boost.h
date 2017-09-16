#ifndef __LOG_WIN_HPP__
#define __LOG_WIN_HPP__
#define BOOST_LOG_USE_WCHAR_T
#include "port.h"
#include <boost/log/common.hpp>


namespace pylon 
{

	struct PYLON_API log_kit{
		enum level_t  { debug=0, info5,info4,info3,info2,info1,info, warnning, error, fatal };
		typedef boost::log::sources::wseverity_logger< log_kit::level_t > logger ;
		static logger* log_ins(const wchar_t * name ); 
		static void conf(const wchar_t* name ,const wchar_t* path , level_t l); 
		static void clear(); 
	};
	typedef log_kit::logger logger ;

	#define WLOG_DEBUG(log) if(log != NULL)  BOOST_LOG_SEV(*log , log_kit::debug  )        << L"[dbg: "     << __FUNCTION__ << L"] : " 
	#define WLOG_INFO(log)  if(log != NULL)  BOOST_LOG_SEV(*log , log_kit::info  )         << L"[info: "   << __FUNCTION__ << L"] : " 
	#define WLOG_INFO1(log)  if(log != NULL)  BOOST_LOG_SEV(*log , log_kit::info1  )       << L"[info1] : "
	#define WLOG_INFO2(log)  if(log != NULL)  BOOST_LOG_SEV(*log , log_kit::info2  )       << L"[info2] : " 
	#define WLOG_INFO3(log)  if(log != NULL)  BOOST_LOG_SEV(*log , log_kit::info3  )       << L"[info3] : " 
	#define WLOG_INFO4(log)  if(log != NULL)  BOOST_LOG_SEV(*log , log_kit::info4  )       << L"[info4] : " 
	#define WLOG_INFO5(log)  if(log != NULL)  BOOST_LOG_SEV(*log , log_kit::info5  )       << L"[info5] : " 
	#define WLOG_WARN(log)  if(log != NULL)  BOOST_LOG_SEV(*log , log_kit::warnning  )     << L"[warning: " << __FUNCTION__ << L"] : " 
	#define WLOG_ERROR(log)   if(log != NULL)  BOOST_LOG_SEV(*log , log_kit::error)        << L"[err: "     << __FUNCTION__ << L"] : " 
	#define WLOG_FATAL(log) if(log != NULL)  BOOST_LOG_SEV(*log , log_kit::fatal  )        << L"[fatal: "   << __FUNCTION__ << L"] : " 
	#define INS_LOG(x,logname)    log_kit::logger* x=log_kit::log_ins(logname);

}
#endif
