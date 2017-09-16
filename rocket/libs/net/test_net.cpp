#include "task/task.h"
#include "task/thread_task.h"
#define BOOST_TEST_MODULE TASK_NET
#include <boost/test/included/unit_test.hpp>
#include "net/remote.h"
#include <fstream>
using namespace std;
using namespace boost;
using namespace pylon;


BOOST_AUTO_TEST_CASE(test_net)
{
	curl_accessor  c ;
	ofstream  jpg("d:/x.jpg",ios::binary|ios::trunc);
	c.get("http://w.qhimg.com/images/v2/site/360/823/slide_1.jpg",jpg);
}