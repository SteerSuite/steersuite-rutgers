#!/usr/bin/python


import psycopg2
import sys

sys.path.append("../")

from steersuitedb import AlgorithmData
from steersuitedb.PprAI import PprAI
from steersuitedb.ConnectionManager import ConnectionManager
from steersuite.LogParser import LogParser


con = None

try:
     
    conM = ConnectionManager()
    con = conM.getConnection()
    
    cur = con.cursor()    


    pprLogFile = open("data/ppr.log", "r")
    
    lparser = LogParser()
    logData = lparser.parseLog(pprLogFile)
    
    # print logData[0]
    
    for dataLine in logData:
        # print dataLine
        ad = PprAI()
        ad.setpprAIValuesfromDict(dataLine)
        
        print ad
        print ad._longplan
        print ad._short_term_planning_total_ticks_accumulated
        ad.insertPprAIData2(cur)

    
#    ad.setAlgorithmReff("pprAI")
    # ad.insertPprAIData2(cur)
    con.commit()


except psycopg2.DatabaseError, e:
    print 'Error pprAI-test: %s' % e    
    sys.exit(1)
    
    
finally:
    
    if con:
        con.close() 
    
    
