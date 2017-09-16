#ifndef __UTLS_H__
#define __UTLS_H__
#define SHM_KEY     0x924660
#include <stdexcept>
#include <string>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <boost/smart_ptr.hpp>
namespace pylon
{
	struct result
	{
		template <class T>
		static  T not_val(T r, T value , const char * info)
		{
			if(r == value)
			{
				char buf[4096] ;
				snprintf(buf,4096,"appinfo: %s,errno:%d errmsg:%s", info,errno, strerror(errno));
				throw  std::runtime_error(buf);
			}
			return r;
		}

		template <class T>
		static  T* not_null(T* r, const char * info)
		{
			if(r == NULL)
				throw  std::runtime_error(std::string(info));
			return r;
		}
	};

	template <class T>
	struct smarty_ptr
	{
		typedef boost::shared_ptr<T>    sptr;
		typedef boost::shared_array<T>  sarr;
		typedef boost::scoped_ptr<T>    cptr;    
		typedef boost::scoped_array<T>  carr;    
	};
}
#endif
