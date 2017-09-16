#!/usr/bin/python2.7
import  re , os , string ,  getopt ,sys , unittest,logging, json,  copy

from py_reducer import *

from reducer import *

def test_e(e):
    print(e.name)

class test_outer(outer):
    def out(self,e):
        self.e = e 
#        print(self.e.name)
        
class test_outer_pkg(outer):
    def out(self,e):
        self.e = e 
        data = json.JSONDecoder().decode(e.data)
#        print(len(data))
        assert len(data)==4

class test_in(iner):
    def set_reducer(self,reducer):
        self.reducer = reducer 
        self.time    = 0 
    def get(self):
        if self.time >= 10 :
            self.reducer.stop()
        self.time  += 1

        return  """Jun  6 10:55:55 s_local@xen02.wd.corp.qihoo.net hydra#pay#10001# {"app":"app1","event":"pay","key":"qid:10001","data":{"money":50}}"""



class reducer_tc(unittest.TestCase):
    def setUp(self):
        line = """Jun  6 10:55:55 s_local@xen02.wd.corp.qihoo.net hydra#pay#10001# {"app":"app1","event":"pay","key":"qid:10001","data":{"money":50}}"""
        self.data       =  [line,line,line];
        self.parser     = parser()
        self.reducer    = reducer()
        level_log(0)
        self.outer      = test_outer();
#        self.reducer.set_out(self.outer)

    def tearDown(self):
        self.reducer    = None
        self.outer      = None

        
    def test_pack(self):
        return 
        self.reducer.set_out(self.outer)
        self.reducer.pack("pay",2,2)
        x = event()
        for line in self.data:
            self.parser.parse(line,x)
            self.reducer.push(x)
        self.assertEqual(self.outer.e.name,"pay")
        expect= '[  {"app":"app1","event":"pay","key":"qid:10001","data":{"money":50}}, {"app":"app1","event":"pay","key":"qid:10001","data":{"money":50}} ] '
        self.assertEqual(self.outer.e.data.strip(),expect.strip())
        data = json.JSONDecoder().decode(expect)
        self.assertEqual(len(data),2)

    def test_noconf(self):
        self.reducer.set_out(self.outer)
#        self.reducer.uniq("pay",2,2)
        x = event()
        for line in self.data:
            self.parser.parse(line,x)
            self.reducer.push(x)
            self.assertEqual(self.outer.e.name,"pay")
            self.assertEqual(self.outer.e.data,"""{"app":"app1","event":"pay","key":"qid:10001","data":{"money":50}}""")

    def test_pack2(self):
        outer = test_outer_pkg()
#        self.reducer.set_out(outer)
#        self.reducer.set_out(self.outer)
#        self.reducer.use_stdin()
        self.reducer.pack_ex("pay",4,4,outer)
        iner = test_in()
        iner.set_reducer(self.reducer)
        self.reducer.time_in(iner,1)
        self.reducer.work()
    def test_uniq(self):
        self.reducer.uniq_ex("pay",2,2,self.outer)
        x = event()
       
        for line in self.data:
            self.parser.parse(line,x)
            self.reducer.push(x)
        self.assertEqual(self.outer.e.name,"pay")
        self.assertEqual(self.outer.e.data,"""{"app":"app1","event":"pay","key":"qid:10001","data":{"money":50}}""")
        pass

class reducer_stub:
    def pack(self,event,pkg_size,wait_sec):
        print("pack")
        pass
    def uniq(self,event,buff_size,wait_sec):
        print("uniq")
        pass
class data_tc(unittest.TestCase):
    def test_loader(self):
        file_path   = os.environ['HOME'] + "/devspace/hydra/test/data/event_ok.yaml"
        econf       = econf_loader()
        econf.bind_args(file_path,reducer_stub())
        if econf.load():
            econf.using_conf()
            self.assertTrue(True)


        file_path   = os.environ['HOME'] + "/devspace/hydra/test/data/event_bad.yaml"
        econf       = econf_loader()
        econf.bind_args(file_path,reducer_stub())
        if econf.load():
            econf.using_conf()
            self.assertTrue(False)
        else:
            self.assertTrue(True)
