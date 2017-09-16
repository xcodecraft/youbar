#include "task_queue.h"
#include <boost/exception/all.hpp>
using namespace std;
using namespace boost;

namespace pylon
{
	void queue_worker::operator()()
	{/*{{{*/
		while(_need_run && _queue->wait_data() )
		{

			if( _queue->empty())
				continue;

			LOG_DEBUG(_logger)  <<  "execute  new  task" ;
			_cur_task = _queue->top();
			if(! _queue->safe_pop(_cur_task))
				continue;

			try
			{
				_task_run = true;
				if (_cur_task->get_status() != task::CANCLE)
				{
					_cur_task->_status = task::RUNNING;
					_cur_task->execute(_logger);
					_cur_task->_status = task::OVER;
				}
			}
			catch(boost::exception &e )
			{
				const char* err_msg = boost::diagnostic_information(e).c_str();
				//LOG_ERROR(_logger) <<  json::utf8to16(err_msg);
				LOG_ERROR(_logger) <<   err_msg;

			}
			catch(std::exception& e)
			{
				LOG_ERROR(_logger) <<  e.what() ;
			}
			catch(...)
			{
				LOG_ERROR(_logger) <<  L"unknow error ";
			}
			LOG_DEBUG(_logger)  << "task over";
			_task_run = false;

			boost::mutex::scoped_lock lock(_mutex);
			_idel_cond.notify_all();
			_run_end = true;
		}
	}/*}}}*/

	void queue_worker::stop()
	{
		_need_run = false;
		_queue->stop_wait();
		if(_cur_task)
			_cur_task->stop_execute();
	}

	void queue_worker::wait_idel()
	{

		_queue->wait_empty();
		boost::mutex::scoped_lock lock(_mutex);
		while(_task_run)
		{
			_idel_cond.wait(lock);
		}
	}

	void queue_worker::interrupt()
	{
		if( _cur_task)
			_cur_task->stop_execute(false);

	}
	void wait_task::stop_wait()
	{
		boost::lock_guard<boost::mutex> lock(_wait_mut);
		_wait_cond.notify_all();
	}
	bool wait_task::execute(logger* l)
	{

		LOG_DEBUG(l) <<  "wait "  << _wait_sec  << " sec start ......" ;
		boost::unique_lock<boost::mutex> lock(_wait_mut);
		boost::system_time  now=get_system_time();
		now  =  now +  boost::posix_time::seconds(_wait_sec);
		_wait_cond.timed_wait(lock, now  );
		LOG_DEBUG(l) <<  "wait "  << _wait_sec  << " sec end" ;
		return true;
	}

	void wait_task::stop_execute( bool stop)
	{
		wait_task::stop_wait();
	}

	wait_task::wait_task( int sec ):_wait_sec(sec)
	{

	}

	bool wait_task::interrupt()
	{
		stop_wait();
		return true;
	}
	bool timed_wait_task::execute( logger* l/*=NULL*/ )
	{
		if(_need_stop) return true;
		LOG_DEBUG(l) <<  "wait "  << _execute_time  << " sec start ......" ;
		boost::unique_lock<boost::mutex> lock(_wait_mut);
		_wait_cond.timed_wait(lock, posix_time::from_time_t(_execute_time));
		if(_need_stop) return true;
		time_t now;
		time(&now);
		if(now >= _execute_time )
		{
			_core_task->execute(l);
		}
		else
		{
			LOG_DEBUG(l) <<  "create re exec task " ;
			_safe_push(_execute_time,_core_task);
		}
		return true;
	}

	void timed_wait_task::stop_wait()
	{
		//boost::lock_guard<boost::mutex> lock(_wait_mut);
		_wait_cond.notify_all();
	}
	void timed_wait_task::stop_execute(bool stop)
	{
		_need_stop=stop;
		stop_wait();
	}

	long timed_wait_task::priority()
	{
		return 0 - (long)_execute_time;
	}

    /*

	bool cmp_priority_task( priority_task::sptr first ,priority_task::sptr second )
	{
		priority_task* first_ori = (priority_task*) first.get();	
		priority_task* second_ori = (priority_task*) second.get();	
		return  first_ori->priority() > second_ori->priority();
	}
    */
}
