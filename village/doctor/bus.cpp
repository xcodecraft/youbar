#include "pandora.h"
#include "navi/logic/navi_bus.h"
#include "win_utls/filepropty.h"
#include <boost/filesystem.hpp>
#include "context/ipc_context.h"
#include "base/sys_env.h"
#include "base/shared_data.h"
#include <fstream>
#include "remote/remote_impl.h"

#include "json/json.h"
#include "baselib/encrypt_str.h"
#include "baselib/base64.h"
using namespace std;
using namespace boost::filesystem;
namespace bfs=boost::filesystem;

namespace yyou
{

	navi_env* navi_env::ins()
	{
		static   navi_env  s_ins ;
		return &s_ins;
	}

	void navi_env::init_start(const yyou::game_account& account, xlogger_holder* logger )
	{
		_logger = logger;
		_DBG_LOG(_logger) << "init logs over";
		init_main_conf();
		yyou::remote::cache_sgt_admin::init(remote_cache_path());
		init_user_profile();
		init_game_account(account);
		//init dir
		init_dir();
	}

	yyou::user_profile::sptr navi_env::get_user_profile()
	{
		return _user_profile;
	}

	yyou::pandora_conf::sptr navi_env::get_main_conf()
	{
		return _pandora_conf;
	}

	std::wstring navi_env::user_game_conf_dir()
	{
		std::wstringstream path;
		path  << _pandora_conf->user_conf_dir(_user_profile->id) << _game_account->game<<L"\\" ;
		std::wstring tmp_path =std_path(path.str()); 
		return tmp_path;
	}

	std::wstring navi_env::user_game_data_dir()
	{
		std::wstringstream path;
		path  << _pandora_conf->user_data_dir(_user_profile->id) <<_game_account->game<<L"\\" ;
		std::wstring tmp_path =std_path(path.str()); 
		return tmp_path;
	}

	std::wstring navi_env::account_data_dir_path()
	{
		std::wstring account_key = _game_account->isp+L"_"+_game_account->username ;
		std::wstringstream path;
		path  << user_game_data_dir() << account_key <<L"\\";
		std::wstring tmp_path =std_path(path.str()); 
		return tmp_path;
	}

	std::wstring navi_env::account_conf_dir_path()
	{
		std::wstring account_key = _game_account->isp+L"_"+_game_account->username;
		std::wstringstream path;
		path  << user_game_conf_dir()  << account_key <<L"\\";
		std::wstring tmp_path =std_path(path.str()); 
		return tmp_path;
	}

	std::wstring navi_env::account_conf_file_path( const wchar_t * file_name )
	{
		std::wstringstream path;
		path  << account_conf_dir_path()  <<file_name  ;
		ensure_dir_exist(path.str());
		return path.str().c_str();
	}

	std::wstring navi_env::account_data_file_path( const wchar_t * file_name )
	{
		std::wstringstream path;
		path  << account_data_dir_path()  <<file_name  ;
		ensure_dir_exist(path.str());
		return path.str().c_str();
	}
	std::wstring navi_env::user_game_conf_file( const wchar_t * file_name )
	{
		std::wstringstream path;
		path  << user_game_conf_dir() <<file_name  ;
		ensure_dir_exist(path.str());
		return path.str().c_str();
	}

	std::wstring navi_env::user_game_data_file(const wchar_t * file_name )
	{
		std::wstringstream path;
		path  << user_game_data_dir() <<file_name  ;
		ensure_dir_exist(path.str());
		return path.str().c_str();
	}
	std::wstring navi_env::user_conf_dir()
	{
		std::wstringstream path;
		path  << _pandora_conf->user_conf_dir(_user_profile->id)  ;
		std::wstring tmp_path =std_path(path.str()); 
		return tmp_path;
	}

	std::wstring navi_env::user_conf_file( const wchar_t * file_name )
	{
		std::wstringstream path;
		path  << user_conf_dir() <<file_name  ;
		ensure_dir_exist(path.str());
		return path.str().c_str();
	}


	navi_env::navi_env()
	{
		_pandora_conf.reset(new yyou::pandora_conf);
		_user_profile.reset(new yyou::user_profile);
	}

	std::wstring navi_env::ensure_dir_exist( const std::wstring& path_str )
	{
		wstring  ensure_path = path_str;
		if(!bfs::is_directory(path_str))
		{
			ensure_path = bfs::wpath(path_str).parent_path().string();
		}
		if(!bfs::exists(ensure_path))
		{
			bfs::create_directories(ensure_path);
		}
		return path_str;
	}

	void navi_env::init_game_account( yyou::game_account account )
	{
		_game_account.reset(new yyou::game_account());
		_game_account->isp = account.isp;
		_game_account->game = account.game;
		_game_account->service = account.service;
		_game_account->username = account.username ;
		_game_account->nickname  = account.nickname ;
		_game_account->password  = account.password;
		_game_account->defaulturl  = account.defaulturl;
		_game_account->args            = account.args;
	}

	std::wstring navi_env::http_cache_path()
	{
		/*
		std::wstringstream path;
		path  << _pandora_conf->cache_dir() << L"\\requests\\"<<_game_account->game<<L"\\" ;
		std::wstring tmp_path =std_path(path.str()); 
		return tmp_path;
		*/
		TCHAR filePath[MAX_PATH];
		::SHGetSpecialFolderPath(NULL, filePath, CSIDL_COMMON_APPDATA, FALSE);
		std::wstring   wstr(filePath);   
		std::wstring tmp_path = un_std_path(wstr+L"\\yunyou\\cache\\game_cache\\"+_game_account->game+L"\\");
		return tmp_path;
	}

	std::wstring navi_env::cookie_path()
	{
		std::wstringstream path;
		path  << _pandora_conf->cache_dir() << L"\\cookies\\"<<_game_account->username<<L"\\" ;
		std::wstring tmp_path =std_path(path.str()); 
		return tmp_path;
	}

	std::wstring navi_env::ie_cache_path()
	{
		/*
		std::wstringstream path;
		path  << _pandora_conf->cache_dir() << L"\\ie\\"<<_game_account->username<<L"\\" ;
		std::wstring tmp_path =std_path(path.str()); 
		*/

		TCHAR filePath[MAX_PATH];
		::SHGetSpecialFolderPath(NULL, filePath, CSIDL_COMMON_APPDATA, FALSE);
		std::wstring   wstr(filePath);   
		std::wstring tmp_path =std_path(wstr+L"\\yunyou\\_temp\\ie\\"+_game_account->username+L"\\");
		return tmp_path;
	}

	std::wstring navi_env::flash_cookie_path()
	{
		TCHAR filePath[MAX_PATH];
		::SHGetSpecialFolderPath(NULL, filePath, CSIDL_COMMON_APPDATA, FALSE);
		std::wstring   wstr(filePath);   
		std::wstring tmp_path = un_std_path(wstr+L"\\yunyou\\_temp\\flash_cookies\\"+_game_account->username+L"\\");
		return tmp_path;
	}

	std::wstring navi_env::knowledge_path()
	{
		return std_path(sys_env::ins()->get_res_data_path()+L"\\knowledge\\");
	}

	std::wstring navi_env::local_data_path()
	{
		return std_path(sys_env::ins()->get_res_data_path());
	}
	std::wstring navi_env::conf_dir()
	{
		std::wstringstream path;
		path  << _pandora_conf->conf_dir() ;
		std::wstring tmp_path =std_path(path.str()); 
		return tmp_path;
	}

	void navi_env::clear_cache()
	{
		if(boost::filesystem::exists(http_cache_path()))
			boost::filesystem::remove_all(http_cache_path())	;
		if(boost::filesystem::exists(ie_cache_path()))
			boost::filesystem::remove_all(ie_cache_path())	;
		if(boost::filesystem::exists(flash_cookie_path()))
			boost::filesystem::remove_all(flash_cookie_path())	;
	}

	void navi_env::clear_cookie()
	{
		if(boost::filesystem::exists(cookie_path()))
			boost::filesystem::remove_all(cookie_path())	;
	}

	void navi_env::init_user_profile()
	{
		_DBG_LOG(LOG_INS_MAIN) << " ==  init_user_profile  BEGIN ==";
		std::wstring user_profile_str = shared_data_svc::ins()->get(L"user");
		if(user_profile_str.size()>0)
			yyou::js_load_conf(*_user_profile,yyou::wstr2str(user_profile_str));
		_DBG_LOG(LOG_INS_MAIN) <<"user_profile id="<< _user_profile->id;
		_DBG_LOG(LOG_INS_MAIN) << " ==  init_user_profile  End ==";
	}

	void navi_env::init_main_conf()
	{
		_pandora_conf = sys_env::ins()->get_main_conf();
	}

	std::wstring navi_env::remote_cache_path()
	{
		std::wstringstream path;
		path  << _pandora_conf->cache_dir() << L"\\remote\\" ;
		std::wstring tmp_path =std_path(path.str()); 
		return tmp_path;
	}

	void navi_env::set_user_profile( user_profile* conf )
	{
		_user_profile->id                = conf->id;
		_user_profile->username  = conf->username;
		_DBG_LOG(LOG_INS_MAIN) <<"set_user_profile "<< _user_profile->id;
	}

	std::wstring navi_env::account_data_log_dir_path()
	{
		std::wstring account_key = _game_account->isp+L"_"+_game_account->username ;
		std::wstringstream path;
		path  << account_data_dir_path() << L"log\\";
		std::wstring tmp_path =std_path(path.str()); 
		return tmp_path;
	}

	std::wstring navi_env::account_data_db_dir_path()
	{
		std::wstring account_key = _game_account->isp+L"_"+_game_account->username ;
		std::wstringstream path;
		path  << account_data_dir_path() << L"db\\";
		std::wstring tmp_path =std_path(path.str()); 
		return tmp_path;
	}

	std::wstring navi_env::account_data_db_file_path( const wchar_t * file_name )
	{
		std::wstringstream path;
		path  << account_data_db_dir_path()  <<file_name  ;
		ensure_dir_exist(path.str());
		return path.str().c_str();
	}

	void navi_env::init_dir()
	{
		boost::filesystem::create_directories(user_game_conf_dir());
		boost::filesystem::create_directories(user_game_data_dir());
		boost::filesystem::create_directories(account_data_dir_path());
		boost::filesystem::create_directories(account_conf_dir_path());
		boost::filesystem::create_directories(user_conf_dir());
		boost::filesystem::create_directories(http_cache_path());
		boost::filesystem::create_directories(ie_cache_path());
		boost::filesystem::create_directories(flash_cookie_path());
		boost::filesystem::create_directories(cookie_path());
		boost::filesystem::create_directories(remote_cache_path());
		boost::filesystem::create_directories(account_data_log_dir_path());
		boost::filesystem::create_directories(account_data_db_dir_path());
	}

	void navi_env::get_qqapp_conf( std::wstring app,qqapp_conf* conf )
	{
		std::wstring conf_dir = sys_env::ins()->get_res_data_path()+L"\\app_confs\\";
		std::wstring dat_file = conf_dir+app+L".conf";
		std::wstring db_file = conf_dir+app+L".db";
		std::string content;
		if(boost::filesystem::exists(db_file))
		{
			content=read_file(db_file);
			content =cbase64::deBase64(enc::decrypt(content));
		}
		else
		{
			content=read_file(dat_file);
		}

		if(content.size()>0)
		{
			Json::Reader reader;
			Json::Value json_value;
			if(reader.parse(content, json_value))
			{
				conf->show_scroll    = json_value["show_scroll"].asBool();
				conf->can_rightkey  = json_value["can_rightkey"].asBool();
			}
		}
	}
}