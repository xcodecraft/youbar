#include <boost/python.hpp>
#include "text/text.h"
#include "text/rigger.h"
#include "log/log.h"
#include <boost/foreach.hpp>
using namespace std;
using namespace boost;
using namespace pylon;
using namespace boost::python;
using namespace pylon::rigger;
namespace py=boost::python ;
namespace rg=pylon::rigger ;


class  pyconf_args   : public conf_args
{
    public:
        py::object get(const char* c)
        {
            wstring key=utf8to16(c);
            if (find(key) != end())
            {
                args_type v = this->operator[](key);
                if( int* pv = boost::get<int>(&v))
                    return py::object(*pv);
                if( float* pv = boost::get<float>(&v))
                    return py::object(*pv);
                if( wstring* pv = boost::get<wstring>(&v))
                    return py::object(*pv);
            }
            return py::object();
        }
        void set(const char* key ,py::object o)
        {
            
        }
};
py::dict conv2dict(const conf_args& args)
{
    py::dict dct;
    BOOST_FOREACH(conf_args::pair i , args)
    {
        args_type v = i.second ;
        string key = utf16to8(i.first);
        if( int* pv = boost::get<int>(&v))
            dct[key] = *pv;
        if( float* pv = boost::get<float>(&v))
            dct[key] = *pv;
        if( wstring* pv = boost::get<wstring>(&v))
        {
            string  str_v = utf16to8(*pv);
            dct[key] =  str_v ;
        }

    }
    return dct ;
}
struct pyconf_object :  public conf_object ,wrapper<pyconf_object> 
{/*{{{*/
    typedef boost::shared_ptr<pyconf_object> sptr;
    pyconf_object( ):conf_object(L"pyconf_object") { }

    virtual void  call(conf::order o ,const char* cmd ,const conf_args* args, pylon::logger* l)
    {

        WLOG_INFO5(l) << L"call : " << utf8to16(cmd)   ;
        if (override f = this->get_override("call"))
        {
            pyconf_args py_args;
            py_args.merge(args);
            f((int)o,cmd,conv2dict(py_args)); 
        }
    }
};/*}}}*/

typedef std::list<pyconf_object::sptr> object_hold_t;
void regist_module_object(const string module_name, const string obj_name ,pyconf_object::sptr obj )
{
    static object_hold_t holds;
    INS_LOG(l,L"rigger");
    wstring umodule = utf8to16(module_name);
    wstring uobj    = utf8to16(obj_name);
    obj->set_name(uobj);
    WLOG_DEBUG(l) << L"module : " <<  umodule << " obj : " << obj->name() ;
    //保护传入对象的生命周期;
    holds.push_back(obj);
    module_space::regist_obj(umodule,obj.get());
}

void object_call(const string xpath, int order,const string cmd  )
{
    conf* p =  module_space::xpath(utf8to16(xpath));
    INS_LOG(l,L"rigger");
    WLOG_DEBUG(l) << L"xpath : "  << utf8to16(xpath)  << L" cmd : " << utf8to16(cmd)  ;
    if(p !=NULL)
    {
        WLOG_DEBUG(l) << L"cmd: "  << utf8to16(cmd)  << L" obj: " << p->name() ;
        conf_args a1;
        a1[L"HOME"] = L"GOOGLE";
        p->call((conf::order)order,cmd.c_str(),&a1,l);
    }
}

BOOST_PYTHON_MODULE(py_pylon_text)
{/*{{{*/

    class_<pyconf_args>("conf_args")
        .def("get",&pyconf_args::get)
        ;
    class_<pyconf_object>("conf_obj")
            .def("call"      ,&pyconf_object::call)
            .def("name"      ,&pyconf_object::name)
            ;
    class_<module_space>("module_space")
        .def("regist_obj"      ,&module_space::regist_obj2).staticmethod("regist_obj")
        ;
    def("regist_module_object",&regist_module_object);
    def("object_call",&object_call);

    class_<rg::rigger>("rigger")
        .def("init"      ,&rg::rigger::init).staticmethod("init")
        .def("clear"     ,&rg::rigger::init).staticmethod("clear")
        ;

}/*}}}*/


