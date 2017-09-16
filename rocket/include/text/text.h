#ifndef __TEXT__HPP__ 
#define __TEXT__HPP__ 
#include "port.h"
#include "utls.h"

namespace  pylon { 

        PYLON_API   std::string utf16to8(const std::wstring& u16) ;
        PYLON_API   std::wstring utf8to16( const std::string& u8) ;
}
#endif
