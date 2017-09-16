// Registry.cpp : implementation file
//
#include "common.h"
#include "win_utls/registry.h"
namespace yyou
{



	/////////////////////////////////////////////////////////////////////////////
	// CRegistry

	registry::registry(HKEY hKey)
	{
		m_hKey=hKey;
	}

	registry::~registry()
	{
		Close();
	}

	/////////////////////////////////////////////////////////////////////////////
	// CRegistry Functions

	BOOL registry::CreateKey(std::wstring lpSubKey)
	{


		HKEY hKey;
		DWORD dw;
		long lReturn=RegCreateKeyEx(m_hKey,(LPCTSTR)lpSubKey.c_str(),0L,NULL,REG_OPTION_VOLATILE,KEY_ALL_ACCESS,NULL,&hKey,&dw);

		if(lReturn==ERROR_SUCCESS)
		{
			m_hKey=hKey;
			return TRUE;
		}

		return FALSE;

	}

	BOOL registry::Open(std::wstring lpSubKey)
	{


		HKEY hKey;
		long lReturn=RegOpenKeyEx(m_hKey,(LPCTSTR)lpSubKey.c_str(),0L,KEY_ALL_ACCESS,&hKey);

		if(lReturn==ERROR_SUCCESS)
		{
			m_hKey=hKey;
			return TRUE;
		}
		return FALSE;

	}

	void registry::Close()
	{
		if(m_hKey)
		{
			RegCloseKey(m_hKey);
			m_hKey=NULL;
		}

	}

	BOOL registry::DeleteValue(std::wstring lpValueName)
	{


		long lReturn=RegDeleteValue(m_hKey,lpValueName.c_str());

		if(lReturn==ERROR_SUCCESS)
			return TRUE;
		return FALSE;

	}

	BOOL registry::DeleteKey(HKEY hKey, std::wstring lpSubKey)
	{


		long lReturn=RegDeleteValue(hKey,lpSubKey.c_str());

		if(lReturn==ERROR_SUCCESS)
			return TRUE;
		return FALSE;

	}

	BOOL registry::Write(std::wstring lpSubKey, int nVal)
	{


		DWORD dwValue;
		dwValue=(DWORD)nVal;

		long lReturn=RegSetValueEx(m_hKey,(LPCTSTR)lpSubKey.c_str(),0L,REG_DWORD,(const BYTE *) &dwValue,sizeof(DWORD));

		if(lReturn==ERROR_SUCCESS)
			return TRUE;

		return FALSE;

	}

	BOOL registry::Write(std::wstring lpSubKey, DWORD dwVal)
	{


		long lReturn=RegSetValueEx(m_hKey,(LPCTSTR)lpSubKey.c_str(),0L,REG_DWORD,(const BYTE *) &dwVal,sizeof(DWORD));

		if(lReturn==ERROR_SUCCESS)
			return TRUE;

		return FALSE;

	}

	BOOL registry::Write(std::wstring lpValueName, std::wstring lpValue)
	{


		long lReturn=RegSetValueEx(m_hKey,(LPCTSTR)lpValueName.c_str(),0L,REG_SZ,(const BYTE *) lpValue.c_str(),lpValue.length()*2+1);

		if(lReturn==ERROR_SUCCESS)
			return TRUE;

		return FALSE;

	}


	BOOL registry::Read(std::wstring lpValueName, int* pnVal)
	{

		DWORD dwType;
		DWORD dwSize=sizeof(DWORD);
		DWORD dwDest;
		long lReturn=RegQueryValueEx(m_hKey,lpValueName.c_str(),NULL,&dwType,(BYTE *)&dwDest,&dwSize);

		if(lReturn==ERROR_SUCCESS)
		{
			*pnVal=(int)dwDest;
			return TRUE;
		}
		return FALSE;

	}

	BOOL registry::Read(std::wstring lpValueName, DWORD* pdwVal)
	{

		DWORD dwType;
		DWORD dwSize=sizeof(DWORD);
		DWORD dwDest;
		long lReturn=RegQueryValueEx(m_hKey,lpValueName.c_str(),NULL,&dwType,(BYTE *)&dwDest,&dwSize);

		if(lReturn==ERROR_SUCCESS)
		{
			*pdwVal=dwDest;
			return TRUE;
		}
		return FALSE;

	}




	BOOL registry::RestoreKey(std::wstring lpFileName)
	{

		long lReturn=RegRestoreKey(m_hKey,lpFileName.c_str(),REG_WHOLE_HIVE_VOLATILE);

		if(lReturn==ERROR_SUCCESS)
			return TRUE;

		return FALSE;
	}

	BOOL registry::SaveKey(std::wstring lpFileName)
	{


		long lReturn=RegSaveKey(m_hKey,lpFileName.c_str(),NULL);

		if(lReturn==ERROR_SUCCESS)
			return TRUE;

		return FALSE;
	}


	BOOL registry::Read(std::wstring lpValueName, std::wstring &lpVal)
	{


		DWORD dwType;
		DWORD dwSize=200;
		TCHAR szString[2550];

		long lReturn=RegQueryValueEx(m_hKey,lpValueName.c_str(),NULL,&dwType,(BYTE *)szString,&dwSize);

		if(lReturn==ERROR_SUCCESS)
		{
			lpVal=szString;
			return TRUE;
		}
		return FALSE;

	}
	BOOL registry::EnumKey(std::vector<std::wstring> &keys_,DWORD &key_count){
		DWORD   sizeSubKey;						//子键大小
		TCHAR		subKey[MAX_KEY_LENGTH];			//子键名

		RegQueryInfoKey(m_hKey,NULL,NULL,NULL,&key_count,NULL,NULL,
			NULL,NULL,NULL,NULL,NULL);
		if(key_count >0){
			for(DWORD i = 0 ;i<key_count;i++){
				subKey[0]= 0;
				sizeSubKey = MAX_KEY_LENGTH;
				RegEnumKeyEx(m_hKey,i,subKey,&sizeSubKey,NULL,NULL,NULL,NULL);
				std::wstring wskey = subKey;keys_.push_back(wskey);
			}
			return true;
		}else	return false;
	}

	BOOL registry::EnumValue(std::vector<std::wstring> &values_,DWORD &value_count)
	{
		DWORD   sizeValue;						//子键大小
		TCHAR	value_[MAX_KEY_LENGTH];			//子键名
		RegQueryInfoKey(m_hKey,NULL,NULL,NULL,NULL,NULL,NULL,&value_count,
			NULL,NULL,NULL,NULL);
		if(value_count >0){
			std::wstring wsvalue;
			for(DWORD i = 0 ;i<value_count;i++){
				value_[0]= 0;
				sizeValue = MAX_KEY_LENGTH;
				RegEnumValue(m_hKey,i,value_,&sizeValue,NULL,NULL,NULL,NULL);
				wsvalue = value_; values_.push_back(wsvalue);
			}
			return true;
		}else	return false;
	}

}
