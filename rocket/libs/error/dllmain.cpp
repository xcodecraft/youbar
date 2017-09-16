// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "error/log_boost.h"
#include "windows.h"
using namespace pylon ;
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		log_def::clear();
		break;
	}
	return TRUE;
}

