#include "common.h"
#include "common/exception_handler.h"
#include <DbgHelp.h>
#include <fstream>
#include <TlHelp32.h>
#pragma comment (lib,"dbghelp.lib")
namespace pylon
{

	static const wchar_t * GetExceptionDescription(DWORD ExceptionCode)
	{
		struct ExceptionNames
		{
			DWORD	ExceptionCode;
			TCHAR *	ExceptionName;
		};

#if 0  // from winnt.h
#define STATUS_WAIT_0                    ((DWORD   )0x00000000L)    
#define STATUS_ABANDONED_WAIT_0          ((DWORD   )0x00000080L)    
#define STATUS_USER_APC                  ((DWORD   )0x000000C0L)    
#define STATUS_TIMEOUT                   ((DWORD   )0x00000102L)    
#define STATUS_PENDING                   ((DWORD   )0x00000103L)    
#define STATUS_SEGMENT_NOTIFICATION      ((DWORD   )0x40000005L)    
#define STATUS_GUARD_PAGE_VIOLATION      ((DWORD   )0x80000001L)    
#define STATUS_DATATYPE_MISALIGNMENT     ((DWORD   )0x80000002L)    
#define STATUS_BREAKPOINT                ((DWORD   )0x80000003L)    
#define STATUS_SINGLE_STEP               ((DWORD   )0x80000004L)    
#define STATUS_ACCESS_VIOLATION          ((DWORD   )0xC0000005L)    
#define STATUS_IN_PAGE_ERROR             ((DWORD   )0xC0000006L)    
#define STATUS_INVALID_HANDLE            ((DWORD   )0xC0000008L)    
#define STATUS_NO_MEMORY                 ((DWORD   )0xC0000017L)    
#define STATUS_ILLEGAL_INSTRUCTION       ((DWORD   )0xC000001DL)    
#define STATUS_NONCONTINUABLE_EXCEPTION  ((DWORD   )0xC0000025L)    
#define STATUS_INVALID_DISPOSITION       ((DWORD   )0xC0000026L)    
#define STATUS_ARRAY_BOUNDS_EXCEEDED     ((DWORD   )0xC000008CL)    
#define STATUS_FLOAT_DENORMAL_OPERAND    ((DWORD   )0xC000008DL)    
#define STATUS_FLOAT_DIVIDE_BY_ZERO      ((DWORD   )0xC000008EL)    
#define STATUS_FLOAT_INEXACT_RESULT      ((DWORD   )0xC000008FL)    
#define STATUS_FLOAT_INVALID_OPERATION   ((DWORD   )0xC0000090L)    
#define STATUS_FLOAT_OVERFLOW            ((DWORD   )0xC0000091L)    
#define STATUS_FLOAT_STACK_CHECK         ((DWORD   )0xC0000092L)    
#define STATUS_FLOAT_UNDERFLOW           ((DWORD   )0xC0000093L)    
#define STATUS_INTEGER_DIVIDE_BY_ZERO    ((DWORD   )0xC0000094L)    
#define STATUS_INTEGER_OVERFLOW          ((DWORD   )0xC0000095L)    
#define STATUS_PRIVILEGED_INSTRUCTION    ((DWORD   )0xC0000096L)    
#define STATUS_STACK_OVERFLOW            ((DWORD   )0xC00000FDL)    
#define STATUS_CONTROL_C_EXIT            ((DWORD   )0xC000013AL)    
#define STATUS_FLOAT_MULTIPLE_FAULTS     ((DWORD   )0xC00002B4L)    
#define STATUS_FLOAT_MULTIPLE_TRAPS      ((DWORD   )0xC00002B5L)    
#define STATUS_ILLEGAL_VLM_REFERENCE     ((DWORD   )0xC00002C0L)     
#endif

		ExceptionNames ExceptionMap[] =
		{
			{0x40010005, L"a Control-C"},
			{0x40010008, L"a Control-Break"},
			{0x80000002, L"a Datatype Misalignment"},
			{0x80000003, L"a Breakpoint"},
			{0xc0000005, L"an Access Violation"},
			{0xc0000006, L"an In Page Error"},
			{0xc0000017, L"a No Memory"},
			{0xc000001d, L"an Illegal Instruction"},
			{0xc0000025, L"a Noncontinuable Exception"},
			{0xc0000026, L"an Invalid Disposition"},
			{0xc000008c, L"a Array Bounds Exceeded"},
			{0xc000008d, L"a Float Denormal Operand"},
			{0xc000008e, L"a Float Divide by Zero"},
			{0xc000008f, L"a Float Inexact Result"},
			{0xc0000090, L"a Float Invalid Operation"},
			{0xc0000091, L"a Float Overflow"},
			{0xc0000092, L"a Float Stack Check"},
			{0xc0000093, L"a Float Underflow"},
			{0xc0000094, L"an Integer Divide by Zero"},
			{0xc0000095, L"an Integer Overflow"},
			{0xc0000096, L"a Privileged Instruction"},
			{0xc00000fD, L"a Stack Overflow"},
			{0xc0000142, L"a DLL Initialization Failed"},
			{0xe06d7363, L"a Microsoft C++ Exception"},
		};

		for (int i = 0; i < sizeof(ExceptionMap) / sizeof(ExceptionMap[0]); i++)
			if (ExceptionCode == ExceptionMap[i].ExceptionCode)
				return ExceptionMap[i].ExceptionName;

		return L"an Unknown exception type";
	}

	std::wstring int2hex(DWORD num ,int len=8)
	{
		wchar_t hexstr[MAX_PATH] = L"";
		if (len == 8)
		{
			swprintf_s(hexstr,MAX_PATH,L"%08x",num);
		}
		else
		{
			std::wstring fmt = L"%0"+str2wstr(int2str(len))+L"x";
			swprintf_s(hexstr,MAX_PATH,fmt.c_str(),num);
		}
		return hexstr;
	}

	int exception_handler( PEXCEPTION_POINTERS p_exception,const wchar_t * log_filepath )
	{
		static bool first_time = true;
		if(!first_time)
			return EXCEPTION_CONTINUE_SEARCH;
		first_time = false;

		std::wofstream of(log_filepath,std::ios::binary);
		of.imbue(std::locale("chs")); 
		if (!of.good())
			return EXCEPTION_CONTINUE_SEARCH;
		of.clear();
		//»ñÈ¡ËÞÖ÷³ÌÐòÂ·¾¶
		wchar_t app_name[MAX_PATH] = L"";
		if (GetModuleFileName(0, app_name, sizeof(app_name)) <= 0)
			lstrcpy(app_name, L"Unknown");

		PEXCEPTION_RECORD record = p_exception->ExceptionRecord;
		PCONTEXT          context= p_exception->ContextRecord;


		//»ñÈ¡Òì³£Ä£¿éÂ·¾¶
		MEMORY_BASIC_INFORMATION MemInfo;
		wchar_t module_name[MAX_PATH] = L"";

		if(VirtualQuery((void*)context->Eip ,&MemInfo,sizeof(MemInfo)) != 0)
		{
			GetModuleFileName((HINSTANCE)MemInfo.AllocationBase,module_name,sizeof(module_name));
		}

		std::wstring major_msg = std::wstring(app_name) + std::wstring(L" caused ") + GetExceptionDescription(record->ExceptionCode) + L" (0x"+ L"0x"+int2hex(record->ExceptionCode)+std::wstring(L") \r\n")+
			L"in module "+ std::wstring(module_name) +L" at 0x"+int2hex(context->SegCs)+L":0x"+int2hex(context->Eip) + L"\r\n";
		of << major_msg;
		of.close();
		return EXCEPTION_CONTINUE_SEARCH;
	}
	
	std::wstring get_user_info()
	{
		wchar_t win_user[MAX_PATH];	
		DWORD namesize = sizeof(win_user);
		if(!::GetUserName(win_user,&namesize))
		{
			return std::wstring(win_user);
		}
		return L"";
	}

	std::wstring get_os_info()
	{
		OSVERSIONINFO osinfo;
		osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if (::GetVersionEx(&osinfo))
		{
			std::string ver = " unknown Windows version";
			if(osinfo.dwMajorVersion == 5)
			{
				if (osinfo.dwMinorVersion == 0)
				{
					ver = "Windows 2000";
				}else if(osinfo.dwMinorVersion == 1)
				{
					ver = "Windows XP";
				}else if(osinfo.dwMinorVersion == 2)
				{
					ver = "Windows 2003";
				}
			}else if(osinfo.dwMajorVersion == 6)
			{
				if(osinfo.dwMinorVersion == 0)
				{
					ver = "Windows Vista";
				}else if(osinfo.dwMinorVersion == 1)
				{
					ver = "Windows 7";
				}
			}
			std::string os_info = ver + " -- "+  int2str(osinfo.dwMajorVersion)+"."+int2str(osinfo.dwMinorVersion)+"."+int2str(osinfo.dwBuildNumber)+"[platformId:"+int2str(osinfo.dwPlatformId)+"]";
			return str2wstr(os_info);
		}
		return L"";
	}

	std::wstring get_cpu_info()
	{
		SYSTEM_INFO	SystemInfo;
		GetSystemInfo(&SystemInfo);

		std::string retn = int2str(SystemInfo.dwNumberOfProcessors) + " processor(s) ,type "+int2str(SystemInfo.dwProcessorType) ; 
		return str2wstr(retn);
	}

	std::wstring get_register_info(PCONTEXT context)
	{
		std::wstring retn =L"EIP:0x"+int2hex(context->Eip) +L"\r\nEAX:0x"+int2hex(context->Eax)+L"\r\nECX:0x"+int2hex(context->Ecx) + L"\r\nEDX:0x"+int2hex(context->Edx)+L"\r\nEBX:0x"+int2hex(context->Ebx)+
			L"\r\nESP:0x"+int2hex(context->Esp)+L"\r\nEBP:0x"+int2hex(context->Ebp)+L"\r\nESI:0x"+int2hex(context->Esi)+L"\r\nEDI:0x"+int2hex(context->Edi);
		return retn;
	}

	std::wstring get_bytes_at_eip(DWORD dwEip,int codesize)
	{
		std::wstring retn =L"" ;
		for (int i = 0; i < codesize - 1; i++)
		{
			BYTE  code = *(BYTE *)(dwEip+i);
			retn = retn + int2hex((DWORD)code ,2)+L" ";
		}
		return retn;
	}

	std::wstring get_file_version(std::wstring filepath)
	{
		std::wstring retn = L"";
		DWORD infosize = ::GetFileVersionInfoSize(filepath.c_str(),NULL);		
		if (infosize > 0)
		{
			wchar_t * buf = new wchar_t(infosize+1);
			ZeroMemory(buf,infosize+1);
			if(::GetFileVersionInfo(filepath.c_str(),NULL,infosize,buf))
			{
				struct LANGANDCODEPAGE 
				{
					WORD wLanguage;
					WORD wCodePage;
				} * pTranslation;
				DWORD lpSize;
				wchar_t * pstr;
				if(::VerQueryValue(buf,L"\\VarFileInfo\\Translation",(LPVOID*)&pTranslation,(PUINT)&lpSize))
				{
					std::wstring get_filever = L"\\StringFileInfo\\"+int2hex(pTranslation->wLanguage,4)+int2hex(pTranslation->wCodePage,4)+L"\\FileVersion";
					::VerQueryValue(buf,get_filever.c_str(),(LPVOID*)&pstr,(PUINT)&lpSize);
					retn =  std::wstring(pstr);
				}
			}
			delete []buf;
		}
		return retn;
	}

	std::wstring get_file_modtime(std::wstring filepath)
	{
		std::wstring retn = L"";
		WIN32_FIND_DATA   wfd;
		SYSTEMTIME   systime; 
		FILETIME   localtime; 
		HANDLE hFile;
		wchar_t stime[MAX_PATH] = L"";
		if(((hFile=FindFirstFile(filepath.c_str(),   &wfd))==INVALID_HANDLE_VALUE))
			return retn;
		FileTimeToLocalFileTime(&wfd.ftLastWriteTime,&localtime); 
		FileTimeToSystemTime(&localtime,&systime); 
		swprintf_s(stime, L"%4d-%02d-%02d   %02d:%02d:%02d ", 
			systime.wYear,systime.wMonth,systime.wDay,systime.wHour, 
			systime.wMinute,systime.wSecond);
		retn = stime;
		return retn;
	}

	std::wstring get_module_info()
	{
		std::wstring retn = L"";
		MODULEENTRY32 lppe;
		lppe.dwSize = sizeof(MODULEENTRY32);
		HANDLE hsnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
		if (hsnap != INVALID_HANDLE_VALUE)
		{
			bool bfound = ::Module32First(hsnap,&lppe);
			while(bfound)
			{
				retn = retn +L"[0x"+int2hex((int)lppe.hModule)+L",0x"+int2hex((int)lppe.modBaseSize)+L"]"+ std::wstring(lppe.szExePath)/*+L"["+get_file_version(lppe.szExePath)+L"  "+get_file_modtime(lppe.szExePath)+L"]"*/+L"\r\n" ;
				bfound = ::Module32Next(hsnap,&lppe);
			}
			::CloseHandle(hsnap);
		}
		return retn;
	}
	
	std::wstring get_stack_info(PCONTEXT pContext)
	{
		std::wstring retn = L"";
		HANDLE        hProcess;
		hProcess = GetCurrentProcess();
		if (!SymInitialize(GetCurrentProcess(),NULL,TRUE))
		{
			return retn ;
		}
		STACKFRAME64 sf64;
		ZeroMemory(&sf64, sizeof(sf64));

		DWORD dwMachineType;
		sf64.AddrPC.Mode = sf64.AddrFrame.Mode = sf64.AddrStack.Mode  = AddrModeFlat;

#ifdef _M_IX86
		dwMachineType = IMAGE_FILE_MACHINE_I386;
		sf64.AddrPC.Offset = pContext->Eip;
		sf64.AddrFrame.Offset = pContext->Ebp;
		sf64.AddrStack.Offset = pContext->Esp;
#elif _M_AMD64
		dwMachineType = IMAGE_FILE_MACHINE_AMD64;
		sf64.AddrPC.Offset = pContext->Rip;
		sf64.AddrFrame.Offset = pContext->Rsp;
		sf64.AddrStack.Offset = pContext->Rsp;
#elif _M_IA64
		dwMachineType = IMAGE_FILE_MACHINE_IA64;
		sf64.AddrPC.Offset = pContext->StIIP;
		sf64.AddrFrame.Offset = pContext->IntSp;
		sf64.AddrBStore.Offset = pContext->RsBSP;
		sf64.AddrStack.Offset = pContext->IntSp;
#else
#error "Platform not supported!"
#endif

		while (true)
		{
			if (!::StackWalk64(dwMachineType,hProcess,NULL,&sf64,(LPVOID)pContext,NULL,SymFunctionTableAccess64,SymGetModuleBase64,NULL))
			{
				break;
			}
			if (0==sf64.AddrStack.Offset)
				break;
			ULONG64 buffer[(sizeof(SYMBOL_INFO)+MAX_SYM_NAME*sizeof(TCHAR)+sizeof(ULONG64)-1)/sizeof(ULONG64)];
			PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;
			pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
			pSymbol->MaxNameLen = MAX_SYM_NAME;

			DWORD64 dwDisplacement = 0;
			if (SymFromAddr(hProcess, sf64.AddrPC.Offset, &dwDisplacement, pSymbol))
				retn = retn + int2hex(sf64.AddrPC.Offset)+L"  "+int2hex(sf64.AddrFrame.Offset)+L"  "+str2wstr(pSymbol->Name)+L"+ "+ int2hex(dwDisplacement)+L"\r\n";
				//PutDebugString(TEXT("%p\t%p\t%hs+0x%X\r\n"), (LPCVOID)sf64.AddrPC.Offset, (LPCVOID)sf64.AddrFrame.Offset, pSymbol->Name, dwDisplacement);
			else
				retn = retn + int2hex(sf64.AddrPC.Offset)+L"  "+int2hex(sf64.AddrFrame.Offset)+L" SymFromAddr gle : "+ int2hex(GetLastError())+L"\r\n";
				//PutDebugString(TEXT("%p\t%p\tSymFromAddr gle: %d\r\n"), (LPCVOID)sf64.AddrPC.Offset, (LPCVOID)sf64.AddrFrame.Offset, GetLastError());
		}
		if (!SymCleanup(hProcess))
		{
			return retn;
		}
		return retn;
	}


	std::wstring save_log( std::wstring & logfile,PEXCEPTION_POINTERS pException )
	{
		std::wofstream of(logfile.c_str(),std::ios::binary);
		of.imbue(std::locale("chs")); 
		if (!of.good())
			return L"";
		of.clear();
		//»ñÈ¡ËÞÖ÷³ÌÐòÂ·¾¶
		wchar_t app_name[MAX_PATH] = L"";
		if (GetModuleFileName(0, app_name, sizeof(app_name)) <= 0)
			lstrcpy(app_name, L"Unknown");
		std::wstring app_path = std::wstring(app_name);

		PEXCEPTION_RECORD record = pException ->ExceptionRecord;
		PCONTEXT          context= pException ->ContextRecord;

		//»ñÈ¡Òì³£Ä£¿éÂ·¾¶
		MEMORY_BASIC_INFORMATION MemInfo;
		wchar_t module_name[MAX_PATH] = L"";

		if(VirtualQuery((void*)context->Eip ,&MemInfo,sizeof(MemInfo)) != 0)
		{
			GetModuleFileName((HINSTANCE)MemInfo.AllocationBase,module_name,sizeof(module_name));
		}
		std::wstring time_info = L"Exception Log CreateTime:"+ str2wstr(time_t2str(now())) + L"\r\n";
		std::wstring major_info= app_path + std::wstring(L" caused ") + GetExceptionDescription(record->ExceptionCode) + L" (0x"+ L"0x"+int2hex(record->ExceptionCode)+std::wstring(L") \r\n")+
			L"in module "+ std::wstring(module_name) +L" at 0x"+int2hex(context->SegCs)+L":0x"+int2hex(context->Eip) + L"\r\n";
		std::wstring user_info = app_path +L" run by " + get_user_info()+L"\r\n"; 
		std::wstring os_info = L"Operation System:" + get_os_info() +L"\r\n"; 
		std::wstring cpu_info = L"Machine's CPU information:"+get_cpu_info()+L"\r\n";
		std::wstring register_info = get_register_info(context)+L"\r\n\r\n";
		std::wstring eip_byte = L"Bytes at CS:EIP:"+get_bytes_at_eip(context->Eip,0x20)+L"\r\n\r\n";
		std::wstring stack_info = L"CallStacks:\r\n" +  get_stack_info(context)+L"\r\n\r\n";
		std::wstring module_info = L"Modules: \r\n"+get_module_info()+L"\r\n";
		//yyStackWalker _stack;
		//_stack.ShowCallstack(GetCurrentThread(),context);

		std::wstring all_msg = time_info + major_info + user_info + os_info + cpu_info + register_info + eip_byte + stack_info + module_info ;

		of << all_msg;
		of.close();
		return all_msg;
	}

	void save_dump( std::wstring & dumpfile,PEXCEPTION_POINTERS pException )
	{
		HANDLE hdump_file = ::CreateFile(dumpfile.c_str(), GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_WRITE|FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0); 

		MINIDUMP_TYPE MiniDumpWithDataSegs =(MINIDUMP_TYPE)( MiniDumpNormal
			| MiniDumpWithHandleData
			| MiniDumpWithUnloadedModules
			| MiniDumpWithIndirectlyReferencedMemory
			| MiniDumpScanMemory
			| MiniDumpWithProcessThreadData
			| MiniDumpWithThreadInfo); 
		MINIDUMP_EXCEPTION_INFORMATION ExpParam; 
		ExpParam.ThreadId = ::GetCurrentThreadId();
		ExpParam.ExceptionPointers = pException;
		ExpParam.ClientPointers = TRUE; 
		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hdump_file, MiniDumpWithDataSegs, &ExpParam, NULL, NULL); 
		::CloseHandle(hdump_file);
	}

}
