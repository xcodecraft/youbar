#pragma  once
#include <atlstr.h> 
#include <winreg.h>

#define  MAX_KEY_LENGTH	 1024
namespace yyou
{

	/////////////////////////////////////////////////////////////////////////////

	class  registry 
	{
	public:
		registry(HKEY hKey=HKEY_LOCAL_MACHINE);

	public:
		BOOL SaveKey(std::wstring lpFileName);
		BOOL RestoreKey(std::wstring lpFileName);
		BOOL Read(std::wstring lpValueName, std::wstring &lpVal);
		BOOL Read(std::wstring lpValueName, DWORD* pdwVal);
		BOOL Read(std::wstring lpValueName, int* pnVal);
		BOOL Write(std::wstring lpSubKey, std::wstring lpVal);
		BOOL Write(std::wstring lpSubKey, DWORD dwVal);
		BOOL Write(std::wstring lpSubKey, int nVal);
		BOOL DeleteKey(HKEY hKey, std::wstring lpSubKey);
		BOOL DeleteValue(std::wstring lpValueName);
		BOOL EnumKey(std::vector<std::wstring> &keys_,DWORD &key_count);
		BOOL EnumValue(std::vector<std::wstring> &values_,DWORD &value_count);
		void Close();
		BOOL Open(std::wstring lpSubKey);
		BOOL CreateKey(std::wstring lpSubKey);
		virtual ~registry();

	protected:
		HKEY m_hKey;

	};

}
