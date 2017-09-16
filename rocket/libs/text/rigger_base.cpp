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
#include <fstream>
//#include <boost/fusion/sequence/intrinsic/at_c.hpp>
//#include <boost/fusion/include/at_c.hpp>

#define IMPL_METHOD(RETURN,CLS,METHOD)  RETURN CLS::METHOD() { return _pimpl->METHOD(); }
#define IMPL_METHOD1(RETURN,CLS,METHOD,T1)  RETURN CLS::METHOD(T1 v1) { return _pimpl->METHOD(v1); }
#define IMPL_METHOD2(RETURN,CLS,METHOD,T1,T2)  RETURN CLS::METHOD(T1 v1,T2 v2) { return _pimpl->METHOD(v1,v2); }

using namespace std;
namespace  pylon { namespace rigger
    {

        std::wostream & operator << (std::wostream& out  , const var_ref& v )
        {
            out << v.name;
            return out ;
        }
        void conf_value::to_conf(std::wostream& o, std::wstring prefix)
        {
            o << prefix <<  _name << L" : \t" <<  _value << endl ;
        }
        void conf_value::clear()
        {}

        void conf_args::merge(const conf_args* other)
        {
            BOOST_FOREACH( conf_args::pair i , *other)
            {
                (*this)[i.first] =  i.second ;
            }
        }


    }
}
