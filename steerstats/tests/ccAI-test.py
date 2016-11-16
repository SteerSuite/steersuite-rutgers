#!/usr/bin/python

import unittest
import psycopg2
import psycopg2.extras
import sys

sys.path.append("../")

from steersuitedb import AlgorithmData
from steersuitedb.CcAI import CcAI
from steersuitedb.ConnectionManager import ConnectionManager
from steersuite.LogParser import LogParser


con = None
ccID=0

class CcAI_Tester(unittest.TestCase):

    def test_Insert(self):
        try:
             
            conM = ConnectionManager()
            cur = conM.getCursor()    
        
        
            ccLogFile = open("data/ccAI.log", "r")
            
            lparser = LogParser()
            logData = lparser.parseLog(ccLogFile)
            
            # print logData[0]
            
            for dataLine in logData:
                # print dataLine
                ad = CcAI()
                ad.setValuesFromDict(dataLine)
                
                print ad
        
                ccID = ad.insertAlgorithmData2(cur)
        
            
        #    ad.setAlgorithmReff("pprAI")
            # ad.insertPprAIData2(cur)
            self.assertEqual( ad._compute_density_field_slowest_execution,
                               str(ad.getccAI(cur, ccID)._compute_density_field_slowest_execution) )
            
        
        
        except psycopg2.DatabaseError, e:
            print 'Error ccAI-test: %s' % e    
            sys.exit(1)
            
            
        finally:
            
            if con:
                con.close() 
            
            
