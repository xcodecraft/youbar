#include <boost/config/warning_disable.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <list>
#include "utls.h"
#include "text/rigger.h"
#include "text/text.h"
#include "rigger_impl.h"
#include <boost/foreach.hpp>
#include "error/error.h"
#include "log/log_boost.h"
#include <fstream>


#define IMPL_METHOD(RETURN,METHOD)  RETURN CLS::METHOD() { return _pimpl->METHOD(); }
#define IMPL_METHOD1(RETURN,METHOD,T1)  RETURN CLS::METHOD(T1 v1) { return _pimpl->METHOD(v1); }
#define IMPL_METHOD2(RETURN,METHOD,T1,T2)  RETURN CLS::METHOD(T1 v1,T2 v2) { return _pimpl->METHOD(v1,v2); }

using namespace std;
namespace  pylon { namespace rigger
    {



        struct conf_system::impl   : public conf_box , mem_pool<conf_system::impl>
        {/*{{{*/
            impl( const wstring& name ,conf* host ) : conf_box(keyword_t(),host){}

        };/*}}}*/

        conf_system::conf_system( const wstring& name  ): _pimpl(new conf_system::impl(name,0)){}

        void  conf_system::call(conf::order o ,const char* cmd ,const conf_args* args, pylon::logger* l)
        {
            _pimpl->call(o,cmd,args,l);
        }
#define CLS conf_system 



        IMPL_METHOD1(void,append, conf* ) ;



        void rigger::init()
        {
            log_kit::conf(L"rigger",L"./",log_kit::debug);
            INS_LOG(l,L"rigger");
            WLOG_INFO(l) << L"---------------------- start ---------------------";
        }
        void rigger::clear()
        {
        }
    }
}
