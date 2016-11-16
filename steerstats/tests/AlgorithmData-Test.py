#!/usr/bin/python
# Replace <USERNAME_DATABASE>, <USERNAME>, and <PASSWORD> below with your actual DB, user, and password.

import psycopg2
import sys

from steersuitedb import AlgorithmData
from steersuitedb import PprAI

con = None

try:
     
    con = psycopg2.connect(database='steersuitedb', user='steeruser', password='steersuite') 
    
    cur = con.cursor()    

    #for row in rows:
    #   print row
    _table_name = "algorithm"


    ad = AlgorithmData.AlgorithmData()
    ad.setAlgorithmReff("pprAI")
    next_val = ad.insertAlgorithmData2(cur)
    
    print "index is: " + str(next_val)
    
    con.commit()


except psycopg2.DatabaseError, e:
    print 'Error %s' % e    
    sys.exit(1)
    
    
finally:
    
    if con:
        con.close() 
    
    
