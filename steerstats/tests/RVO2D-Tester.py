#!/usr/bin/python

import unittest
import psycopg2
import psycopg2.extras
import sys

sys.path.append("../")

from steersuitedb.RVO2DAI import RVO2DAI
from steersuitedb.ConnectionManager import ConnectionManager


con = None
ppr_config_id=4

# class PPRConfig_Tester(unittest.TestCase):
class PPRConfig_Tester():

    def test_Insert(self):
        try:
             
            conM = ConnectionManager()
            cur = conM.getCursor()    
            
            data = {}
            data['number_of_times_executed']=1
            n=0.7
            data['total_ticks_accumulated'] = n 
            n += 1
            data['shortest_execution'] = n 
            n += 1
            data['longest_execution'] = n 
            n += 1
            data['tick_frequency'] = n 
            n += 1
            data['fastest_execution'] = n 
            n += 1
            data['slowest_execution'] = n 
            n += 1
            data['average_time_per_call'] = n 
            n += 1
            data['total_time_of_all_calls'] = n 
            n += 1
            
            pprC = RVO2DAI(data)
            
            pprC.insertAlgorithmData2(cur)
            
            pprC2 = pprC.getRVO2dAI(cur, 3)._data
            
            print pprC2
            
            # if len(set(pprC2._data.keys()) - set(data.keys())) == 0 :
            #self.assertEqual(data['total_ticks_accumulated'],
            #                  pprC2._data['total_ticks_accumulated'])
            
            #print data
            
        
        except psycopg2.DatabaseError, e:
            print 'Error PPRConfig-test: %s' % e    
            sys.exit(1)
            
            
        finally:
            
            if con:
                con.close() 
            

ptest = PPRConfig_Tester()
ptest.test_Insert()         
