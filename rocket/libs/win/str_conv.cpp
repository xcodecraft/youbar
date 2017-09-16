#include "common.h"
#include "win_utls/str_conv.h"
#include <atlbase.h>
#include <atlwin.h>
#include <fstream>
//#define  OUT_CODE	CP_ACP
#define  OUT_CODE	CP_UTF8
using namespace std;

namespace yyou
{
	std::string int2str(int idata)
	{
		char ss[MAX_PATH];
		memset(ss,0,MAX_PATH);
		sprintf(ss,"%d",idata);
		return std::string(ss);

	}

	int str2int(std::string ss)
	{
		int idata = atoi(ss.c_str());
		return idata;
	}

	std::wstring  str2wstr(const std::string& from, char_encode::encode_t char_code   )
	{

		int buflen=MultiByteToWideChar( char_code, 0, from.c_str(), -1, NULL, 0 ) +1 ;
		boost::scoped_array<wchar_t> buf( new wchar_t[buflen]);
		memset(buf.get(),0,  buflen*2 );
		MultiByteToWideChar( char_code, 0, from.c_str(), -1, buf.get(), buflen );
		return std::wstring(buf.get());
	}

	std::string  wstr2str(const std::wstring& from,char_encode::encode_t char_code  )
	{
		//得到转化后需要Buf的长度
		int buflen = WideCharToMultiByte( char_code, 0, from.c_str(), -1, NULL, 0, NULL, NULL ) + 1;
		boost::scoped_array<char> buf( new char[buflen]);
		memset(buf.get(),0,  buflen );
		WideCharToMultiByte( char_code, 0, from.c_str(), -1, buf.get(), buflen, NULL, NULL );
		return std::string(buf.get());
	}

	std::string  time_t2str(time_t _time)
	{
		char _st[100];
		memset(_st,0,100);
		int year=0,mon=0,day=0,hour=0,min=0,sec=0;
		tm *m_tm; 
		m_tm = localtime(&(time_t)_time);
		year = m_tm->tm_year+1900;
		mon = m_tm->tm_mon+1;
		day = m_tm->tm_mday;
		hour = m_tm->tm_hour;
		min = m_tm->tm_min;
		sec = m_tm->tm_sec; 
		sprintf(_st,"%04d-%02d-%02d %d:%d:%d",year,mon,day,hour,min,sec);
		return std::string(_st);
	}
	std::string read_file(std::wstring& data_path)
	{
		std::ifstream fin(data_path.c_str());
		std::stringstream sbuf;
		while(!fin.eof() && fin.good())
		{
			char buf[1025];
			memset(buf,0,1025);
			fin.read(buf,1024);
			sbuf << buf;
		}
		fin.close();
		return sbuf.str();
	}
	void save_file(std::wstring & data_path,std::string cata)
	{
		std::ofstream out(data_path.c_str());
		out << cata;
		out.close();
	}
	time_t  str2time_t(const std::string str_time)
	{
		time_t    tResult        = 0;
		struct tm tmResult    = {0};
		int iYear    = 0;
		int iMonth    = 0;
		int iDay    = 0;
		int iHour    = 0;
		int iMinut    = 0;
		int iSecon    = 0;
		sscanf(str_time.c_str(),"%d-%d-%d %d:%d:%d",&iYear,&iMonth,&iDay,&iHour,&iMinut,&iSecon);
		tmResult.tm_hour    = iHour;
		tmResult.tm_isdst    = 0;
		tmResult.tm_min        = iMinut;
		tmResult.tm_mon        = iMonth - 1;
		tmResult.tm_sec        = iSecon;
		tmResult.tm_mday    = iDay;
		tmResult.tm_year    = iYear - 1900;
		tResult = mktime(&tmResult);
		return tResult;
	}
	std::string  urlencode(const std::string& src)  
	{  
		static    char hex[] = "0123456789ABCDEF";  
		std::string dst;  
		for (size_t i = 0; i < src.size(); i++)  
		{  
			unsigned char ch = src[i];  
			if (isalnum(ch))  
			{  
				dst += ch;  
			}  
			else  
				if (src[i] == ' ')  
				{  
					dst += '+';  
				}  
				else  
				{  
					unsigned char c = static_cast<unsigned char>(src[i]);  
					dst += '%';  
					dst += hex[c / 16];  
					dst += hex[c % 16];  
				}  
		}  
		return dst;  
	}  

	time_t  now()
	{
		time_t now;
		time(&now);
		return now;
	}

	void debug_log(const char * str)
	{
		COPYDATASTRUCT _data;
		HWND hMain;
		hMain = ::FindWindow(NULL,L"51Logon_DebugLog");
		if (!hMain) return;
		_data.dwData=0;
		_data.lpData=(PVOID)str;
		_data.cbData=strlen(str);
		::SendMessage(hMain,WM_COPYDATA,0,(DWORD)&_data);
	}

	std::wstring extract_filepath( std::wstring filename )
	{
		std::wstring res = L"";
		int pos = filename.find_last_of(L"\\");
		if(pos > 0)
			res = filename.substr(0,pos+1);
		return res;
	}

	std::wstring extract_filename( std::wstring filename )
	{
		std::wstring res = L"";
		int pos = filename.find_last_of(L"\\");
		if(pos > 0)
			res = filename.substr(pos+1,filename.size()-pos-1);
		return res;
	}

	std::wstring get_module_path()
	{
		wchar_t mod[MAX_PATH] = L"";
		::GetModuleFileName(0,mod,MAX_PATH);
		return std::wstring(mod);
	}

	std::string urlEncoding( string &sIn )
	{
		cout << "size: " << sIn.size() << endl;
		string sOut;
		for( int ix = 0; ix < sIn.size(); ix++ )
		{
			BYTE buf[4];
			memset( buf, 0, 4 );
			if( isalnum( (BYTE)sIn[ix] ) )
			{
				buf[0] = sIn[ix];
			}
			else if ( isspace( (BYTE)sIn[ix] ) )
			{
				buf[0] = '+';
			}
			else
			{
				buf[0] = '%';
				buf[1] = toHex( (BYTE)sIn[ix] >> 4 );
				buf[2] = toHex( (BYTE)sIn[ix] % 16);
			}
			sOut += (char *)buf;
		}
		return sOut;
	}
	inline BYTE toHex(const BYTE &x)
	{
		return x > 9 ? x + 55: x + 48;
	}
}
