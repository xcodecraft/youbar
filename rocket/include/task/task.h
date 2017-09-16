#ifndef __PYLON_TASK_HPP__
#define __PYLON_TASK_HPP__
#include "port.h"
#include "pimpl.h"
#include "utls.h"
#include "error/log.h"
#include <boost/function.hpp>
namespace pylon
{

	struct  PYLON_API cmd  
	{
		virtual bool execute() = 0 ;
		virtual ~cmd(){}
	};

	struct  PYLON_API task:   public  smarty_ptr<task>
	{
		enum status_t{ READY,RUNNING, OVER,CANCLE};
		status_t     _status;
		task():_status(READY){};
		virtual void stop_execute( bool stop=true)=0;
		virtual bool execute(logger* logobj=NULL) = 0 ;
		virtual status_t get_status()
		{
			return _status;
		}
		virtual ~task(){}
	};

	struct  PYLON_API priority_task:   public  task
	{
        priority_task(int priority , task::sptr t);
		virtual void stop_execute( bool stop=true);
		virtual bool execute(logger* logobj=NULL) ;
		int priority() const ;
		PIMPL_DEF;
	};
    bool operator == (const priority_task& first  ,const priority_task& second);
    bool operator <  (const priority_task& first  ,const priority_task& second);


	class  PYLON_API ring_task : public task   
	{
	public:

		typedef boost::function<void (task::sptr)> push_op_t;
		ring_task( push_op_t safe_push,task::sptr& t , int times=-1);
		virtual void stop_execute(bool stop=true);
		virtual bool execute(logger* logger=NULL) ;
		PIMPL_DEF;
	};



}
#endif 
