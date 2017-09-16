#ifndef __RIGGER_IMPL__HPP__ 
#define __RIGGER_IMPL__HPP__ 
#include "text/rigger.h"
#include <boost/foreach.hpp>
using namespace std;
namespace  pylon { namespace rigger
    {
        struct conf_box;
        typedef void (*box_call_t) (conf_box * ,const std::wstring&)  ;
        struct conf_box
        {/*{{{*/
            typedef std::vector<conf_object*> obj_vector;
            typedef std::pair<std::wstring, conf*>  pair;
            typedef std::map<std::wstring, conf*>   map;
            keyword_t				_keyword;
            map						_dict; 
            conf::sptr_vector		_arr;
            conf*					_host;
            conf_args               _args;
            conf_box( keyword_t k, conf* host) : _keyword(k),_host(host) {}
            virtual ~conf_box(){}

            virtual void clear()
            {/*{{{*/
                BOOST_FOREACH(conf* i , _arr)
                {
                    i->clear();
                }
                _dict.clear();
                _arr.clear();
                _host = NULL;
            }/*}}}*/
            void to_conf( std::wostream& o, std::wstring prefix)
            {/*{{{*/
                   o <<  prefix <<  L" {\n " ;
                   for( conf::sptr_vector::iterator it = _arr.begin() ; it != _arr.end(); ++it)
                   {
                       (*it)->to_conf(o,prefix+L"\t") ;
                   }
                   o << prefix << L"}\n" ;
            }/*}}}*/

            void append( conf* value )
            {/*{{{*/
                _arr.push_back(value);
                _dict[ value->name() ]  = value; 
                INS_LOG(l,L"rigger");
                WLOG_DEBUG(l) <<  value->name();
            }/*}}}*/

            void   set_default_args(const conf_args* args)
            {
                _args = *args;     
            }
            conf_args*  default_args()
            {
                return &_args;
            }

            bool get_sub(wstring& key, conf*& v)
            {/*{{{*/
                map::iterator found_it=  _dict.find(key);
                if (found_it ==  _dict.end())
                {
                    return false;

                }
                v=  found_it->second ;
                return true;
            }/*}}}*/

            conf* xpath(  conf::path_arr_t key_arr) 
            {/*{{{*/
                conf*  result=NULL ;
                INS_LOG(l,L"rigger");
                if ( key_arr.size() >= 1 )
                {
                    conf::path_type key   = key_arr.front(); 
                    key_arr.pop_front();
                    if( get_sub(key,result))
                    {
                        if ( key_arr.size()  ==  0 )
                        {
                            return  result;
                        }
                        return result->xpath(key_arr);
                    }
                    else
                    {
                        WLOG_WARN(l) << L"xpath unfound at : " << key ;
                    }

                }
                return   result ;
            }/*}}}*/

            void  call(conf::order o ,const char* cmd ,const conf_args* args, pylon::logger* l)
            {/*{{{*/
                if(o == conf::positive)
                {
                    BOOST_FOREACH(conf* x , _arr)
                    {
                        conf_object* obj= dynamic_cast<conf_object* >(x);
                        if( obj != NULL)
                        {
                            conf_args now = *(obj->default_args());
                            now.merge(args);
                            obj->call(o,cmd,&now,l);
                        }
                    }
                }
                else
                {
                    BOOST_REVERSE_FOREACH(conf* x , _arr)
                    {
                        conf_object* obj= dynamic_cast<conf_object* >(x);
                        if( obj != NULL)
                        {
                            conf_args now = *(obj->default_args());
                            now.merge(args);
                            obj->call(o,cmd,&now,l);
                        }
                    }
                }

            }/*}}}*/

        };/*}}}*/
    }
}
#endif

