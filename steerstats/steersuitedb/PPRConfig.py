import psycopg2
from Sequence import ConfigSequence

class PPRConfig(object):
    _id_name = "ppr_ai_config_id"
    _table_name = "ppr_ai_config"
    _insert_order = """  
(ppr_ai_config_id,
ped_max_speed,
ped_typical_speed,
ped_max_force,
ped_max_speed_factor,
ped_faster_speed_factor,
ped_slightly_faster_speed_factor,
ped_typical_speed_factor,
ped_slightly_slower_speed_factor,
ped_slower_speed_factor,
ped_cornering_turn_rate,
ped_adjustment_turn_rate,
ped_faster_avoidance_turn_rate,
ped_typical_avoidance_turn_rate,
ped_braking_rate,
ped_comfort_zone,
ped_query_radius,
ped_similar_direction_dot_product_threshold,
ped_same_direction_dot_product_threshold,
ped_oncoming_prediction_threshold,
ped_oncoming_reaction_threshold,
ped_wrong_direction_dot_product_threshold,
ped_threat_distance_threshold,
ped_threat_min_time_threshold,
ped_threat_max_time_threshold,
ped_predictive_anticipation_factor,
ped_reactive_anticipation_factor,
ped_crowd_influence_factor,
ped_facing_static_object_threshold,
ped_ordinary_steering_strength,
ped_oncoming_threat_avoidance_strength,
ped_cross_threat_avoidance_strength,
ped_max_turning_rate,
ped_feeling_crowded_threshold,
ped_scoot_rate,
ped_reached_target_distance_threshold,
ped_dynamic_collision_padding,
ped_furthest_local_target_distance,
ped_next_waypoint_distance,
ped_max_num_waypoints
)"""

    """
    ppr_ai_config
(

ppr_ai_config_id integer NOT NULL primary key,
ped_max_speed    double precision not null,
ped_typical_speed  double precision not null,
ped_max_force   double precision not null,
ped_max_speed_factor   double precision not null,
ped_faster_speed_factor  double precision not null,
ped_slightly_faster_speed_factor double precision not null,
ped_typical_speed_factor    double precision not null,
ped_slightly_slower_speed_factor double precision not null,
ped_slower_speed_factor double precision not null,
ped_cornering_turn_rate double precision not null,
ped_adjustment_turn_rate double precision not null,
ped_faster_avoidance_turn_rate double precision not null,
ped_typical_avoidance_turn_rate double precision not null,
ped_braking_rate  double precision not null,
ped_comfort_zone    double precision not null,
ped_query_radius   double precision not null,
ped_similar_direction_dot_product_threshold double precision not null,
ped_same_direction_dot_product_threshold double precision not null,
ped_oncoming_prediction_threshold double precision not null,
ped_oncoming_reaction_threshold double precision not null,
ped_wrong_direction_dot_product_threshold double precision not null,
ped_threat_distance_threshold double precision not null,
ped_threat_min_time_threshold double precision not null,
ped_threat_max_time_threshold double precision not null,
ped_predictive_anticipation_factor  double precision not null,
ped_reactive_anticipation_factor double precision not null,
ped_crowd_influence_factor double precision not null,
ped_facing_static_object_threshold double precision not null,
ped_ordinary_steering_strength double precision not null,
ped_oncoming_threat_avoidance_strength double precision not null,
ped_cross_threat_avoidance_strength double precision not null,
ped_max_turning_rate double precision not null,
ped_feeling_crowded_threshold integer not null,
ped_scoot_rate  double precision not null,
ped_reached_target_distance_threshold  double precision not null,
ped_dynamic_collision_padding double precision not null,
ped_furthest_local_target_distance integer not null,
ped_next_waypoint_distance integer not null,
ped_max_num_waypoints integer not null

) ;"""

    # data should be a properly formed dictionary
    def __init__(self, data):
        self._data=data
    
    def getConfigData(self, cur, n):
        cur.execute("SELECT * FROM " + self._table_name + " where " + self._id_name + " = "+ str(n))
        row = cur.fetchone()
        return PPRConfig(row)
    
    def setValuesFromDict(self, valuesDict):
        self._data = valuesDict

    def insertConfig(self, cur, data):
        try:
            

            
            cur.execute("INSERT INTO " + self._table_name + " " +
                        self._insert_order + " " +
                        "VALUES(%s,%s,%s,%s,%s,%s,%s,%s,%s,%s" + # 10 per row
                            ",%s,%s,%s,%s,%s,%s,%s,%s,%s,%s" +
                            ",%s,%s,%s,%s,%s,%s,%s,%s,%s,%s" +
                            ",%s,%s,%s,%s,%s,%s,%s,%s,%s,%s" +
                        ")",
                        ( 
                            data['ppr_ai_config_id'],
                            data['ped_max_speed'],
                            data['ped_typical_speed'],
                            data['ped_max_force'],
                            data['ped_max_speed_factor'],
                            data['ped_faster_speed_factor'],
                            data['ped_slightly_faster_speed_factor'],
                            data['ped_typical_speed_factor'],
                            data['ped_slightly_slower_speed_factor'],
                            data['ped_slower_speed_factor'],
                            data['ped_cornering_turn_rate'],
                            data['ped_adjustment_turn_rate'],
                            data['ped_faster_avoidance_turn_rate'],
                            data['ped_typical_avoidance_turn_rate'],
                            data['ped_braking_rate'],
                            data['ped_comfort_zone'],
                            data['ped_query_radius'],
                            data['ped_similar_direction_dot_product_threshold'],
                            data['ped_same_direction_dot_product_threshold'],
                            data['ped_oncoming_prediction_threshold'],
                            data['ped_oncoming_reaction_threshold'],
                            data['ped_wrong_direction_dot_product_threshold'],
                            data['ped_threat_distance_threshold'],
                            data['ped_threat_min_time_threshold'],
                            data['ped_threat_max_time_threshold'],
                            data['ped_predictive_anticipation_factor'],
                            data['ped_reactive_anticipation_factor'],
                            data['ped_crowd_influence_factor'],
                            data['ped_facing_static_object_threshold'],
                            data['ped_ordinary_steering_strength'],
                            data['ped_oncoming_threat_avoidance_strength'],
                            data['ped_cross_threat_avoidance_strength'],
                            data['ped_max_turning_rate'],
                            data['ped_feeling_crowded_threshold'],
                            data['ped_scoot_rate'],
                            data['ped_reached_target_distance_threshold'],
                            data['ped_dynamic_collision_padding'],
                            data['ped_furthest_local_target_distance'],
                            data['ped_next_waypoint_distance'],
                            data['ped_max_num_waypoints']
                        )
                        )
            return data['ppr_ai_config_id']
            
        except psycopg2.DatabaseError, e:
            print 'Error PPRConfig: %s' % e    
            
    def insertConfig2(self, cur):
        return self.insertConfig(cur, self._data)
            # sys.exit(1)          
            
