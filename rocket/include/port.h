#ifndef _PYLON_PORT_HPP__
#define _PYLON_PORT_HPP__
#if defined(OS_UNIX)
#       define PYLON_API
#else
#       define PYLON_API
#       ifndef PYLON_API 
#			if defined PYLON_EXPORTS
#				define PYLON_API __declspec(dllexport)
#			else
#				define PYLON_API __declspec(dllimport)
#			endif
#       endif
#endif 

#if defined(_MSC_VER) && (_MSC_VER >= 1400) 
	#pragma warning( disable : 4275 )
	#pragma warning( disable : 4251 )
    #define DISABLE_WARNING(X)  
#else 
    #define DISABLE_WARNING(X)  
#endif

#endif


