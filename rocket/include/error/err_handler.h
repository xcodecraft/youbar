#pragma  once
#include "common/log.h"
#include <boost/function.hpp>
/*
#define MB_ERROR 100
#define MB_WARN 101
#define MB_COMPLETE 102
*/
namespace pylon
{

	class prompt
	{
	public:
		static const int  BOX_ERROR = 0x000000003;
		static const int  BOX_WARN = 0x000000002;
		static const int  BOX_COMPLETE = 0x000000001;
		static const int  BOX_YESNO = 0x000000010;
	public:
		virtual void describe(const wchar_t* msg,int msg_type=0)=0;
		virtual void show()=0;
		virtual bool chosed_yes()=0;
		virtual ~prompt(){}
	};
	class err_prompt : public prompt 
	{
	public:
		virtual void reason(const wchar_t*  msg)=0;
		virtual void debug_msg(const wchar_t *msg)=0;
		virtual ~err_prompt(){}
	};
	
	typedef boost::function<void (const wchar_t*  ) >  notice_call  ; 
	class err_hander
	{
	public :
		typedef boost::function<void (std::vector<std::wstring>* args)> jscmd_call_t; 
		typedef boost::function<bool ( void  )>  cube_call; 
		typedef boost::function<void ()>  void_call; 
		static void jscmd_call(const char * call_name , err_hander::jscmd_call_t call, std::vector<std::wstring>* args,xlog * log);
		static bool com_call_proc(cube_call call, xlog* log=NULL);
		static bool call_proc(void_call call,err_prompt* notice, xlog * log);
		static bool call_proc(void_call call,xlog* log);
		static void void_call_proc(void_call call,xlog * log);
	};
    typedef  err_hander  err_hendler ;
}
