#pragma once
#include "common.h"
#include "common/log.h"
#include "knowledge/conf_def.h"
#include <sstream>
typedef  boost::shared_ptr<std::istream> istream_sptr;

namespace yyou
{

	class navi_env
	{
	public:
		yyou::pandora_conf::sptr _pandora_conf;
		yyou::user_profile::sptr  _user_profile;
		yyou::game_account::sptr _game_account;
		xlogger_holder*				   _logger;
		static navi_env* ins();
		void init_start(const yyou::game_account& account,xlogger_holder* logger);
		yyou::user_profile::sptr  get_user_profile();
		yyou::pandora_conf::sptr get_main_conf();

		void get_qqapp_conf(std::wstring app,qqapp_conf* conf);

		std::wstring account_conf_dir_path();
		std::wstring account_data_dir_path();
		std::wstring user_game_conf_dir();
		std::wstring user_game_data_dir();
		std::wstring user_conf_dir();

		std::wstring account_data_log_dir_path();
		std::wstring account_data_db_dir_path();
		std::wstring account_data_db_file_path( const wchar_t * file_name );

		std::wstring account_conf_file_path(const wchar_t * file_name );
		std::wstring account_data_file_path(const wchar_t * file_name );
		std::wstring user_game_conf_file(const wchar_t * file_name );
		std::wstring user_game_data_file(const wchar_t * file_name );
		std::wstring user_conf_file(const wchar_t * file_name );

		std::wstring http_cache_path();
		std::wstring cookie_path();
		std::wstring ie_cache_path();
		std::wstring flash_cookie_path();
		std::wstring remote_cache_path();

		std::wstring local_data_path();
		std::wstring conf_dir();
		std::wstring knowledge_path();

		void clear_cache();
		void clear_cookie();
		void set_user_profile( yyou::user_profile* conf);

	private:
		navi_env();
		std::wstring ensure_dir_exist(const std::wstring& path_str);
		void init_game_account(yyou::game_account account);
		void init_user_profile();
		void init_main_conf();
		void init_dir();
	};
}