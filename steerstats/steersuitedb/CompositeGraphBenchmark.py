import psycopg2
from steersuitedb.Test import Test
from steersuitedb.Composite2Benchmark import Composite2Benchmark

class CompositeGraphBenchmark(Composite2Benchmark):
    """A simple example class"""
    _id_name = "benchmark_id"
    _table_name = "compositeGraph_benchmark_technique"
    _insert_order = """(
    benchmark_id,
    scenario_id  ,
    frames ,
    rand_calls  ,
    collisions  ,    
    time ,    
    effort ,    
    acceleration ,    
    score ,    
    success  ,    
    num_unique_collisions  ,    
    total_time_enabled ,    
    total_acceleration ,    
    total_distance_traveled ,    
    total_change_in_speed ,    
    total_degrees_turned ,    
    sum_total_of_instantaneous_acceleration ,    
    sum_total_of_instantaneous_kinetic_energies ,    
    average_kinetic_energy ,    
    agent_complete  ,    
    agent_success  ,    
    optimal_path_length  ,    
    length_ratio ,    
    optimal_time ,    
    time_ratio,
    agent_path_lengths,
    agent_time_enableds,
    agent_distance_traveleds,
    agent_completes,
    agent_successes,
    agent_intersections,
    ref_agent_intersections,
    box_obstacles,
    agent_goals,
    agent_initial_disks,
    agent_initial_forward_direction,
    agent_static_paths,
    agent_static_path_intersection_points,
    ref_agent_static_path_intersection_points
)""" 

#compositeGraph_benchmark_technique 
#(
#    benchmark_id integer NOT NULL references test(test_id),
#    scenario_id integer NOT NULL,
#    frames integer,
#    rand_calls integer NOT NULL,
#    collisions integer NOT NULL,    
#    time double precision NOT NULL,    
#    effort double precision NOT NULL,    
#    acceleration double precision NOT NULL,    
#    score double precision NOT NULL,    
#    success integer NOT NULL,    
#    num_unique_collisions integer NOT NULL,    
#    total_time_enabled double precision NOT NULL,    
#    total_acceleration double precision NOT NULL,    
#    total_distance_traveled double precision NOT NULL,    
#    total_change_in_speed double precision NOT NULL,    
#    total_degrees_turned double precision NOT NULL,    
#    sum_total_of_instantaneous_acceleration double precision NOT NULL,    
#    sum_total_of_instantaneous_kinetic_energies double precision NOT NULL,    
#    average_kinetic_energy double precision NOT NULL,    
#    agent_complete integer NOT NULL,    
#    agent_success integer NOT NULL,    
#    optimal_path_length integer NOT NULL,    
#    length_ratio double precision NOT NULL,    
#    optimal_time double precision NOT NULL,    
#    time_ratio double precision NOT NULL,
#    agent_intersections polygon,
#    ref_agent_intersections polygon,
#    box_obstacles polygon[]
#    agent_goals point[],
#    agent_initial_disks circle[],
#    agent_initial_forward_direction point[],
#    agent_static_paths path[]
#    agent_static_path_intersection_points polygon,
#    ref_agent_static_path_intersection_points polygon
#)

    def __init__(self, algorithm_data_id=0, test_comments="", benchmark_id=0, test_case="", test_status=0, scenario_group=0,    
                scenario_id=0,
                frames=0,
                rand_calls=0,
                collisions=0,    
                time=0,    
                effort=0,
                acceleration=0,    
                score=0,    
                success=0,    
                num_unique_collisions=0,    
                total_time_enabled=0,    
                total_acceleration=0,    
                total_distance_traveled=0,    
                total_change_in_speed=0,    
                total_degrees_turned=0,    
                sum_total_of_instantaneous_acceleration=0,    
                sum_total_of_instantaneous_kinetic_energies=0,    
                average_kinetic_energy=0,    
                agent_complete=0,    
                agent_success=0,    
                optimal_path_length=0,    
                length_ratio=0,    
                optimal_time=0,    
                time_ratio=0,
                agent_path_lengths="",
                agent_time_enableds="",
                agent_distance_traveleds="",
                agent_completes="",
                agent_successes="",
                agent_intersections="",
                ref_agent_intersections="",
                box_obstacles="",
                agent_goals="",
                agent_initial_disks="",
                agent_initial_forward_direction="",
                agent_static_paths="",
                agent_static_path_intersection_points="",
                ref_agent_static_path_intersection_points="" ):
        # call super constructor
        self._agent_intersections=agent_intersections
        self._ref_agent_intersections = ref_agent_intersections
        self._box_obstacles = box_obstacles
        self._agent_goals=agent_goals
        self._agent_initial_disks=agent_initial_disks
        self._agent_initial_forward_direction=agent_initial_forward_direction
        self._agent_static_paths=agent_static_paths
        self._agent_static_path_intersection_points=agent_static_path_intersection_points
        self._ref_agent_static_path_intersection_points=ref_agent_static_path_intersection_points
        super(CompositeGraphBenchmark,self).__init__(algorithm_data_id, test_comments, benchmark_id, test_case, test_status, scenario_group,    
                scenario_id,
                frames,
                rand_calls,
                collisions,    
                time,    
                effort,    
                acceleration,
                score,    
                success,    
                num_unique_collisions,    
                total_time_enabled,    
                total_acceleration,    
                total_distance_traveled,    
                total_change_in_speed,    
                total_degrees_turned,    
                sum_total_of_instantaneous_acceleration,    
                sum_total_of_instantaneous_kinetic_energies,    
                average_kinetic_energy,    
                agent_complete,    
                agent_success,    
                optimal_path_length,    
                length_ratio,    
                optimal_time,    
                time_ratio,
                agent_path_lengths,
                agent_time_enableds,
                agent_distance_traveleds,
                agent_completes,
                agent_successes)
    
    def getBenchmarkData(self, cur, n):
        cur.execute("SELECT * FROM " + self._table_name + " where " + self._id_name + " = "+ str(n))
        row = cur.fetchone()
        return row
    
    def setBenchmarkValuesFromDict(self, valuesDict):
        super(CompositeGraphBenchmark,self).setBenchmarkValuesFromDict(valuesDict)
        
        # "()" is the flag for no polygons
        
        if valuesDict['agent_intersections'] == "()":
            self._agent_intersections = None
        else:
            self._agent_intersections=valuesDict['agent_intersections']
        
        # checks to see if there are any points to insert
        if valuesDict['ref_agent_intersections'] == "()":
            self._ref_agent_intersections = None
        else:
            self._ref_agent_intersections=valuesDict['ref_agent_intersections']
            
        # checks to see if there are obstacles
        if valuesDict['box_obstacles'] == "null":
            self._box_obstacles = None
        else:
            # converts string to list wich psycopg2 will convert into ARRAY 
            obstacles=valuesDict['box_obstacles'].split(";")
            agent_goals_string = "{"
            for obstacle in obstacles:
                agent_goals_string = agent_goals_string + "\"" + str(obstacle) + "\","
                
            agent_goals_string = agent_goals_string[:-1] + "}"
            self._box_obstacles=agent_goals_string
            # self._box_obstacles[0] = self._box_obstacles[0]+"::polygon"
            # print str(self._box_obstacles)
            
        # checks to see if there are obstacles
        if valuesDict['agent_goals'] == "null":
            self._box_obstacles = None
        else:
            # converts string to list wich psycopg2 will convert into ARRAY 
            agent_goals=valuesDict['agent_goals'].split(";")
            agent_goals_string = "{"
            for goal in agent_goals:
                agent_goals_string = agent_goals_string + "\"" + str(goal) + "\","
                
            agent_goals_string = agent_goals_string[:-1] + "}" # remove last comma
            self._agent_goals=agent_goals_string
            # self._box_obstacles[0] = self._box_obstacles[0]+"::polygon"
            # print str(self._box_obstacles)
        
        # checks to see if there are obstacles
        if valuesDict['agent_initial_disks'] == "null":
            self._box_obstacles = None
        else:
            # converts string to list wich psycopg2 will convert into ARRAY 
            agent_goals=valuesDict['agent_initial_disks'].split(";")
            agent_initial_disks_string = "{"
            for goal in agent_goals:
                agent_initial_disks_string = agent_initial_disks_string + "\"" + str(goal) + "\","
                
            agent_initial_disks_string = agent_initial_disks_string[:-1] + "}" # remove last comma
            self._agent_initial_disks=agent_initial_disks_string
            # self._box_obstacles[0] = self._box_obstacles[0]+"::polygon"
            # print str(self._box_obstacles)
            
        # checks to see if there are obstacles
        if valuesDict['agent_initial_forward_direction'] == "null":
            self._box_obstacles = None
        else:
            # converts string to list wich psycopg2 will convert into ARRAY 
            agent_initial_forward_direction=valuesDict['agent_initial_forward_direction'].split(";")
            agent_initial_forward_direction_string = "{"
            for goal in agent_initial_forward_direction:
                agent_initial_forward_direction_string = agent_initial_forward_direction_string + "\"" + str(goal) + "\","
                
            agent_initial_forward_direction_string = agent_initial_forward_direction_string[:-1] + "}" # remove last comma
            self._agent_initial_forward_direction=agent_initial_forward_direction_string
            # self._box_obstacles[0] = self._box_obstacles[0]+"::polygon"
            # print str(self._box_obstacles)
            
        # checks to see if there are obstacles
        if valuesDict['agent_static_paths'] == "null":
            self._box_obstacles = None
        else:
            # converts string to list wich psycopg2 will convert into ARRAY 
            obstacles=valuesDict['agent_static_paths'].split(";")
            agent_goals_string = "{"
            for obstacle in obstacles:
                agent_goals_string = agent_goals_string + "\"" + str(obstacle) + "\","
                
            agent_goals_string = agent_goals_string[:-1] + "}"
            self._agent_static_paths=agent_goals_string
            # self._box_obstacles[0] = self._box_obstacles[0]+"::polygon"
            # print str(self._box_obstacles)
            
        if valuesDict['agent_static_path_intersection_points'] == "null":
            self._agent_static_path_intersection_points = None
        else:
            self._agent_static_path_intersection_points=valuesDict['agent_static_path_intersection_points']    

            
        if valuesDict['ref_agent_static_path_intersection_points'] == "null":
            self._ref_agent_static_path_intersection_points = None
        else:
            self._ref_agent_static_path_intersection_points=valuesDict['ref_agent_static_path_intersection_points']    


    def insertBenchmark(self, cur, 
                        scenario_id  ,
                        frames ,
                        rand_calls  ,
                        collisions  ,    
                        time ,    
                        effort ,    
                        acceleration ,    
                        score ,    
                        success  ,    
                        num_unique_collisions  ,    
                        total_time_enabled ,    
                        total_acceleration ,    
                        total_distance_traveled ,    
                        total_change_in_speed ,    
                        total_degrees_turned ,    
                        sum_total_of_instantaneous_acceleration ,    
                        sum_total_of_instantaneous_kinetic_energies ,    
                        average_kinetic_energy ,    
                        agent_complete  ,    
                        agent_success  ,    
                        optimal_path_length  ,    
                        length_ratio ,    
                        optimal_time ,    
                        time_ratio,
                        agent_path_lengths,
                        agent_time_enableds,
                        agent_distance_traveleds,
                        agent_completes,
                        agent_successes,
                        agent_intersections,
                        ref_agent_intersections,
                        box_obstacles,
                        agent_goals,
                        agent_initial_disks,
                        agent_initial_forward_direction,
                        agent_static_paths,
                        agent_static_path_intersection_points,
                        ref_agent_static_path_intersection_points ):
        try:
            
            testID = self.insertTest2(cur)
            benchmark_data_id = testID
            
            
            # print "Polygon: "  + str(agent_intersections)
            
            
            cur.execute("INSERT INTO " + self._table_name + " " +
                        self._insert_order + " " +
                        "VALUES(%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s," +
                         "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s," + 
                         "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", # 24
                        ( 
                            str(benchmark_data_id),
                            str(scenario_id  ),
                            frames ,
                            str(rand_calls  ),
                            str(collisions  ),    
                            str(time ),    
                            str(effort ),    
                            str(acceleration ),    
                            str(score ),    
                            str(success  ),    
                            str(num_unique_collisions  ),    
                            str(total_time_enabled ),    
                            str(total_acceleration ),    
                            str(total_distance_traveled ),    
                            str(total_change_in_speed ),    
                            str(total_degrees_turned ),    
                            str(sum_total_of_instantaneous_acceleration ),    
                            str(sum_total_of_instantaneous_kinetic_energies ),    
                            str(average_kinetic_energy ),    
                            str(agent_complete  ),    
                            str(agent_success  ),    
                            str(optimal_path_length  ),    
                            str(length_ratio ),    
                            str(optimal_time ),    
                            str(time_ratio),
                            agent_path_lengths,
                            agent_time_enableds,
                            agent_distance_traveleds,
                            agent_completes,
                            agent_successes,
                            agent_intersections,
                            ref_agent_intersections,
                            box_obstacles,
                            agent_goals,
                            agent_initial_disks,
                            agent_initial_forward_direction,
                            agent_static_paths,
                            agent_static_path_intersection_points,
                            ref_agent_static_path_intersection_points
                        )
                    )
            return benchmark_data_id
            
        except psycopg2.DatabaseError, e:
            print 'Error insertCompositeGraphBenchmark: %s' % e    
            
    def insertBenchmark2(self, cur):
        return self.insertBenchmark(cur, 
            self._scenario_id  ,
            self._frames ,
            self._rand_calls  ,
            self._collisions  ,
            self._time ,
            self._effort , 
            self._acceleration ,   
            self._score ,
            self._success  , 
            self._num_unique_collisions  ,    
            self._total_time_enabled ,
            self._total_acceleration ,
            self._total_distance_traveled ,
            self._total_change_in_speed ,
            self._total_degrees_turned ,
            self._sum_total_of_instantaneous_acceleration ,  
            self._sum_total_of_instantaneous_kinetic_energies ,
            self._average_kinetic_energy ,
            self._agent_complete  ,
            self._agent_success  ,
            self._optimal_path_length  ,
            self._length_ratio ,
            self._optimal_time ,
            self._time_ratio ,
            self.get_agent_path_lengths(),
            self.get_agent_time_enableds(),
            self.get_agent_distance_traveleds(), 
            self.get_agent_completes(),
            self.get_agent_successes(),
            self._agent_intersections,
            self._ref_agent_intersections,
            self._box_obstacles,
            self._agent_goals,
            self._agent_initial_disks,
            self._agent_initial_forward_direction,
            self._agent_static_paths,
            self._agent_static_path_intersection_points,
            self._ref_agent_static_path_intersection_points
            )

    def get_agent_intersections(self):
        return self.__agent_intersections


    def get_ref_agent_intersections(self):
        return self.__ref_agent_intersections


    def get_box_obstacles(self):
        return self.__box_obstacles


    def get_agent_goals(self):
        return self.__agent_goals


    def get_agent_initial_disks(self):
        return self.__agent_initial_disks


    def get_agent_initial_forward_direction(self):
        return self.__agent_initial_forward_direction


    def get_agent_static_paths(self):
        return self.__agent_static_paths


    def get_agent_static_path_intersection_points(self):
        return self.__agent_static_path_intersection_points


    def get_ref_agent_static_path_intersection_points(self):
        return self.__ref_agent_static_path_intersection_points


    def set_agent_intersections(self, value):
        self.__agent_intersections = value


    def set_ref_agent_intersections(self, value):
        self.__ref_agent_intersections = value


    def set_box_obstacles(self, value):
        self.__box_obstacles = value


    def set_agent_goals(self, value):
        self.__agent_goals = value


    def set_agent_initial_disks(self, value):
        self.__agent_initial_disks = value


    def set_agent_initial_forward_direction(self, value):
        self.__agent_initial_forward_direction = value


    def set_agent_static_paths(self, value):
        self.__agent_static_paths = value


    def set_agent_static_path_intersection_points(self, value):
        self.__agent_static_path_intersection_points = value


    def set_ref_agent_static_path_intersection_points(self, value):
        self.__ref_agent_static_path_intersection_points = value


    def del_agent_intersections(self):
        del self.__agent_intersections


    def del_ref_agent_intersections(self):
        del self.__ref_agent_intersections


    def del_box_obstacles(self):
        del self.__box_obstacles


    def del_agent_goals(self):
        del self.__agent_goals


    def del_agent_initial_disks(self):
        del self.__agent_initial_disks


    def del_agent_initial_forward_direction(self):
        del self.__agent_initial_forward_direction


    def del_agent_static_paths(self):
        del self.__agent_static_paths


    def del_agent_static_path_intersection_points(self):
        del self.__agent_static_path_intersection_points


    def del_ref_agent_static_path_intersection_points(self):
        del self.__ref_agent_static_path_intersection_points

    agent_intersections = property(get_agent_intersections, set_agent_intersections, del_agent_intersections, "agent_intersections's docstring")
    ref_agent_intersections = property(get_ref_agent_intersections, set_ref_agent_intersections, del_ref_agent_intersections, "ref_agent_intersections's docstring")
    box_obstacles = property(get_box_obstacles, set_box_obstacles, del_box_obstacles, "box_obstacles's docstring")
    agent_goals = property(get_agent_goals, set_agent_goals, del_agent_goals, "agent_goals's docstring")
    agent_initial_disks = property(get_agent_initial_disks, set_agent_initial_disks, del_agent_initial_disks, "agent_initial_disks's docstring")
    agent_initial_forward_direction = property(get_agent_initial_forward_direction, set_agent_initial_forward_direction, del_agent_initial_forward_direction, "agent_initial_forward_direction's docstring")
    agent_static_paths = property(get_agent_static_paths, set_agent_static_paths, del_agent_static_paths, "agent_static_paths's docstring")
    agent_static_path_intersection_points = property(get_agent_static_path_intersection_points, set_agent_static_path_intersection_points, del_agent_static_path_intersection_points, "agent_static_path_intersection_points's docstring")
    ref_agent_static_path_intersection_points = property(get_ref_agent_static_path_intersection_points, set_ref_agent_static_path_intersection_points, del_ref_agent_static_path_intersection_points, "ref_agent_static_path_intersection_points's docstring")
        
            
