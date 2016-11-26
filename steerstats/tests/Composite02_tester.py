#!/usr/bin/python
# Replace <USERNAME_DATABASE>, <USERNAME>, and <PASSWORD> below with your actual DB, user, and password.

import psycopg2
import sys

sys.path.append("../")

from steersuitedb.Test import Test
from steersuitedb.Util import getTime
from steersuitedb.Scenario import Scenario
from steersuitedb.Algorithm import Algorithm
from steersuitedb.Benchmark import Benchmark
from steersuitedb.ConnectionManager import ConnectionManager

from steersuite import LogParser
from steersuitedb.Composite2Benchmark import Composite2Benchmark
# import steersuitedb.Composite1Benchmark

con = None


try:
     
    conM = ConnectionManager()
    con = conM.getConnection() 
    
    cur = con.cursor()    

    test_comments = "This is to test Test insertion"

    benchmark = Benchmark()
    benchmarkIndex = benchmark.getBenchmarkIndex(cur, "Composite02")
    
    algorithm = Algorithm()
    algorithmIndex = algorithm.getAlgorithmIndex(cur, "pprAI")
    
    testcase = "blah blahbidy blah"
    
    test_status = 0
    
    scenario_group = 3

    # test = Test(22, test_comments, benchmarkIndex, testcase, test_status, scenario_group)
    
    benchmark = Composite2Benchmark()
    
   # benchmark.setValuesFromDict(valuesDict)
    
    benchmark.insertComposite2Benchmark2(cur)
    
    print cur.fetchone()
    con.commit()


except psycopg2.DatabaseError, e:
    print 'Error pprAI-test: %s' % e    
    sys.exit(1)
    
    
finally:
    
    if con:
        con.close() 
    
    
