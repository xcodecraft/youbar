#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "stub.h"
#include <typeinfo>

using namespace std;
using namespace boost;

void stop_handler( boost::asio::io_service* io ,const boost::system::error_code& error, int signal_number)
{
    if (!error)
    {
        io->stop();
    }
}
namespace posix = boost::asio::posix;
void stub_work(void)
{
    boost::asio::io_service  _io_svc; 
    boost::asio::signal_set signals(_io_svc,SIGINT,SIGQUIT,SIGTERM);
    signals.async_wait(boost::bind(stop_handler,&_io_svc,_1,_2));
    _io_svc.run();
}
