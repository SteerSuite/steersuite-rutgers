#!/usr/bin/python
# Replace <USERNAME_DATABASE>, <USERNAME>, and <PASSWORD> below with your actual DB, user, and password.

import psycopg2
import sys

from steersuitedb import AlgorithmData
from steersuitedb.PprAI import PprAI
from steersuitedb.FootstepAI import FootstepAI
from steersuitedb.ConnectionManager import ConnectionManager

con = None

try:
    connectionManager = ConnectionManager()
    con = connectionManager.getConnection()
    con.set_isolation_level(0) 
    
    cur = con.cursor()    

    #for row in rows:
    #   print row


    fai = FootstepAI()
#    ad.setAlgorithmReff("pprAI")
    fai.insertFootstepAIData2(cur)
    print cur.fetchone()
    con.commit()


except psycopg2.DatabaseError, e:
    print 'Error footstepAI-test: %s' % e    
    sys.exit(1)
    
    
finally:
    
    if con:
        con.close() 
    
    
