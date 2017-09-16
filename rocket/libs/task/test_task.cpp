#include "task/task.h"
#include "task/thread_task.h"
#define BOOST_TEST_MODULE TASK_UT
#include <boost/test/included/unit_test.hpp>
using namespace std;
using namespace boost;
using namespace pylon;

class   wait_task2 :    public task
{
	int       _wait_sec;
public:
	wait_task2(int sec):_wait_sec(sec)
	{

	}
	virtual void  stop_execute(bool stop){}
	virtual bool execute(logger* l=NULL)
	{
		//Sleep(_wait_sec* 1000);
		cout<< "wait " << _wait_sec << " sec!" << endl;
		return  true;
	}
};
BOOST_AUTO_TEST_CASE(exe_unit)
{
	task_exec_unit eunit;
	
	eunit.join_task(task::sptr(new wait_task2(5)));
	eunit.join_task(task::sptr(new wait_task2(5)));
	eunit.join_task(task::sptr(new wait_task2(5)));
	eunit.join_task(task::sptr(new wait_task2(5)));
	eunit.work(NULL,2);
	eunit.wait_idel();
	eunit.stop_work();
	eunit.wait_work_end();
}

/*
class   curl_test_task :    public task
{
public:
	virtual void  stop_execute(bool stop){}
	virtual bool execute(logger* l=NULL)
	{
        char *url ="http://zion.caiyun.cc/live.html";
        stringstream outs;

        curl_accessor curl;
        curl.get(url,outs);
        std::cout << outs.str().c_str();
        return true;
	}
};


BOOST_AUTO_TEST_CASE(muti_curl_test)
{
	task_exec_unit eunit;
    for(int i =0 ; i<10 ; ++i)
        eunit.join_task(task::sptr(new curl_test_task));
	eunit.work(NULL,NULL,2);
	eunit.wait_idel();
	eunit.stop_work();
	eunit.wait_work_end();
}

*/




class echo_task : public task  
{
	std::string _msg;
public:
	echo_task(const char * msg)
	{
		_msg = msg;
	}
	virtual void stop_execute(bool stop)
	{

	}
	virtual bool execute(logger* logger=NULL)
	{
		cout << "echo task :"  << _msg.c_str() <<endl;
		return true;
	}
};



BOOST_AUTO_TEST_CASE(test_timed_exit)
{

	timed_task_exec_unit eunit;
	time_t now;
	time(&now);

	eunit.join_task( now+5 ,task::sptr(new echo_task("5")));
	eunit.join_task( now+7 ,task::sptr(new echo_task("7")));
	eunit.work(NULL,1);
	eunit.join_task( now+1 ,task::sptr(new echo_task("1")));
	eunit.join_task( now+8 ,task::sptr(new echo_task("8")));
	eunit.join_task( now+3 ,task::sptr(new echo_task("3")));
	eunit.join_task( now+3 ,task::sptr(new echo_task("3")));
	//eunit.reset();
	eunit.wait_idel();
	eunit.stop_work();						 
	eunit.wait_work_end();
}
