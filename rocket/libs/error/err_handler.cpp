#include "common.h"
#include "common/err_handler.h"

void pylon::err_hander::jscmd_call( const char * call_name , jscmd_call_t call, std::vector<std::wstring>* args ,xlog* log)
{/*{{{*/
	_DBG_LOG(log)<< "jscmd: " << call_name;

	try
	{
		return call(args);
	}
	catch(pylon::pylon_err &e )
	{

		std::wstringstream serr;
		serr << boost::diagnostic_information(e).c_str();
		_ERR_LOG(log) << boost::diagnostic_information(e).c_str();

	}
	catch(std::exception &e)
	{
		std::wstringstream serr;
		serr << e.what() ;
		_ERR_LOG(log) <<  e.what();
	}
	catch(...)
	{
		_ERR_LOG(log) <<  "UNKONW ERR";
	}
	return ;
}/*}}}*/

bool pylon::err_hander::com_call_proc( cube_call call,xlog* log )
{/*{{{*/
	try
	{
		return call();
	}
	catch(pylon::pylon_err &e )
	{

		std::wstringstream serr;
		serr << boost::diagnostic_information(e).c_str();
		_ERR_LOG(log) << boost::diagnostic_information(e).c_str();

	}
	catch(std::exception &e)
	{
		std::wstringstream serr;
		serr << e.what() ;
		_ERR_LOG(log) <<  e.what();
	}
	catch(...)
	{
		_ERR_LOG(log) <<  "UNKONW ERR";
	}
	return false;
}/*}}}*/

bool pylon::err_hander::call_proc( void_call call,err_prompt* notice, xlog * log)
{/*{{{*/

	try
	{
		call();
		return true;
	}
	catch(pylon::remote_err& e)
	{
		std::wstringstream serr;
		_ERR_LOG(log) << boost::diagnostic_information(e).c_str();
		notice->reason(L"ÔÝÊ±²»ÄÜ·ÃÎÊÔÆ¶Ë·þÎñ");
		notice->debug_msg(str2wstr(boost::diagnostic_information(e)).c_str());
		notice->show();
	}
	catch(pylon::pylon_err &e )
	{
		std::wstringstream serr;
		serr << boost::diagnostic_information(e).c_str();
		_ERR_LOG(log) << boost::diagnostic_information(e).c_str();
		notice->reason(e.what());
		notice->debug_msg(str2wstr(boost::diagnostic_information(e)).c_str());
		notice->show();
	}
	catch(std::exception &e)
	{
		_ERR_LOG(log) <<  e.what();
		notice->reason(str2wstr(e.what()).c_str());
		notice->debug_msg(str2wstr(e.what()).c_str());
		notice->show();
	}
	catch(...)
	{
		_ERR_LOG(log) <<  "UNKONW ERR";
		notice->reason(L"Î´ÖªµÄÔ­Òò");
		notice->debug_msg(L"");
		notice->show();

	}
	return false;
}/*}}}*/


bool pylon::err_hander::call_proc( void_call call,xlog* log)
{/*{{{*/

	try
	{
		call();
		return true;
	}
	catch(pylon::remote_err& e)
	{
		std::wstringstream serr;
		_ERR_LOG(log) << boost::diagnostic_information(e).c_str();
	}
	catch(pylon::pylon_err &e )
	{

		std::wstringstream serr;
		serr << boost::diagnostic_information(e).c_str();
		_ERR_LOG(log) << boost::diagnostic_information(e).c_str();

	}
	catch(std::exception &e)
	{
		std::wstringstream serr;
		serr << e.what() ;
		_ERR_LOG(log) <<  e.what();
	}
	catch(...)
	{
		_ERR_LOG(log) <<  "UNKONW ERR";
	}
	return false;
}/*}}}*/



void pylon::err_hander::void_call_proc( void_call call,xlog* log)
{/*{{{*/
    call_proc(call,log);
}/*}}}*/
