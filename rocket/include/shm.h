#pragma  once;
#include "boost/interprocess/sync/interprocess_mutex.hpp"
#include "boost/interprocess/sync/interprocess_condition.hpp"
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>




namespace bi=boost::interprocess;

template < typename T > 
struct share_data
{/*{{{*/
	bi::interprocess_mutex			mutex;
	bi::interprocess_semaphore     semaphore;
	bi::interprocess_condition		data_update;
	T								data; 
	share_data():semaphore(0)
	{

	}
};/*}}}*/

struct  sync_base
{/*{{{*/
	bi::shared_memory_object*		_shm;
	std::string						_name;
	bi::mapped_region*				_region;
	bi::interprocess_mutex*			_mutex;
	bi::interprocess_semaphore*     _semaphore;
	bi::interprocess_condition*		_data_update;
	bi::interprocess_mutex*  mutex()
	{
		return _mutex;
	}
	void semaphore_wait()
	{
		_semaphore->wait();		
	}
	void semaphore_post()
	{
		_semaphore->post();
	}
	void update_notify_one()
	{
		_data_update->notify_one();
	}
	void update_notify_all()
	{
		_data_update->notify_all();
	}
	void update_wait(bi::scoped_lock<bi::interprocess_mutex>& lock)
	{
		_data_update->wait(lock);
	}
    bi::shared_memory_object* shared_memery()
    {
        return _shm;
    }

};/*}}}*/
template < typename T >
struct sync_master: public sync_base , public smart_def< sync_master<T> >
{/*{{{*/

	typedef  share_data<T> data_t; 
	data_t  *				_sync;
	sync_master(const std::string& name) 
	{ 
		_name = name;
		bi::shared_memory_object::remove(_name.c_str());
		_shm = new bi::shared_memory_object(bi::create_only,name.c_str(),bi::read_write);
		_shm->truncate(sizeof(data_t));
		_region = new bi::mapped_region(*_shm,bi::read_write );

		void * addr       = _region->get_address();
		_sync			= new (addr) data_t;
		_mutex			= &(_sync->mutex);
		_data_update	= &(_sync->data_update);
		_semaphore		= &(_sync->semaphore);



	}
	T *  data()
	{
		return &(_sync->data);
	}
	~sync_master()
	{ 
		_sync->mutex.lock();
		delete _region;
		delete _shm;
		bi::shared_memory_object::remove(_name.c_str());
	}
} ;/*}}}*/

template < typename T >
struct sync_slaver: public sync_base , public smart_def< sync_slaver<T > >
{/*{{{*/
	typedef  share_data<T> data_t; 
	data_t *				_sync;
	sync_slaver(const std::string& name) 
	{ 
		_name = name;
		_shm = new bi::shared_memory_object(bi::open_only  ,_name.c_str(),bi::read_write);
		_region = new bi::mapped_region(*_shm,bi::read_write );
		void * addr       = _region->get_address();
		_sync = static_cast<data_t* >(addr);
		_mutex = &(_sync->mutex);
		_data_update = & (_sync->data_update);
		_semaphore		= &(_sync->semaphore);


	}
	T *  data()
	{
		return &(_sync->data);
	}
	~sync_slaver()
	{ 
		delete _region;
		delete _shm;
	}
} ;/*}}}*/
