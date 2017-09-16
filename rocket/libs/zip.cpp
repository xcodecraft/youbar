#include "common.h"
#include "context/context.h"
#include "baselib/zip.h"
#include "boost/filesystem.hpp"
#include <shellapi.h>
using namespace std;
using namespace boost;
using namespace yyou;
namespace bsf=boost::filesystem ;

bool zip_utls::zip_dir( const std::wstring& dir, const std::wstring& zipfile )
{
	xlogger_holder* log = thread_context::get<xlogger_holder*>(XLOG,NULL);
	wstringstream params_stream;
	params_stream << L" a "   << L"\"" << std_path(wstring(zipfile))  << L"\"   \""  << std_path(wstring(dir))  << L"/*\" -y ";
	wstring params = params_stream.str();
	_DBG_LOG(log) << "zip_dir cmd:"  <<  wstr2str(_zip_exe).c_str() << wstr2str( params).c_str();
	SHELLEXECUTEINFO   sei;   
	ZeroMemory(&sei,   sizeof(SHELLEXECUTEINFO));   
	sei.cbSize   =   sizeof(SHELLEXECUTEINFO);   
	sei.fMask   =   SEE_MASK_NOCLOSEPROCESS;   
	sei.hwnd   =   NULL;   
	sei.lpVerb   =   NULL;   
	sei.lpFile   = _zip_exe.c_str();
	sei.lpParameters   =   params.c_str() ;
	sei.lpDirectory   =   NULL;   
	sei.nShow   =   SW_HIDE;   
	sei.hInstApp   =   NULL;   
	ShellExecuteEx(&sei);   
	WaitForSingleObject(sei.hProcess,   INFINITE);   
	TerminateProcess(sei.hProcess,   0); 
	return true;
}

bool zip_utls::unzip( const std::wstring& zipfile, const std::wstring& dist )
{
	xlogger_holder* log = thread_context::get<xlogger_holder*>(XLOG,NULL);
	wstringstream params_stream;
	params_stream << L" x "   << L"\"" << std_path(wstring(zipfile)) << L"\"  -o\""  << std_path(wstring(dist))   << L"\" -y";
	wstring params = params_stream.str();
	_DBG_LOG(log) << "zip_dir cmd:"  <<  wstr2str(_zip_exe).c_str() << wstr2str( params).c_str();
	SHELLEXECUTEINFO   sei;   
	ZeroMemory(&sei,   sizeof(SHELLEXECUTEINFO));   
	sei.cbSize   =   sizeof(SHELLEXECUTEINFO);   
	sei.fMask   =   SEE_MASK_NOCLOSEPROCESS;   
	sei.hwnd   =   NULL;   
	sei.lpVerb   =   NULL;   
	sei.lpFile   = _zip_exe.c_str();
	sei.lpParameters   =   params.c_str() ;
	sei.lpDirectory   =   NULL;   
	sei.nShow   =   SW_HIDE;   
	sei.hInstApp   =   NULL;   
	ShellExecuteEx(&sei);   
	WaitForSingleObject(sei.hProcess,   INFINITE);   
	TerminateProcess(sei.hProcess,   0); 
	return true;

}

zip_utls::zip_utls( std::wstring path )
{
	
	_zip_exe=  path +  L"/7zr.exe " ;
}

zip_utls::zip_utls()
{

	wchar_t buf[1024];
	memset(buf,0,1024);
	GetModuleFileName(NULL,buf,1024);
	bsf::wpath  proc(buf);
	//_zip_exe=proc.parent_path().string();
	_zip_exe += L"/7zr.exe";
	_zip_exe  =  L"\"" + _zip_exe  + L"\"";
	throw yyou_err(L"no impl");
}

