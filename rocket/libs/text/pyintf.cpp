#include <boost/python.hpp>
#include "text/text.h"
#include "text/conf_core.h"
#include "log/log.h"
using namespace std;
using namespace boost;
using namespace pylon;
using namespace boost::python;
using namespace pylon::rigger;


template <typename T>
bool to_object( conf::value_type& val,object& obj)
{
    if(T *p = boost::get<T>(&val))
    {
        obj = object(*p);
        return true ;
    }
    return false;
}

bool str_to_object( conf::value_type& val,object& obj)
{
    if(wstring *p = boost::get<wstring>(&val))
    {
        string x = utf16to8(*p);
        obj = object(x);
        return true ;
    }
    return false;
}


struct py_object : smarty_ptr<py_object>
{
    py_object( conf_object* o):_obj(o){}
    conf_object* _obj;
    object get_val(string n)
    {
        wstring wn = utf8to16(n);
        conf::value_type val;
        object obj;
        if(_obj->get_val(wn,val))
        {
            if(to_object<int>(val,obj)) return obj;
            if(to_object<float>(val,obj)) return obj;
            if(str_to_object(val,obj)) return obj;
        }
        return  obj;
    }
};
struct py_space  : smarty_ptr<py_space>
{
    py_space( conf_space* s):_space(s){}
    conf_space*  _space;
} ;

struct pyconf_obj :  public conf_object
{/*{{{*/
    typedef boost::shared_ptr<pyconf_obj> optr;
    pyconf_obj(const std::string& name ):conf_object(L"")
    {
        init_props()
    }
    virtual void init_props()
    {
        if (override f = this->get_override("init_props"))
            f();
    }
    virtual void config()
    {
        config4py();
    }
    object get_val(string n)
    {
        wstring wn = utf8to16(n);
        conf::value_type val;
        object obj;
        if(get_val(wn,val))
        {
            if(to_object<int>(val,obj)) return obj;
            if(to_object<float>(val,obj)) return obj;
            if(str_to_object(val,obj)) return obj;
        }
        return  obj;
    }
    void regist_prop(string key, object val)
    {
        append(key,conf::sptr(new conf_value(key,1)));
    }
    void config4py(py_object& obj ,py_space& host)
    {
        if (override f = this->get_override("config"))
            f(); 
    }

};/*}}}*/

class py_rg : public zconf
{
    void regist_pyobj( const char* path, pyconf_obj::optr pyobj )
    {
        regist_obj(path,obj)

    }
};
BOOST_PYTHON_MODULE(py_pylon_text)
{/*{{{*/

    class_<conf,boost::noncopyable>("conf")
        .def("use",&conf::use)
        .def("extend",&conf::extend)
//        .def("xpath",&conf::xpath)
        ;
    class_<conf_value>("conf_value")
        .def("use",&conf_value::use)
        ;


    class_<py_space>("conf_space",init<conf_space*>())
        ;
    class_<zconf,boost::noncopyable>("zconf",init<>())
        .def("import_file",&zconf::import)
        .def("link",  &zconf::link)
        .def("use"  , &zconf::use)
        .def("call", &zconf::call2)
        .def("regist_obj", &zconf::regist_obj)
        ;
    class_<pyconf_obj,boost::noncopyable>("conf_obj",init<>())
        .def("config"   , pure_virtual(&pyconf_obj::config4py))
        .def("start"    , pure_virtual(&pyconf_obj::start))
        .def("stop"     , pure_virtual(&pyconf_obj::stop))
        .def("reload"   , pure_virtual(&pyconf_obj::reload))
        .def("clean"    , pure_virtual(&pyconf_obj::clean))
        .def("regist"   , pure_virtual(&pyconf_obj::clean))
        void .def("val"      ,&py_object::get_val)
        ;
}/*}}}*/

