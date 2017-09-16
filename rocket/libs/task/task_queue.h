#pragma  once
#include "port.h"
#include "utls.h"
#include "error/log.h"
#include <boost/thread.hpp>
#include <boost/heap/priority_queue.hpp>
#include <queue>
#include "task/task.h"
namespace pylon
{
	template < typename T> 
	class safe_queue_tpl :public smarty_ptr< safe_queue_tpl <T> > 
	{/*{{{*/
	public:
		virtual bool wait_data()=0;
		virtual void wait_empty()=0;
		virtual T	 top()=0;
		virtual void stop_wait()=0;
		virtual bool safe_pop(T& t)=0;
		virtual void safe_push(T& t)=0;
		virtual bool empty()=0;
		virtual ~safe_queue_tpl(){}
	};/*}}}*/
	template < typename T> 
	class thread_queue : public safe_queue_tpl<T> 
	{/*{{{*/
		boost::mutex _mutex;	
		boost::condition_variable _need_work_cond;
		boost::condition_variable _exec_task_cond;
		std::queue<T>            _queue_impl;

	public:
		bool wait_data()
		{
			if(!_queue_impl.empty())
			{
				return true;
			}
			boost::mutex::scoped_lock lock(_mutex);
			if(_queue_impl.empty())
			{
				_need_work_cond.wait(lock);
			}
			return !_queue_impl.empty();

		}
		bool empty()
		{
			return _queue_impl.empty();
		}

		void wait_empty()
		{
			if(_queue_impl.empty())
				return ;
			boost::mutex::scoped_lock lock(_mutex);
			while(!_queue_impl.empty())
			{
				_exec_task_cond.wait(lock);
			}
		}
		T top()
		{
			return _queue_impl.front();
		}
		void stop_wait()
		{
			boost::mutex::scoped_lock lock(_mutex);
			_need_work_cond.notify_all();
		}
		bool safe_pop(T& t)
		{
			boost::mutex::scoped_lock lock(_mutex);
			if(_queue_impl.empty())
				return false;
			t = _queue_impl.front();
			_queue_impl.pop();
			_exec_task_cond.notify_one();
			return true;
		}
		void safe_push(T& t)
		{
			boost::mutex::scoped_lock lock(_mutex);
			_queue_impl.push(t);
			_need_work_cond.notify_one();
		}

	};/*}}}*/

	template < typename T> 
	class thread_priority_queue  : public safe_queue_tpl<T>
	{
		boost::mutex				_mutex;	
		boost::condition_variable _need_work_cond;
		boost::condition_variable _exec_task_cond;
//        std::vector<T>          _queue_impl;
        boost::heap::priority_queue<T> _queue_impl;
		bool wait_data()
		{
			if(!_queue_impl.empty())
			{
				return true;
			}
			boost::mutex::scoped_lock lock(_mutex);
			if(empty())
			{
				_need_work_cond.wait(lock);
			}
			return !empty();

		}
		void wait_empty()
		{
			if(_queue_impl.empty())
				return ;
			boost::mutex::scoped_lock lock(_mutex);
			while(!_queue_impl.empty())
			{
				_exec_task_cond.wait(lock);
			}
		}
		T top()
		{
            return _queue_impl.top();
		}
		void stop_wait()
		{
			boost::mutex::scoped_lock lock(_mutex);
			_need_work_cond.notify_all();
		}
		bool safe_pop(T& t)
		{
			boost::mutex::scoped_lock lock(_mutex);
			if(this->_queue_impl.empty())
				return false;
            _queue_impl.pop();
			_exec_task_cond.notify_all();
			return true;
		}
		void safe_push(T& t)
		{
			boost::mutex::scoped_lock lock(_mutex);
			_queue_impl.push(t);
			_need_work_cond.notify_one();
		}
		bool empty()
		{
			return _queue_impl.empty();
		}


	};
	typedef thread_priority_queue<priority_task::sptr> priority_task_queue;
	typedef safe_queue_tpl<task::sptr> safe_queue;
	typedef thread_queue< task::sptr > task_queue;
	typedef thread_queue<boost::shared_ptr<std::wstring> > task_msg_queue;
	typedef thread_queue<boost::shared_ptr<std::wstring> > msg_queue;

	class  queue_worker : public smarty_ptr<queue_worker> //,  public sptr_vector<queue_worker>
	{
	public:
		typedef  std::vector< boost::shared_ptr<queue_worker > > parr;
	private:
		safe_queue *				_queue ;
		bool						_need_run;
		bool						_run_end;
		task::sptr   				_cur_task;
		logger*						_logger;
		boost::mutex				_mutex;	
		boost::condition_variable	_idel_cond;
		bool						_task_run;
	public:
		queue_worker(safe_queue* q,  logger* log)
			:_queue(q), _need_run(true),_run_end(false),_logger(log),_task_run(false)
		{
		}
		void stop();
		bool is_end()
		{
			return _run_end;
		}
		void wait_idel();
		void interrupt();
		void operator()();
	};


	class   wait_task :    public task
	{
		int			_wait_sec;
		bool		_need_stop;
		boost::condition_variable _wait_cond;
		boost::mutex _wait_mut;
	public:

		wait_task(int sec);
		virtual void  stop_execute( bool stop=true);
		virtual bool execute(logger* log=NULL);
		void stop_wait();
		virtual bool interrupt();
	};

	class timed_wait_task  :  public task
	{
	public:
		typedef boost::function<void ( time_t ,task::sptr)> push_timed_task_t;
		timed_wait_task(push_timed_task_t push,  time_t  exe_time, task::sptr t )
			:_safe_push(push),_execute_time(exe_time),_need_stop(false),_core_task(t)
		{
		}
		virtual long  priority();
		virtual void  stop_execute( bool stop=true);
		virtual bool execute(logger* log=NULL);
		void stop_wait();
	private:
		push_timed_task_t			_safe_push;
		time_t 						_execute_time;
		bool						_need_stop;
		boost::condition_variable	_wait_cond;
		boost::mutex				_wait_mut;
		task::sptr					_core_task; 
	};

//	bool cmp_priority_task( priority_task::sptr first ,priority_task::sptr second );
}
