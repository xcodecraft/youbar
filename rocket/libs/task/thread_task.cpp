#include "task/thread_task.h"
#include <boost/foreach.hpp>
#include <boost/thread.hpp>
#include "task_queue.h"
using namespace boost;
namespace pylon
{

	typedef boost::shared_ptr< boost::thread>   thread_sptr;
	typedef std::vector<boost::shared_ptr< boost::thread> >  thread_sptr_vct;
	struct task_exec_unit::impl {
		thread_sptr_vct					_thread_vct;
		queue_worker::parr				_workers;
		task_queue::sptr  				_queue;
		logger*				_log ;
		int					_worker_cnt;
		bool				_working;
		impl()
		{
			_working = false;
			_queue.reset(new task_queue);
		}

		void work( logger* log, int worker_cnt  )
		{
			_working = true;
			_log   = log;
			_worker_cnt = worker_cnt;
			if(!_thread_vct.empty())
				return ;
			for(int i =0 ; i < worker_cnt ; ++i)
			{
				queue_worker::sptr worker(new queue_worker(_queue.get(),log));
				_workers.push_back(worker);
				thread_sptr  worker_thread( new boost::thread(boost::ref(*worker)));
				_thread_vct.push_back(worker_thread);
			}
		}

		void stop_work()
		{

			BOOST_FOREACH(queue_worker::sptr w , _workers)
			{
				w->stop();
			}
		}

		void wait_work_end()
		{
			//stop_work();
			BOOST_FOREACH(thread_sptr t , _thread_vct)
			{
				t->join();
			}
		}

		void join_task( task::sptr t )
		{
			_queue->safe_push(t);
		}
		void join_ring_task( int wait_sec,task::sptr t, int start_wait_sec)
		{
			ring_task::push_op_t  op = BOOST_BIND(&task_exec_unit::impl::join_task,this,_1);
			task::sptr ringt(new ring_task (op, t) );
			if(start_wait_sec !=0 )
			{
				task::sptr task_obj(new wait_task(start_wait_sec));
				join_task(task_obj);
			}
			join_task(ringt);
			if(wait_sec!=0)
			{
				task::sptr wait_obj(new wait_task(wait_sec));
				task::sptr task_obj(new ring_task (op, wait_obj) );
				join_task(task_obj);
			}
		}

		void wait_idel()
		{

			BOOST_FOREACH(queue_worker::sptr w , _workers)
			{
				w->wait_idel();
			}
		}

		void reset()
		{
			if (!_working) return;
			stop_work();
			wait_work_end();
			_thread_vct.clear();
			_workers.clear();
			_queue.reset( new task_queue);

			for(int i =0 ; i < _worker_cnt ; ++i)
			{
				queue_worker::sptr worker(new queue_worker(_queue.get(),_log));
				_workers.push_back(worker);
				thread_sptr  worker_thread( new boost::thread(boost::ref(*worker)));
				_thread_vct.push_back(worker_thread);
			}

		}
		~impl()
		{
			_working = false;
			_queue.reset(new task_queue);
		}
	};

	task_exec_unit::task_exec_unit():_pimpl(new task_exec_unit::impl) { }
	void task_exec_unit::work( logger* log, int worker_cnt  )
	{
		_pimpl->work(log,worker_cnt);
	}
	void task_exec_unit::stop_work()
	{
		_pimpl->stop_work();
	}
	void task_exec_unit::wait_work_end()
	{
		_pimpl->wait_work_end();
	}
	void task_exec_unit::join_task( task::sptr t )
	{
		_pimpl->join_task(t);
	}
	void task_exec_unit::join_ring_task( int wait_sec,task::sptr t, int start_wait_sec)
	{
		_pimpl->join_ring_task(wait_sec,t,start_wait_sec);
	}
	void task_exec_unit::wait_idel()
	{
		_pimpl->wait_idel();
	}
	void task_exec_unit::reset()
	{
		_pimpl->reset();
	}



	struct timed_task_exec_unit::impl
	{

		thread_sptr_vct					_thread_vct;
		queue_worker::parr				_workers;
		priority_task_queue::sptr  		_queue;
		logger*				_log ;
		int					_worker_cnt;
		bool				_working;
		impl()
		{
			_working = false;
			_queue.reset(new priority_task_queue);
		}
		void work( logger* log, int worker_cnt  )
		{
			_working = true;
			_log   = log;
			_worker_cnt = worker_cnt;
			if(!_thread_vct.empty())
				return ;
			for(int i =0 ; i < worker_cnt ; ++i)
			{
				queue_worker::sptr worker(new queue_worker(_queue.get(),log));
				_workers.push_back(worker);
				thread_sptr  worker_thread( new boost::thread(boost::ref(*worker)));
				_thread_vct.push_back(worker_thread);
			}
		}

		void stop_work()
		{

			BOOST_FOREACH(queue_worker::sptr w , _workers)
			{
				w->stop();
			}
		}

		void wait_work_end()
		{
			//stop_work();
			BOOST_FOREACH(thread_sptr t , _thread_vct)
			{
				t->join();
			}
		}

		void join_task( time_t  exe_time, task::sptr t )
		{

			timed_wait_task::push_timed_task_t op = BOOST_BIND(&impl::join_task,this,_1,_2);
			task::sptr timed_task = task::sptr( new timed_wait_task(op,exe_time,t)); 
			_queue->safe_push(timed_task);
			BOOST_FOREACH(queue_worker::sptr w,_workers)
			{
				w->interrupt();

			}
		}

		void wait_idel()
		{

			BOOST_FOREACH(queue_worker::sptr w , _workers)
			{
				w->wait_idel();
			}
		}

		void reset()
		{
			if (!_working) return;
			stop_work();
			wait_work_end();
			_thread_vct.clear();
			_workers.clear();
			_queue.reset( new priority_task_queue);

			for(int i =0 ; i < _worker_cnt ; ++i)
			{
				queue_worker::sptr worker(new queue_worker(_queue.get(),_log));
				_workers.push_back(worker);
				thread_sptr  worker_thread( new boost::thread(boost::ref(*worker)));
				_thread_vct.push_back(worker_thread);
			}

		}

		~impl()
		{
			_working = false;
			_queue.reset(new priority_task_queue);
		}
	};


	timed_task_exec_unit::timed_task_exec_unit():_pimpl(new timed_task_exec_unit::impl) { }
	void timed_task_exec_unit::work( logger* log, int worker_cnt  )
	{
		_pimpl->work(log,worker_cnt);
	}

	void timed_task_exec_unit::stop_work()
	{
		_pimpl->stop_work();
	}

	void timed_task_exec_unit::wait_work_end()
	{
		_pimpl->wait_work_end();
	}

	void timed_task_exec_unit::join_task( time_t  exe_time, task::sptr t )
	{
		_pimpl->join_task(exe_time,t);
	}

	void timed_task_exec_unit::wait_idel()
	{
		_pimpl->wait_idel();
	}

	void timed_task_exec_unit::reset()
	{
		_pimpl->reset();
	}


}
