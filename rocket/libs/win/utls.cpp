#include "common.h"
#include "win_utls/utls.h"
#include <boost/filesystem.hpp>
#include "shlobj.h"
#include <shellapi.h>

using namespace yyou;

scope_sigcnn_sptr  yyou::make_sig_cnn(boost::signals2::connection& cnn)
{
    return scope_sigcnn_sptr(new scoped_cnn(cnn));

}
std::wstring std_path(std::wstring& path)
{
    boost::replace_all(path,L"\\\\",L"/");
    boost::replace_all(path,L"\\",L"/");
    boost::replace_all(path,L"//",L"/");
    boost::to_lower(path);
    return path;
}
std::string std_path(std::string& path)
{
    boost::replace_all(path,"\\\\","/");
    boost::replace_all(path,"\\","/");
    boost::replace_all(path,L"//",L"/");
    std::wstring wpath = yyou::str2wstr(path);
    boost::to_lower(wpath);
    return yyou::wstr2str(wpath,char_encode::win_sys);
}
void optimize_mem(int restore_size)
{
    int min_size ;
    int max_size ;
    if(restore_size<0)
    {
        min_size = -1;
        max_size = -1;
    }
    else
    {
        min_size = 1 * 1024 * 1024;
        max_size = restore_size * 1024 * 1024;
    }
    freemem(min_size,max_size);
}

void freemem(int mix_size,int max_size)
{
    CoFreeUnusedLibraries();
    SetProcessWorkingSetSize(GetCurrentProcess(),  mix_size, max_size);
    //	SetProcessWorkingSetSize(GetCurrentProcess(), -1, -1);
    //	SetProcessWorkingSetSizeEx(GetCurrentProcess(), 20 *  1024 * 1024, 50 * 1024 * 1024 , QUOTA_LIMITS_HARDWS_MIN_ENABLE | QUOTA_LIMITS_HARDWS_MAX_ENABLE);
}

yyou::scope_status_transform::scope_status_transform( bool& status ) :_status(status)
{
    _status = !	_status;
}

yyou::scope_status_transform::~scope_status_transform()
{
    _status = !	_status;
}


cache_stg* yyou::cache_stg::empty_stg()
{
    static cache_stg stg;
    /*
       WCHAR szPath[50];
    // 获得系统目录
    ::GetWindowsDirectoryW( szPath, (sizeof(szPath) * 50));
    // 格式化文件路径
    //wcscat( szPath, L"\\Temp\\" );
    */

    TCHAR filePath[MAX_PATH];
    ::SHGetSpecialFolderPath(NULL, filePath,CSIDL_COMMON_APPDATA , FALSE);//CSIDL_APPDATA
    std::wstring   wstr(filePath);   
    std::wstring cache_path = wstr+L"\\caiyun\\cache\\";
    boost::filesystem::create_directories(cache_path);
    stg._cache_path=cache_path;//std::wstring(szPath) + L"\\Temp\\";
    stg._cache_sec =0 ;
    return  &stg;
}

std::wstring AddVersionBuild( std::wstring sVersion )
{
    if (sVersion.empty())
        return FALSE;

    std::wstring version = L"";

    int fullstop_count = 0;
    std::wstring sbuild = L"";
    std::wstring sVersion_nub = L"";
    for (unsigned int i = 0; i < sVersion.size(); i++)
    {
        if (fullstop_count >= 3)
            sbuild += sVersion[i];

        if (sVersion[i] == '.')
            fullstop_count++;

        if (fullstop_count < 3)
            sVersion_nub += sVersion[i];
    }

    version = sVersion_nub + L"(build:" + sbuild + L")";
    return version;
}
void split_str( std::string& ori_string,std::string split_str,std::vector<std::string> &out )
{
    int length=split_str.size();
    int pos= 0;
    int from=0;
    do{   
        pos=ori_string.find(split_str.c_str(),from);   
        out.push_back(ori_string.substr(from,pos-from));
        from=pos+length;
    }while(pos!=std::string::npos);
}
bool delete_file(std::wstring & path )
{
    wchar_t s_path[MAX_PATH] = L"";
    memcpy(s_path,path.c_str(),(path.size()+1)*sizeof(wchar_t));
    SHFILEOPSTRUCT FileOp;
    //FileOp.fFlags = FOF_NOCONFIRMATION;
    //FileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI | FOF_SILENT;
    FileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;
    FileOp.hNameMappings = NULL;
    FileOp.hwnd = NULL;
    FileOp.lpszProgressTitle = NULL;
    FileOp.pFrom =  s_path;
    FileOp.pTo = NULL;
    FileOp.wFunc = FO_DELETE;
    return  SHFileOperation(&FileOp) == 0;
}

std::wstring un_std_path(std::wstring & path)
{
    boost::replace_all(path,L"\\\\",L"\\");
    boost::replace_all(path,L"/",L"\\");
    boost::to_lower(path);
    return path;
}
std::string un_std_path(std::string & path)
{
    boost::replace_all(path,L"/",L"\\");
    boost::replace_all(path,L"\\\\",L"\\");
    std::wstring wpath = yyou::str2wstr(path);
    boost::to_lower(wpath);
    return yyou::wstr2str(wpath,char_encode::win_sys);
}


std::wstring get_exe_position()
{
    TCHAR szBuf[MAX_PATH];
    ZeroMemory(szBuf,MAX_PATH);
    GetModuleFileName(NULL,szBuf,MAX_PATH);
    if   (_tcsrchr(szBuf,   _T('\\'))   !=   NULL)   
        *_tcsrchr(szBuf,   _T('\\'))   =   _T('\0');   
    std::wstring module_path(szBuf);
    return  std_path(module_path);
}

