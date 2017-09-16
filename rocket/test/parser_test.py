#coding=utf-8
import  re , os , string ,  sys , unittest,logging 
from py_rocket import *
class parser_tc(unittest.TestCase):
    def test_parse(self):
        p   = parser()
        if p.parse('{ "a" : 1 } ' ) :
            self.assertTrue( True)
            print( p.data())
        if not p.parse('{ "a"  1 } ' ) :
            self.assertTrue( True)

        if p.parse('{ "a" : "xxx" } ' ) :
            print(p.data()) 
            self.assertTrue( True)
        else:
            self.assertTrue( False)
        x=""" { "link": { "name": "中国", "type": "node", "value": { "url": { "name": "url", "type": "string", "value": "http://" }, "title": { "name": "biaoti", "type": "string", "value": "==title==" }, "color": { "name": "yansi", "type": "string", "value": "black" } } }, "item": { "name": "lanmu", "type": "node", "value": { "key": { "name": "KEY", "type": "string", "value": "" }, "val": { "name": "xxxx", "type": "string", "value": "" } } } } """ 
        if p.parse(x) :
            print(p.data())
        else:
            self.assertTrue( False)
