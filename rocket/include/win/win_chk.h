#pragma once
//#include "common/err_def.h"
//#include <atlbase.h>

template < typename T, typename X,typename ERR_TYPE>
inline T ensure_winapi( T result , ERR_TYPE& err, X failed ,const char*  exp_info ,const char * fun_name ,int line)
{
	if ( result == (T)failed)
		throw err <<  err_info(exp_info,fun_name,line,GetLastError());
	return result ;
}

template <  typename ERR_TYPE>
inline HRESULT ensure_com(HRESULT result , ERR_TYPE& err, const char*  exp_info ,const char * fun_name ,int line)
{
	if (! SUCCEEDED(result) )
		throw err <<  err_info(exp_info,fun_name,line,GetLastError());
	return result ;
}


#ifndef WAPI_NOT_NULL
#define WAPI_NOT_NULL(EXP,ERR) \
	ensure_winapi(EXP,ERR,NULL,#EXP,__FUNCTION__,__LINE__);  
#endif

#ifndef WAPI_NOT_V
#define WAPI_NOT_V(EXP,ERR,FAILD) \
	ensure_winapi(EXP,ERR,FAILD,#EXP,__FUNCTION__,__LINE__);  
#endif

#ifndef WAPI_NOT_FALSE
#define WAPI_NOT_FALSE(EXP,ERR) \
	ensure_winapi(EXP,ERR,FALSE,#EXP,__FUNCTION__,__LINE__);  
#endif

#ifndef COM_OK
#define COM_OK(EXP,ERR) \
	ensure_com(EXP,ERR,#EXP,__FUNCTION__,__LINE__);  
#endif
