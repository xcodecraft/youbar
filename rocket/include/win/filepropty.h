#pragma  once
#include "boost/filesystem.hpp"
namespace yyou
{

	static struct LANGANDCODEPAGE 
	{
		WORD wLanguage;
		WORD wCodePage;
	} *lpTranslate;
	class  file_propty
	{
	private:
		DWORD			_dwSize;
		UINT			_uiSize;
		PTSTR			_pBuffer;
		UINT			_uiOtherSize;
		PVOID			_pTmp;
		std::wstring	_pcszFileName;
		LANGANDCODEPAGE *_pLanguage;
		TCHAR SubBlock[MAX_PATH];
		TCHAR _retValue[MAX_PATH];
	public:
		file_propty():_pcszFileName(L""){}
		file_propty(std::wstring filename_):_pcszFileName(filename_){}
		void setFilename(std::wstring filename_){_pcszFileName = filename_;}
		bool InitGetVersion();
		std::wstring	GetVersionName();
		std::wstring	GetProductName();
		void static std_version(std::wstring &version_);
		bool static version_cmp( std::wstring& CurrentVersion,std::wstring& SvcVersion );
	};

	bool   copy_dir(const boost::filesystem::path & from_dir,const boost::filesystem::path & to_dir);
}
