#!/usr/bin/python2.7
import  re , os , string ,  getopt ,sys , unittest,logging, json 
sys.path.append(os.environ['HOME'] + "/devspace/rocket/bin")
import res
from py_pylon_text import *
from inspect import *



def import_res( ):
    for k,v in res.__dict__.items():
        if isclass(v)  :
            if issubclass(v,res.conf_base)  :
                obj = v();
                regist_module_object("res",v.__name__,obj);


if __name__ == '__main__':
    rigger.init()
    import_res();
    args = conf_args()
    object_call("res.res_t1",0,"config")
    object_call("res.res_t2",0,"config")
    
#    module_space.import_
#    rg = zconf(k
#    rg.import_file("../libs/text/test/y.conf")
#    import_res(rg)
#    rg.link()
#    rg.use("env.dev")
#    rg.use("admin")
#    finder = my_finder()
#    finder.init()
#    rg.regist("res",finder);
#    rg.call(0,"config","admin")
#    finder.clear()


    #unittest.main()
