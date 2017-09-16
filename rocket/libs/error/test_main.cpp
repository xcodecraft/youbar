#include "error/error.h"
#include "error/log.h"


using namespace pylon ;

using namespace std;

int main (void )
{
    log_def::conf(L"test1" , L"D:/",log_def::debug);
    log_def::conf(L"test2" , L"D:/",log_def::info);
	INS_LOG(log1,L"test1");
	INS_LOG(log2,L"test2");
	LOG_DEBUG(log1) << "debug";
	LOG_DEBUG(log2) << "debug";
	LOG_INFO(log2) << "info";
	log_def::clear();
    return 0 ;
}
