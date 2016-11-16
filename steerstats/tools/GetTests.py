#!/usr/bin/python
# Replace <USERNAME_DATABASE>, <USERNAME>, and <PASSWORD> below with your actual DB, user, and password.

import psycopg2
import psycopg2.extras
import sys

# Append parent directory to access steersuite api.
sys.path.append("../")

test_ids_file_name = sys.argv[1]
save_dir = sys.argv[2]

from steersuitedb.Test import Test
from steersuitedb.Util import getTime
from steersuitedb.Scenario import Scenario
from steersuitedb.Algorithm import Algorithm
from steersuitedb.Benchmark import Benchmark
from steersuitedb.ConnectionManager import ConnectionManager
from steersuite.LogParser import LogParser

con = None

try:
     
    conM = ConnectionManager()
    con = conM.getConnection()
    test_id=544
    logparser = LogParser()
    
    test_ids_file = open(test_ids_file_name, "r")
    test_data = test_ids_file.read()
    test_ids_file.close()
  
    # print test_data  
    
    _test_ids = logparser.parsePostgres(test_data)
 
    # print _test_ids
    # gets a Dict like cursor so that columns can be referenced by name.
    cur = con.cursor(cursor_factory=psycopg2.extras.DictCursor)
    
    test = Test()
#    ad.setAlgorithmReff("pprAI")
    for test_id in _test_ids:
        result = test.getTestData(cur, test_id)
        # print result._test_case
        
        test_file = open(save_dir+"testcase-"+str(test_id)+".xml","w")
        test_file.write(result.getTestXML())
        # print result.getTestXML()
        test_file.close()


except psycopg2.DatabaseError, e:
    print "Error  getting test " + str(test_id) + ": %s" % e    
    sys.exit(1)
    
    
finally:
    
    if con:
        con.close() 
    
    
