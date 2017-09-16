#include "common.h"
#include "win_utls/filepropty.h"	 
using namespace boost::filesystem;
namespace yyou
{

	bool file_propty::InitGetVersion(){
		_dwSize = 0;
		_uiSize = GetFileVersionInfoSize(_pcszFileName.c_str(),&_dwSize);

		if (0 == _uiSize) return false;
		_pBuffer = new TCHAR[_uiSize];
		if (NULL == _pBuffer) return false;
		memset(_pBuffer,0,_uiSize);
		if(!GetFileVersionInfo(_pcszFileName.c_str(),0,_uiSize,(PVOID)_pBuffer)) {			//获取exe 或 DLL 的资源信息，存放在pBuffer内
			delete []_pBuffer; _pBuffer = NULL;
			return false;	
		}

		_pLanguage = NULL;  //这里这样设置没关系了。
		_uiOtherSize = 0;

		if (!VerQueryValue(_pBuffer,L"\\VarFileInfo\\Translation",(PVOID*)&_pLanguage,&_uiOtherSize)) {
			delete []_pBuffer;		_pBuffer = NULL;
			return false;	//获取资源相关的 codepage 和language 
		}
		_pTmp = NULL;   //一定要把pTmp这个变量设置成PVOID或LPVOID型的
		memset((void*)SubBlock,0,sizeof(SubBlock));
		memset((void*)_retValue,0,sizeof(_retValue));
		return true;
	}

	std::wstring file_propty::GetVersionName(){
		if (!InitGetVersion())	return L"none";
		std::wstring wsout;
		wsprintf(SubBlock, L"\\StringFileInfo\\%04x%04x\\FileVersion",_pLanguage[0].wLanguage,_pLanguage[0].wCodePage);
		//   Comments InternalName ProductName 
		//   CompanyName LegalCopyright ProductVersion 
		//   FileDescription LegalTrademarks PrivateBuild 
		//   FileVersion OriginalFilename SpecialBuild  
		// OriginalFilename 可由上面的各种代替。

		VerQueryValue(_pBuffer,SubBlock, (PVOID*)&_pTmp, &_uiOtherSize); 
		wcscpy_s(_retValue,(TCHAR*)_pTmp);
		wsout= _retValue;
		delete []_pBuffer;	_pBuffer = NULL;
		return wsout;
	}

	std::wstring file_propty::GetProductName(){
		if(!InitGetVersion())	return L"cant not find the Product Name";
		std::wstring wsout = L"";
		wsprintf(SubBlock, L"\\StringFileInfo\\%04x%04x\\ProductName",	_pLanguage[0].wLanguage,_pLanguage[0].wCodePage);

		VerQueryValue(_pBuffer,SubBlock, (PVOID*)&_pTmp, &_uiOtherSize); 
		wcscpy_s(_retValue,(TCHAR*)_pTmp);
		wsout= _retValue;
		delete []_pBuffer;	_pBuffer = NULL;
		return wsout;
	}

	void file_propty::std_version(std::wstring &version_)
	{
		boost::algorithm::replace_all(version_,L",",L".");
		boost::algorithm::replace_all(version_,L" ",L"");
		boost::algorithm::trim(version_);
		
	}

	bool file_propty::version_cmp( std::wstring& CurrentVersion,std::wstring& SvcVersion )
	{
		if (CurrentVersion.empty() || SvcVersion.empty())
			return false;

		int iCurrentTemp1,iCurrentTemp2,iCurrentTemp3,iCurrentTemp4;
		int iSvcTemp1,iSvcTemp2,iSvcTemp3,iSvcTemp4;
		sscanf(yyou::wstr2str(CurrentVersion).c_str(),"%d.%d.%d.%d",&iCurrentTemp1,&iCurrentTemp2,&iCurrentTemp3,&iCurrentTemp4);
		sscanf(yyou::wstr2str(SvcVersion).c_str(),"%d.%d.%d.%d",&iSvcTemp1,&iSvcTemp2,&iSvcTemp3,&iSvcTemp4);

		if (iCurrentTemp1 < iSvcTemp1)
		{
			return true;
		}
		else if(iCurrentTemp1 > iSvcTemp1)
			return false;
		if (iCurrentTemp2 < iSvcTemp2)
		{
			return true;
		}
		else if(iCurrentTemp2 > iSvcTemp2)
			return false;
		if (iCurrentTemp3 < iSvcTemp3)
		{
			return true;
		}
		else if (iCurrentTemp3 > iSvcTemp3)
			return false;
		if (iCurrentTemp4 < iSvcTemp4)
		{
			return true;
		}
		else if (iCurrentTemp4 > iSvcTemp4)
			return false;

		return false;
	}


	bool  copy_dir(const boost::filesystem::path & from_dir,const boost::filesystem::path & to_dir)
	{
		/*
		if ( !exists( from_dir ) ) return false;
		if(!exists(to_dir))
		{
			create_directories(to_dir);
		}
		directory_iterator end_itr;
		for ( directory_iterator itr( from_dir ); itr != end_itr; ++itr )
		{
			if(!is_directory(itr->status()))
			{
				path file_name = itr->filename();
				copy_file(itr->path(),to_dir.string()+"//"+file_name.string());
			}
			else
			{
				path file_name = itr->filename();
				std::string to_path = to_dir.string()+"//"+file_name.string();
				copy_dir(itr->path(),to_path);
			}
		}
		*/
		return true;

	}
}
