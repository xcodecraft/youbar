#include "wan/gdict.h"
#include "net/remote.h"
#include "text/xjson.h"
#include "error/log.h"
#include <boost/format.hpp>
using namespace std;
using namespace pylon;
using namespace boost;
namespace  wan360
{
	struct gdict::impl
	{
		bool list_zone_by_gkey(const char * pos , const char* gkey ,json::node::sptr& node ,logger* l  )
		{
			curl_accessor  curl;
			stringstream   data;
			json::parser   jp;

			//url = format("http://api.gadmin.svc.1360.com:8360/svc.php?do=listZoneByGkey?pos=%1gkey=%2") %2310 %"sxd";
			//string url("http://api.gadmin.svc.1360.com:8360/svc.php?do=listZonesByGkey?pos=%2310gkey=sxd&__echo=json");
			string url("http://api.gadmin.svc.1360.com:8360/svc.php?do=help&__echo=json");

			//call_result result = curl.get(url.c_str(),data,l,8360);
			call_result result = curl.get(url.c_str(),data,l,1000,8360);
			if(result)
			{
				if(jp.parse_utf8(data.str()))
				{
					node = jp.root();
					return true;
				}
			}
			return false ;
		}
	};
	gdict::gdict():_pimpl(new impl)
	{}
	bool gdict::list_zone_by_gkey(const char * pos , const char* gkey,json::node::sptr& node ,logger* l  )
	{
		return _pimpl->list_zone_by_gkey(pos,gkey, node,l);
	}
}
