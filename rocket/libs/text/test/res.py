import  re , os , string ,  sys  
from py_pylon_text import *


class conf_base(conf_obj):
    def config(self,order,args):
        pass
    def bind_prop(self,args):
        for k,v in self.__class__.__dict__.items() :
            if re.match(r'^_[a-z,A-Z,0-9]+',k):
                if not args.has_key(k):
                    k = k[1:]
                    args[k.upper()] = v 
        return args
    def call(self,order,cmd,args):
        args = self.bind_prop(args) 
        if cmd == "config":
            self.config(order,args)


class res_t1(conf_base):
    _name = "res_t1"
    _name1_ = "res_t2"
    ___name2 = "res_t2"
    def config(self,order,args):
        print("python :: res_1")
        assert( args["NAME"] == "res_t1" )
        assert( args["HOME"] == "GOOGLE" )
        assert(not args.has_key("NAME1"))
        assert(not args.has_key("NAME2"))

class res_t2(conf_base):
    _name = "res_t2"
    def config(self,order,args):
        print("python :: res_2")
        assert( args["NAME"] == "res_t2" )
        assert( args["HOME"] == "GOOGLE" )
        assert(not args.has_key("NAME1"))
        assert(not args.has_key("NAME2"))
