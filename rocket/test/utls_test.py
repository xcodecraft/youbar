#coding=utf-8
import  re , os , string ,  sys , unittest,logging 
from py_reducer import *
class utls_tc(unittest.TestCase):
    def test_lru(self):
        l = lru(5,4)
        self.assertEqual(l.cache("1",1),False)
        self.assertEqual(l.cache("1",1),True)
        self.assertEqual(l.cache("2",1),False)
        self.assertEqual(l.cache("2",1),True)
        self.assertEqual(l.cache("3",1),False)
        self.assertEqual(l.cache("3",1),True)
        self.assertEqual(l.cache("1",1),True)
        self.assertEqual(l.cache("4",1),False)
        self.assertEqual(l.cache("4",1),True)
        l.time_expire()
        self.assertEqual(l.cache("5",1),False)
        self.assertEqual(l.cache("5",1),True)
        l.time_expire()
        l.time_expire()
        l.time_expire()
        l.time_expire()
        print(l.ratio())

    def t1est_lru2(self):
        l = lru(10,5)

        for  x in range(0,5) :
            for  i in range(0,20) :
                l.cache(str(i),1)
