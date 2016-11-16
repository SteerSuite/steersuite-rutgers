#!/usr/bin/python
# Replace <USERNAME_DATABASE>, <USERNAME>, and <PASSWORD> below with your actual DB, user, and password.

import psycopg2
import sys
import pickle

from steersuitedb import Algorithm
from steersuitedb import AlgorithmData

con = None

try:
     
    con = psycopg2.connect(database='steersuitedb', user='steeruser', password='steersuite') 
    
    cur = con.cursor()    

    #for row in rows:
    #   print row
    _table_name = "algorithm"




    cur.execute("SELECT * FROM " + _table_name)
    rows = cur.fetchall()
    for row in rows:
       print row


except psycopg2.DatabaseError, e:
    print 'Error %s' % e    
    sys.exit(1)
    
    
finally:
    
    if con:
        con.close() 
    
    
