#pragma once
#include "common.h"
#include "task/task.h"
#include "task/task_queue.h"
#include "common/log.h"
#include "knowledge/conf_def.h"
#include "navi/logic/browser_ctrl_base.h"

namespace yyou
{

	class architect_impl;
	class architect
	{
		architect_impl*  _impl;
        bool              _ynet_ready;
	public:
		architect(std::wstring account_string);
		~architect();
		static void log_init(const  std::wstring& log_path ,const std::wstring& prefix);
		void start_work();
        void bind_browser(ybrowser& ie);
		void stop_work();

		void accept_realtime_task(yyou::task::sptr t,  yyou::task_def::task_run_t rt);
		void accept_task(yyou::task::sptr t,  yyou::task_def::task_run_t rt);
		void accept_auto_task( int sec, yyou::task::sptr t, int first_wait);
		browser_ctrl::sptr get_browser_ctrl();

        void set_ynet_ready()
        {
            _ynet_ready = true;
        }
        bool ynet_ready()
        {
            return _ynet_ready ;
        }

	};
}
