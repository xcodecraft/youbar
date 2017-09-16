#pragma once
#include "port.h"
DISABLE_WARNING(4251)
#include "pimpl.h"
#include "error/log.h"
#include "task/task.h"
#include "utls.h"

namespace pylon
{

	class  PYLON_API task_exec_unit : public smarty_ptr< task_exec_unit> 
	{
	public:

		task_exec_unit();
		void work( logger* log , int worker_cnt=1 );
		void stop_work();
		void wait_idel();
		void wait_work_end();
		void join_task(task::sptr t);
		void join_ring_task( int wait_sec,task::sptr t, int first_sec=0);
		void reset();
		PIMPL_DEF ;
	};


	class  PYLON_API timed_task_exec_unit : public smarty_ptr< timed_task_exec_unit> 
	{
	public:

		timed_task_exec_unit();
		void work( logger* log , int worker_cnt=1 );
		void stop_work();
		void wait_idel();
		void wait_work_end();
		void join_task( time_t exe_time, task::sptr t );
		void reset();
		PIMPL_DEF;
	};
}
