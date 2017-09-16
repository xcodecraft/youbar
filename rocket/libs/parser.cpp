#include <string>
#include <stdlib.h>
#include <stdio.h>
#include "reducer.h"
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/bind.hpp>
#include <list>
#include "method.cpp"
#include <iostream>
#include "logger.h"
#include <typeinfo>
#include <sstream>

#include "boost/date_time/posix_time/posix_time.hpp" 
#include <boost/date_time/gregorian/gregorian.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

enum parse_status
{
    TIMESTAMP,
    HOST,
    TITILE,
    NAME,
    KEY,
    DATA,
};

inline bool walk_to_s(const char* & line, size_t setp)
{
    for ( size_t i = 0  ;  i< setp  ; ++i)
    {
        if (*line  == 0) 
            throw runtime_error("string is end");
        ++line;
    }
    return true;
}

inline bool walk_to_k(const char* & line, char stop_key)
{
    while ( *line != stop_key)  
    {
        if (*line  == 0)
            throw runtime_error("string is end");
        ++line;
    }
    return true;
}


//2012-06-22T19:40:11+08:00
//2012-06-22 23:12:12
//Jun  6 10:55:55 s_local@xen02.wd.corp.qihoo.net hydra#pay#10001# {"app":"app1","event":"pay","key":"qid:10001","data":{"money":50}}
bool parser::parse(const char* line, event::sptr e)
{

    LOG_DEBUG("line: " <<line);
    //    size_t len = strlen(line);

    //parse timestamp
    try{
        const char * begin = line ;
        walk_to_s(line,19);
        e->time = string(begin,line);
        ++ line;
        //parse host
        begin  = line ;
        walk_to_k(line,' ') ;
        e->host = string(begin,line);
        ++ line;
        // parse hydra keyword
        begin  = line ;
        walk_to_k(line,'#') ;
        if ( line - begin  < 5 )  return false;
        if (string(begin,begin+5) != "hydra")
            return false;
        ++ line;
        // parse name ;
        begin  = line ;
        walk_to_k(line,'#') ;
        e->name = string(begin,line);
        ++ line;
        // parse key;
        begin  = line ;
        walk_to_k(line,'#') ;
        e->key= string(begin,line);
        //' '
        walk_to_s(line,2);
        e->data = string(line);
        trim(e->data);
        LOG_DEBUG( "name: " << e->name << "  key: " << e->key );
        return true;
    }
    catch (runtime_error& e)
    {
        return false;
    }
    return false;
}
