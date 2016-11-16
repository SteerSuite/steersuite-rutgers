#!/usr/bin/python

import unittest
import psycopg2
import psycopg2.extras
import sys

sys.path.append("../")

from steersuitedb.RVO2DConfig import RVO2DConfig
from steersuitedb.ConnectionManager import ConnectionManager


con = None
ppr_config_id=4

# class PPRConfig_Tester(unittest.TestCase):
class RVO2DConfig_Tester():

    def test_Insert(self):
        try:
             
            conM = ConnectionManager()
            cur = conM.getCursor()    
            
            data = {}
            n = 4
            data['rvo2d_ai_config_id']=1
            data['rvo_neighbor_distance'] = n 
            n += 1
            data['rvo_time_horizon'] = n 
            n += 1
            data['rvo_max_speed'] = n 
            n += 1
            data['rvo_time_horizon_obstacles'] = n 
            n += 1
            data['rvo_max_neighbors'] = n 
            n += 1
            
            pprC = RVO2DConfig(data)
            
            pprC.insertConfig2(cur)
            
            pprC2 = pprC.getConfigData(cur, 3)._data
            
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
            

ptest = RVO2DConfig_Tester()
ptest.test_Insert()         
