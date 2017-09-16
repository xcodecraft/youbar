#ifndef __CONF_CMD__HPP__ 
#define __CONF_CMD__HPP__ 
#include <string>
#include "port.h"
#include "pimpl.h"
#include "utls.h"
#include "log/log.h"


namespace  pylon { namespace rigger
    {
		struct conf_object ;
//        struct conf_cmd  : public pylon::smarty_ptr<conf_cmd>
//        {
//            virtual void config(	conf_object* obj, conf_space* host) =0 ;
//            virtual void start(		conf_object* obj, conf_space* host) =0 ;
//            virtual void stop(		conf_object* obj, conf_space* host) =0 ;
//            virtual void reload(	conf_object* obj, conf_space* host) =0 ;
//            virtual void clean(		conf_object* obj, conf_space* host) =0 ;
//            virtual void data(		conf_object* obj, conf_space* host) =0 ;
//            virtual void call( const char* cmd ,conf_object* obj, conf_space* host,pylon::logger* l);
//            virtual ~conf_cmd(){}

//        };
//        struct cmd_finder : public pylon::smarty_ptr<cmd_finder>
//        {
//            virtual conf_cmd* find(const wchar_t* name, pylon::logger* l) { return NULL;}
//            virtual ~cmd_finder(){}
//        };





    }


}
#endif
