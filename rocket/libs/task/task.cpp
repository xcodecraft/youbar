#include "port.h"
#include "task/task.h"

namespace pylon
{

    struct priority_task::impl
    {
        task::sptr _ori_task;
        int        _priority;
        impl(int p, task::sptr t):_ori_task(t),_priority(p){}
		void stop_execute( bool stop=true)
        {
            _ori_task->stop_execute(stop);
        }
		bool execute(logger* logobj=NULL) 
        {
            return _ori_task->execute(logobj);
        }
		int priority() const 
        {
            return _priority ;
        }
    };
    priority_task::priority_task(int p, task::sptr t ) : _pimpl(new impl(p,t)){}

    void priority_task::stop_execute(bool stop)
    {
        _pimpl->stop_execute(stop);
    }
    bool priority_task::execute(logger* log )
    {
        return _pimpl->execute(log);
    }

    int priority_task::priority() const 
    {
        return  _pimpl->priority();
    }
    bool operator == (const priority_task& first  ,const priority_task& second)
    {
        return first.priority() == second.priority() ;
    }
    bool operator <  (const priority_task& first  ,const priority_task& second)
    {
        return first.priority() < second.priority() ;
    }

    struct ring_task::impl 
    {/*{{{*/
        impl( push_op_t safe_push,task::sptr& t, int times )
            :_safe_push(safe_push),_task(t),_times(times)
        { }
        void stop_execute(bool stop=true)
        {
            _task->stop_execute(stop);
        }
        bool execute(logger* l=NULL) 
        {
            if(_times ==0 )
                return true;
            bool ret = _task->execute(l);
            if(_times > 0 )
                _times -- ;
            LOG_DEBUG(l) << "create new ring task " ;
            task::sptr task_obj(new ring_task(_safe_push,_task,_times));
            _safe_push(task_obj);
            return ret; 
        }

        push_op_t  _safe_push;
        task::sptr _task ;
        int       _times;
    };/*}}}*/

    bool ring_task::execute(logger* l)
    {
        return _pimpl->execute(l);
    }

    void ring_task::stop_execute(bool stop)
    {
        _pimpl->stop_execute(stop);
    }

    ring_task::ring_task( push_op_t safe_push,task::sptr& t, int times )
        :_pimpl(new impl(safe_push,t,times))
    { }

}
