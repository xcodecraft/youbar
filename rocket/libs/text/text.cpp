#include <boost/foreach.hpp>
#include "text/text.h"
#include "utf8.h"

namespace pylon
{
    std::string utf16to8(const std::wstring& u16)
    {
        std::string u8;
        utf8::utf16to8(u16.begin(),u16.end() , back_inserter(u8));
        return u8;
    }
    std::wstring utf8to16( const std::string& u8)
    {
        std::wstring u16;
        utf8::utf16to8(u8.begin(),u8.end() , back_inserter(u16));
        return u16;
    }
}
