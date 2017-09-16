#ifndef  __MYFILE_HPP__
#define  __MYFILE_HPP__
#include "port.h"
#include "pimpl.h"
#include "text/xjson.h"
#include "error/log.h"
namespace  wan360
{
	struct gdict
	{
		gdict();
		bool gdict::list_zone_by_gkey(const char * pos , const char* gkey ,pylon::json::node::sptr& node ,pylon::logger* l  );
		PIMPL_DEF;
	};
}
#endif
