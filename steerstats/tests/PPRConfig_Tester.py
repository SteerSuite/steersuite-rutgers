#!/usr/bin/python

import unittest
import psycopg2
import psycopg2.extras
import sys

sys.path.append("../")

from steersuitedb.PPRConfig import PPRConfig
from steersuitedb.ConnectionManager import ConnectionManager


con = None
ppr_config_id=4

class PPRConfig_Tester(unittest.TestCase):
# class PPRConfig_Tester():

    def test_Insert(self):
        try:
             
            conM = ConnectionManager()
            cur = conM.getCursor()    
            
            data = {}
            data['ppr_ai_config_id']=1
            n=0.7
            data['ped_max_speed'] = n
            n += 1
            data['ped_typical_speed'] = n
            n += 1
            data['ped_max_force'] = n
            n += 1
            data['ped_max_speed_factor'] = n
            n += 1
            data['ped_faster_speed_factor'] = n
            n += 1
            data['ped_slightly_faster_speed_factor'] = n
            n += 1
            data['ped_typical_speed_factor'] = n
            n += 1
            data['ped_slightly_slower_speed_factor'] = n
            n += 1
            data['ped_slower_speed_factor'] = n
            n += 1
            data['ped_cornering_turn_rate'] = n
            n += 1
            data['ped_adjustment_turn_rate'] = n
            n += 1
            data['ped_faster_avoidance_turn_rate'] = n
            n += 1
            data['ped_typical_avoidance_turn_rate'] = n
            n += 1
            data['ped_braking_rate'] = n
            n += 1
            data['ped_comfort_zone'] = n
            n += 1
            data['ped_query_radius'] = n
            n += 1
            data['ped_similar_direction_dot_product_threshold'] = n
            n += 1
            data['ped_same_direction_dot_product_threshold'] = n
            n += 1
            data['ped_oncoming_prediction_threshold'] = n
            n += 1
            data['ped_oncoming_reaction_threshold'] = n
            n += 1
            data['ped_wrong_direction_dot_product_threshold'] = n
            n += 1
            data['ped_threat_distance_threshold'] = n
            n += 1
            data['ped_threat_min_time_threshold'] = n
            n += 1
            data['ped_threat_max_time_threshold'] = n
            n += 1
            data['ped_predictive_anticipation_factor'] = n
            n += 1
            data['ped_reactive_anticipation_factor'] = n
            n += 1
            data['ped_crowd_influence_factor'] = n
            n += 1
            data['ped_facing_static_object_threshold'] = n
            n += 1
            data['ped_ordinary_steering_strength'] = n
            n += 1
            data['ped_oncoming_threat_avoidance_strength'] = n
            n += 1
            data['ped_cross_threat_avoidance_strength'] = n
            n += 1
            data['ped_max_turning_rate'] = n
            n += 1
            data['ped_feeling_crowded_threshold'] = n
            n += 1
            data['ped_scoot_rate'] = n
            n += 1
            data['ped_reached_target_distance_threshold'] = n
            n += 1
            data['ped_dynamic_collision_padding'] = n
            n += 1
            data['ped_furthest_local_target_distance'] = n
            n += 1
            data['ped_next_waypoint_distance'] = n
            n += 1
            data['ped_max_num_waypoints'] = n
            
            pprC = PPRConfig(data)
            
            pprC.insertConfig2(cur)
            
            pprC2 = pprC.getConfigData(cur, 1)
            
            # if len(set(pprC2._data.keys()) - set(data.keys())) == 0 :
            self.assertEqual(data['ped_braking_rate'], pprC2._data['ped_braking_rate'])
            
            print data
            
        
        except psycopg2.DatabaseError, e:
            print 'Error PPRConfig-test: %s' % e    
            sys.exit(1)
            
            
        finally:
            
            if con:
                con.close() 
            

# ptest = PPRConfig_Tester()
# ptest.test_Insert()         
