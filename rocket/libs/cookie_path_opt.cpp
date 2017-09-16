#include "common.h"
#include "baselib/cookie_path_opt.h"
using namespace yyou;
typedef LONG (WINAPI __pfnRegQueryValueExW(HKEY hKey,
			  LPCTSTR lpValueName,
			  LPDWORD lpReserved,
			  LPDWORD lpType,
			  LPBYTE lpData,
			  LPDWORD lpcbData));

typedef LONG (WINAPI __pfnRegQueryValueExA(HKEY hKey,
			  LPCSTR lpValueName,
			  LPDWORD lpReserved,
			  LPDWORD lpType,
			  LPBYTE lpData,
			  LPDWORD lpcbData));

LONG WINAPI My_RegQueryValueExW(DWORD RetAddr,
								__pfnRegQueryValueExW pfnRegQueryValueEx,
								HKEY hKey,
								LPCTSTR lpValueName,
								LPDWORD lpReserved,
								LPDWORD lpType,
								LPBYTE lpData,
								LPDWORD lpcbData);
LONG WINAPI My_RegQueryValueExA(DWORD RetAddr,
								__pfnRegQueryValueExA pfnRegQueryValueEx,
								HKEY hKey,
								LPCSTR lpValueName,
								LPDWORD lpReserved,
								LPDWORD lpType,
								LPBYTE lpData,
								LPDWORD lpcbData);

std::wstring __cookie_path = L""; //cookie 路径字符串
std::wstring __cache_path = L""; //cache 路径字符串
std::wstring __appdata_path = L""; //appdata 路径字符串

PHOOKENVIRONMENT pHookEnv; //hook过的 
PHOOKENVIRONMENT pHookEnvA; //hook过的 
LONG WINAPI My_RegQueryValueExA(DWORD RetAddr,
								__pfnRegQueryValueExA pfnRegQueryValueEx,
								HKEY hKey,
								LPCSTR lpValueName,
								LPDWORD lpReserved,
								LPDWORD lpType,
								LPBYTE lpData,
								LPDWORD lpcbData)
{
	if(lpValueName)
	{
		if (__cache_path!=L"" && strcmp(lpValueName,"Cache") == 0)
		{
			int nlen = (__cache_path.size()+1) ;
			//LONG retn = pfnRegQueryValueEx(hKey,lpValueName,lpReserved,lpType,lpData,lpcbData);
			if(nlen < (int)(*lpcbData))
			{
				if(lpType) *lpType = REG_EXPAND_SZ; 
				if(lpcbData) *(DWORD*)lpcbData = nlen;
				if(lpcbData) memcpy(lpData,wstr2str(__cache_path,char_encode::win_sys).c_str(),nlen);
				return ERROR_SUCCESS;
			}
			return  ERROR_MORE_DATA;
		}
		if ( __cookie_path!=L"" && strcmp(lpValueName,"Cookies") == 0)
		{
			int nlen = (__cookie_path.size()+1) ;
			//LONG retn = pfnRegQueryValueEx(hKey,lpValueName,lpReserved,lpType,lpData,lpcbData);
 			if(nlen < (int)(*lpcbData))
			{
				if(lpType) *lpType = REG_EXPAND_SZ; 
				if(lpcbData) *(DWORD*)lpcbData = nlen;
				if(lpData) memcpy(lpData,wstr2str(__cookie_path,char_encode::win_sys).c_str(),nlen);
				return ERROR_SUCCESS;
			}
			return  ERROR_MORE_DATA ;
		}
		if (__appdata_path!=L"" && strcmp(lpValueName,"AppData") == 0)
		{
			int nlen = (__appdata_path.size()+1) ;
			//LONG retn = pfnRegQueryValueEx(hKey,lpValueName,lpReserved,lpType,lpData,lpcbData);
			if (nlen < (int)(*lpcbData))
			{
				if(lpType) *lpType = REG_EXPAND_SZ; 
				if(lpcbData) *(DWORD*)lpcbData = nlen;
				if(lpData) memcpy(lpData,wstr2str(__appdata_path,char_encode::win_sys).c_str(),nlen);
				return ERROR_SUCCESS;
			}
			return  ERROR_MORE_DATA;
		}
	}
	return pfnRegQueryValueEx(hKey,lpValueName,lpReserved,lpType,lpData,lpcbData);
}

LONG WINAPI My_RegQueryValueExW(DWORD RetAddr,
								__pfnRegQueryValueExW pfnRegQueryValueEx,
								HKEY hKey,
								LPCTSTR lpValueName,
								LPDWORD lpReserved,
								LPDWORD lpType,
								LPBYTE lpData,
								LPDWORD lpcbData)
{
	if(lpValueName)
	{
		if (__cache_path!=L"" && wcscmp(lpValueName,L"Cache") == 0)
		{
			int nlen = (__cache_path.size()+1) * sizeof(wchar_t);
			//LONG retn = pfnRegQueryValueEx(hKey,lpValueName,lpReserved,lpType,lpData,lpcbData);
			if(nlen < (int)(*lpcbData))
			{
				if(lpType) *lpType = REG_EXPAND_SZ; 
				if(lpcbData) *(DWORD*)lpcbData = nlen;
				if(lpcbData) memcpy(lpData,__cache_path.c_str(),nlen);
				debug_log(wstr2str(L"Cache目录:"+__cache_path,char_encode::win_sys).c_str());
				return ERROR_SUCCESS;
			}
			return  ERROR_MORE_DATA;
		}
		if ( __cookie_path!=L"" && wcscmp(lpValueName,L"Cookies") == 0)
		{
			int nlen = (__cookie_path.size()+1) * sizeof(wchar_t);
			//LONG retn = pfnRegQueryValueEx(hKey,lpValueName,lpReserved,lpType,lpData,lpcbData);
 			if(nlen < (int)(*lpcbData))
			{
				if(lpType) *lpType = REG_EXPAND_SZ; 
				if(lpcbData) *(DWORD*)lpcbData = nlen;
				if(lpData) memcpy(lpData,__cookie_path.c_str(),nlen);
				debug_log(wstr2str(L"Cookies目录:"+__cookie_path,char_encode::win_sys).c_str());
				return ERROR_SUCCESS;
			}
			return  ERROR_MORE_DATA ;
		}
		if (__appdata_path!=L"" && wcscmp(lpValueName,L"AppData") == 0)
		{
			int nlen = (__appdata_path.size()+1) * sizeof(wchar_t);
			//LONG retn = pfnRegQueryValueEx(hKey,lpValueName,lpReserved,lpType,lpData,lpcbData);
			if (nlen < (int)(*lpcbData))
			{
				if(lpType) *lpType = REG_EXPAND_SZ; 
				if(lpcbData) *(DWORD*)lpcbData = nlen;
				if(lpData) memcpy(lpData,__appdata_path.c_str(),nlen);
				debug_log(wstr2str(L"AppData目录:"+__appdata_path,char_encode::win_sys).c_str());
				return ERROR_SUCCESS;
			}
			return  ERROR_MORE_DATA;
		}
	}
	return pfnRegQueryValueEx(hKey,lpValueName,lpReserved,lpType,lpData,lpcbData);
}

/*
bool install_cookie_path(std::wstring path)
{
	__cookie_path = path;
	pHookEnv = InstallHookApi(L"Advapi32.dll","RegQueryValueExW",My_RegQueryValueExW);
	return true;
}
bool install_cache_path(std::wstring path)
{
	UnInstallHookApi(pHookEnv);
	return true;
}

bool uninstall_cookie_path()
{
	UnInstallHookApi(pHookEnv);
	return true;
}
*/

ie_hook_svc * ie_hook_svc::ins()
{
	static ie_hook_svc svc;
	return &svc;
}

void ie_hook_svc::set_cookie_path(std::wstring path)
{
	__cookie_path = path;
}

void ie_hook_svc::set_cache_path(std::wstring path)
{
	__cache_path = path;
}

void ie_hook_svc::set_flash_cookie_path( std::wstring path )
{
	__appdata_path = path;
}

bool is_hook_ok(LPCTSTR DllName,LPCSTR ApiName)
{
	HMODULE DllHandle = ::GetModuleHandle(DllName);
	if(DllHandle != NULL)
	{
		PVOID pfunc = ::GetProcAddress(DllHandle,ApiName);
		if(pfunc != NULL)
		{
			if(*(BYTE*)pfunc == 0xE9)
			{
				return true;
			}
			else
			{
				return false;
			}

		}
	}
	return false;
		
}

bool ie_hook_svc::install_hook()
{
	//pHookEnv = InstallHookApi(L"Advapi32.dll","RegQueryValueExW",My_RegQueryValueExW);
	//yyou::debug_log(std::string("hookon begin  " + yyou::wstr2str(__cookie_path) ).c_str());
	if (is_os_nt5())
	{
		//yyou::debug_log("hookon nt5");
		pHookEnv = InstallHookApi(L"Advapi32.dll","RegQueryValueExW",My_RegQueryValueExW);
		pHookEnvA = InstallHookApi(L"Advapi32.dll","RegQueryValueExA",My_RegQueryValueExA);
		return is_hook_ok(L"Advapi32.dll","RegQueryValueExW");
	}
	else
	{
		//yyou::debug_log("hookon nt6");
		pHookEnv = InstallHookApi(L"kernel32.dll","RegQueryValueExW",My_RegQueryValueExW);
		pHookEnvA = InstallHookApi(L"kernel32.dll","RegQueryValueExA",My_RegQueryValueExA);
		return is_hook_ok(L"kernel32.dll","RegQueryValueExW");
	}
	//yyou::debug_log("hookon end");
	return false;
}

bool ie_hook_svc::uninstall_hook()
{
	UnInstallHookApi(pHookEnv);
	UnInstallHookApi(pHookEnvA);
	return true;
}

bool ie_hook_svc::is_os_nt5()
{
	OSVERSIONINFO osvi;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	GetVersionEx(&osvi);

	if (osvi.dwMajorVersion < 6 ) // winxp
		return true;
	else
	{
		if(osvi.dwMinorVersion == 0) // vista
			return true; 
		else if(osvi.dwMinorVersion > 0) //win7
			return false;
	}

	return true;
}