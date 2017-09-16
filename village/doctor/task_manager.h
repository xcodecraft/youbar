#pragma once

#include "task/task.h"
#include "task/task_queue.h"
#include "task/thread_task.h"
#include "common/log.h"
#include <boost/thread.hpp>

namespace yyou
{

	class   task_manager
	{
	private:
		bool							            _work_sig;
		task_exec_unit::pmap			_autoexec_map; //自动定时任务
		task_exec_unit					    _realtime_exec;  //即时任务
		task_msg_queue					_rpt_queue;
	public:
		task_manager()
		{
		}
		~task_manager()
		{
			stop_work();
			wait_work_end();
		}
		task_msg_queue*   task_report_queue()
		{
			return &_rpt_queue;
		}
		void join_auto_task(int sec, task::sptr t,int start_wait_sec=0)
		{
			xlogger_holder* log =  LOG_INS(log_def::AUTO);
			join_auto_task_impl(_autoexec_map,log,sec,t,start_wait_sec);
		}
		void join_auto_task_impl(task_exec_unit::pmap& exec_map, xlogger_holder* logger,  int sec, task::sptr t,int start_wait_sec=0)
		{
			task_exec_unit::pmap::iterator found = exec_map.find(sec);
			if(found != exec_map.end())
			{
				push_op_t  op = BOOST_BIND(&task_exec_unit::join_task,found->second.get(),_1);
				task::sptr ringt(new ring_task (op, t) );
				found->second->join_task(ringt);
			}
			else
			{
				task::sptr start_wait_sec(new wait_task(start_wait_sec));
				task::sptr wait_sec(new wait_task(sec));
				task_exec_unit::sptr u(new task_exec_unit);	
				push_op_t  op = BOOST_BIND(&task_exec_unit::join_task,u.get(),_1);
				task::sptr ringt(new ring_task (op, t) );
				task::sptr ring_wait(new ring_task (op, wait_sec) );
				_autoexec_map[sec]	= u;
				u->join_task(start_wait_sec);
				u->join_task(ringt);
				u->join_task(ring_wait);
				u->work(&_rpt_queue,logger);
				//work();
			}
		}
		void join_realtime_task(task::sptr t)
		{
			_realtime_exec._queue->safe_push(t);
		}
		void work()
		{
			xlogger_holder* log2 =  LOG_INS(log_def::RT);
			_realtime_exec.work(&_rpt_queue,log2,4);
		}
		void stop_work()
		{
			BOOST_FOREACH(task_exec_unit::pair  item ,_autoexec_map)
			{
				item.second->stop_work();
			}
			_realtime_exec.stop_work();
		}
		void wait_work_end()
		{
			BOOST_FOREACH(task_exec_unit::pair  item ,_autoexec_map)
			{
				item.second->stop_work();
				item.second->wait_work_end();
			}
			_realtime_exec.stop_work();
			_realtime_exec.wait_work_end();
		}
	};
}