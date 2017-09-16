#include "pandora.h"
#include "knowledge/json_base.h"
#include "knowledge/conf_json.h"
#include "navi/logic/architect.h"
#include "base/smith.h"
#include "base/neo.h"
#include "base/log_init.h"
#include "context/ipc_context.h"
#include "navi/logic/navi_bus.h"
#include "navi/logic/game_env_svc.h"

using namespace std;
using namespace boost;
using namespace boost::filesystem;         
using namespace boost::logging;
namespace yyou
{

	class architect_impl
	{
	public :
		architect_impl(std::wstring account_string);
		~architect_impl();
		void start_work();
		void stop_work();

		void accept_realtime_task(task::sptr t,  task_def::task_run_t rt);
		void accept_auto_task( int sec, task::sptr t, int first_wait);
		void ensure_runenv();
		void set_exe_run_env();
        void bind_browser(ybrowser& ie)
        {
		    _game_env_svc->init(ie);
        }

		browser_ctrl::sptr get_browser_ctrl();
	protected:
		smith									_smith;
		xlogger_holder*				_logger;
		std::wstring   _start_info;
		yyou::game_account  _game_account;
		game_env_svc::sptr       _game_env_svc;
		void init_log(std::wstring path=L"");
	};



	////////////////////////////////////////////////

	//=================== architect_impl ======================
	architect_impl::architect_impl (std::wstring account_string)
		:_logger(LOG_INS_MAIN)
	{
		std::string account_str = wstr2str(account_string);
		try
		{
			_DBG_LOG(_logger) <<  "====== begin js_load_conf ======";
			yyou::js_load_conf(_game_account,account_str);
		}
		catch (yyou::conf_err &e)
		{
			_ERR_LOG(_logger) <<  "game_account js_load_conf err! "<< e.what() ;
			std::wstring err = L"账户信息解析错误";
			throw yyou::logic_err (err.c_str());
		}
		_DBG_LOG(_logger) <<  "====== start navi_env ======";
		navi_env::ins()->init_start(_game_account,_logger);
		//init_log(navi_env::ins()->get_main_conf()->log_dir());

		_DBG_LOG(_logger) <<  "====== start game_env ======";
		_game_env_svc.reset(new game_env_svc(_game_account.game));
		_DBG_LOG(_logger) <<  "====== start smith_task ======";
		NEO_CNN_M2(neo::ins()->realtime_task,&architect_impl::accept_realtime_task);
	}
	architect_impl::~architect_impl()
	{
		_smith.wait_work_end();
	}
	void architect_impl::start_work()
	{
		_smith.work();
	}
	void architect_impl::stop_work()
	{
		_smith.stop_work();
		_smith.wait_work_end();
	}
	void architect_impl::accept_realtime_task( task::sptr t ,task_def::task_run_t rt)
	{
		if(rt == task_def::REAL_TIME)
			_smith.join_realtime_task(t);
		else
			_ERR_LOG(_logger) <<  "auto task, but not accept" ;
	}
	browser_ctrl::sptr architect_impl::get_browser_ctrl()
	{
		return _game_env_svc->get_browser();
	}
	void architect_impl::accept_auto_task( int sec, task::sptr t ,int first_wait )
	{
		_smith.join_auto_task(sec,t,first_wait);
	}
	void architect_impl::init_log( std::wstring path/*=L""*/ )
	{
		BOOST_ASSERT(path.length());
		architect::log_init(path,L"navi");
		_logger = LOG_INS(log_def::MAIN);
	}



	//=================== architect ======================
	architect::architect(std::wstring account_string)
	{
        _ynet_ready = false;
		_impl = new architect_impl(account_string);	
	}
	architect::~architect()
	{
		delete _impl;
	}
	void architect::start_work()
	{
		_impl->start_work();
	}
    void architect::bind_browser(ybrowser& ie)
    {
        _impl->bind_browser(ie);
    }
	void architect::accept_realtime_task( task::sptr t, task_def::task_run_t rt )
	{
		_impl->accept_realtime_task(t,rt);
	}
	void architect::accept_auto_task( int sec, task::sptr t, int first_wait )
	{
		_impl->accept_auto_task(sec,t,first_wait);
	}

	void architect::stop_work()
	{
		_impl->stop_work();
	}
	browser_ctrl::sptr architect::get_browser_ctrl()
	{
		return _impl->get_browser_ctrl();
	}
	void  architect::log_init(const  std::wstring& log_path ,const std::wstring& prefix)
	{/*{{{*/
		log_env_init(log_path,prefix);
	}/*}}}*/
}
