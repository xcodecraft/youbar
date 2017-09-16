#!/usr/bin/python2.7
import  re , os , string ,  getopt ,sys , unittest,logging, json 
sys.path.append(os.environ['HOME'] + "/devspace/rocket/bin")
#sys.path.append(os.environ['HOME'] + "/devspace/rocket/src")
#sys.path.append("/home/q/tools/py_stone/")

from py_rocket import *
from parser_test import *
#from reducer_test import *
#from utls_test import *

if __name__ == '__main__':
#    level_log(0)
#    loglev= logging.ERROR
    loglev= logging.DEBUG
    logging.basicConfig(format="[ %(filename)s .  %(funcName)s .  %(lineno)d ] %(message)s",level=loglev)
    unittest.main()
