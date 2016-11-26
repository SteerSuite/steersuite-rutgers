#!/usr/bin/python
# Replace <USERNAME_DATABASE>, <USERNAME>, and <PASSWORD> below with your actual DB, user, and password.

import psycopg2
import sys

from steersuitedb.Scenario import Scenario
from steersuitedb.Algorithm import Algorithm
from steersuitedb.Benchmark import Benchmark

con = None

try:
     
    con = psycopg2.connect(database='steersuitedb', user='steeruser', password='steersuite') 
    con.set_isolation_level(0) 
    
    cur = con.cursor()    

    # get the proper algorithm id
    
    algorithm = Algorithm()
    algorithmIndex = algorithm.getAlgorithmIndex(cur, "pprAI")
    
    benchmark = Benchmark()
    benchmarkIndex = benchmark.getBenchmarkIndex(cur, "Composite01")
    
    scenario = Scenario(algorithmIndex, benchmarkIndex, "testing testing 123.")
#    ad.setAlgorithmReff("pprAI")
    scenario.insertScenario2(cur)
    print cur.fetchone()
    con.commit()


except psycopg2.DatabaseError, e:
    print 'Error pprAI-test: %s' % e    
    sys.exit(1)
    
    
finally:
    
    if con:
        con.close() 
    
    
