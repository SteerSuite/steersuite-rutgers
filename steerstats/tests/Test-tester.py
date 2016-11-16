#!/usr/bin/python

import psycopg2
import sys
import time
from datetime import datetime

sys.path.append("../")
from steersuitedb.Test import Test
from steersuitedb.Util import getTime
from steersuitedb.Scenario import Scenario
from steersuitedb.Algorithm import Algorithm
from steersuitedb.Benchmark import Benchmark
from steersuitedb.ConnectionManager import ConnectionManager

con = None

try:
     
    conM = ConnectionManager()
    cur = conM.getCursor()     

    test_comments = "This is to test Test insertion"

    benchmark = Benchmark()
    benchmarkIndex = benchmark.getBenchmarkIndex(cur, "composite01")
    
    testcase = "blah blahbidy blah: " + str(datetime.now())
    
    test_status = 0
    
    scenario_group = 3
    
    test = Test(2, test_comments, benchmarkIndex, testcase, test_status, scenario_group)
#    ad.setAlgorithmReff("pprAI")
    test2 = test.getTestData(cur, test.insertTest2(cur))
    print test2._test_case


except psycopg2.DatabaseError, e:
    print 'Error pprAI-test: %s' % e    
    sys.exit(1)
    
    
finally:
    
    if con:
        con.close() 
    
    
