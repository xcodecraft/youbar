#pragma once
//#include "common/common_base.h"

#include <iostream>
#include <string>
#include <fstream>
#include <ctype.h>
#include <stdlib.h>

namespace yyou
{
	//typedef unsigned char BYTE;
	struct char_encode
	{
		enum encode_t{utf8=CP_UTF8 , win_sys=CP_ACP};
	};
	 std::wstring   str2wstr(const std::string& from, char_encode::encode_t char_code  = char_encode::utf8);
	 std::string    wstr2str(const std::wstring& from, char_encode::encode_t char_code  = char_encode::utf8);
	 std::string    time_t2str(time_t _time);	 
	 time_t  str2time_t(const std::string str_time);
	 std::string  urlencode(const std::string& src);
	 std::string    int2str(int idata);
	 int  str2int(std::string ss);
	 time_t  now();
	 std::string read_file(std::wstring& data_path);

	 void save_file(std::wstring & data_path,std::string cata);
	 void debug_log(const char * str);

	 std::wstring get_module_path();
	 std::wstring extract_filepath(std::wstring filename);
	 std::wstring extract_filename(std::wstring filename);


	 inline BYTE toHex(const BYTE &x);

	 std::string urlEncoding( std::string &sIn );
}
