#ifndef EXCEPTIONHANDLER_H
#define EXCEPTIONHANDLER_H
//#pragma once
#include <stdio.h>
using namespace std;
namespace pylon
{
	typedef struct _EXCEPTION_POINTERS EXCEPTION_POINTERS, *PEXCEPTION_POINTERS;
	int exception_handler(PEXCEPTION_POINTERS p_exception,const wchar_t * log_filepath);

	//std::wstring except_file = get_exception_file();

	std::wstring save_log(std::wstring & logfile,PEXCEPTION_POINTERS pException);
	void save_dump(std::wstring & dumpfile,PEXCEPTION_POINTERS pException);
}



#endif
