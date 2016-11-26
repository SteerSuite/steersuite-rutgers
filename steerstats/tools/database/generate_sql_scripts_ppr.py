
dir_path = "optim_param/"


ppr_parameters = """ped_max_speed,
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
ped_max_num_waypoints"""
ppr_parameters = ppr_parameters.translate(None,'\n')
print ppr_parameters
ppr_parameters_list = ppr_parameters.split(',')


for param in ppr_parameters_list:
        
    output_quiery_string = """select sum(test_status) as "scenarios_passed",
    sum((pp.test_status + 0.0 ))/count(*) as "c(Av)",
    1 - sum((pp.test_status + 0.0 ))/count(*) as "d(Av)",
    1 - sum(((1.0/300)/pp.num_agents)/(pp.average_time_per_call/pp.num_agents))/count(*) as "eff(Av)",
    1 - (SUM( CASE WHEN pp.test_status = 1 THEN pp.optimal_path_length / pp.total_distance_traveled ELSE 0 END)/count(*)) as "distance(Av)",
    1 - (SUM( CASE WHEN pp.test_status = 1 THEN pp.ple_energy_optimal / pp.ple_energy ELSE 0 END)/count(*)) as "ple(Av)",
    1 - (SUM( CASE WHEN pp.test_status = 1 THEN pp.optimal_time / pp.total_time_enabled ELSE 0 END)/count(*)) as "time(Av)",
    sum(CASE WHEN pp.test_status = 1 THEN 0 ELSE 1 END) as "scenarios failed",
    sum(CASE WHEN pp.test_status = 1 THEN 0 ELSE 1.0 END)/count(*) as "scenarios failed percent",
    count(*), 
    c.config_id, 
    pp.scenario_group, 
    -- pc.ped_adjustment_turn_rate,
    pc.""" + param + """,
    -- AVG(pp.total_ticks_accumulated/pp.frames) as "average_total_ticks for frames",
    AVG(pp.total_ticks_accumulated) as "average_total_ticks",
    AVG(pp.average_time_per_call) as "average_total_time_per_call",
    AVG(pp.total_time_of_all_calls) as "average_total_time_all_calls",
    AVG(pp.number_of_times_executed) as "average_total_times_called"
    -- AVG(pp.frames) as "average_number_of_frames"
from ppr_pa3_op_"""+ param+"""_table pp, scenario s, config c, ppr_ai_config pc
where 
    pp.scenario_group = s.scenario_id and 
    s.config_id = c.config_id and 
    c.config_id = pc.ppr_ai_config_id
    -- pp.scenario_id < 5000
    group by 
        pp.scenario_group, c.config_id, pc.""" + param + """
    order by 
        pc.""" + param + """;"""    
    # print output_quiery_string
    output_param_file = open(dir_path+param+".sql", "w")
    output_param_file.write(output_quiery_string)
    output_param_file.close()
        
        
