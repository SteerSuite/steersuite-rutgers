import psycopg2
from steersuitedb.Test import Test

class Composite1Benchmark(Test):
    _id_name = "benchmark_id"
    _table_name = "composite01_benchmark_technique"
    _insert_order = """(
    benchmark_id,
    scenario_id  ,
    frames ,
    rand_calls  ,
    collisions  ,    
    time ,    
    effort ,    
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
    time_ratio ,
    agent_path_lengths,
    agent_time_enableds,
    agent_distance_traveleds,
    agent_completes,
    agent_successes
)""" 

#composite01_benchmark_technique 
#(
#    benchmark_id integer NOT NULL references test(test_id),
#    scenario_id integer NOT NULL,
#    rand_calls integer NOT NULL,
#    collisions integer NOT NULL,    
#    time double precision NOT NULL,    
#    effort double precision NOT NULL,    
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
#    time_ratio double precision NOT NULL
#)

    def __init__(self, algorithm_data_id=0, test_comments="", benchmark_id=0, test_case="", test_status=0, scenario_group=0,    
                scenario_id=0,
                frames =0,
                rand_calls=0,
                collisions=0,    
                time=0,    
                effort=0,    
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
                agent_successes="" ):
        
        super(Composite1Benchmark,self).__init__(algorithm_data_id, test_comments, benchmark_id, test_case, test_status, scenario_group)   

        self._scenario_id  = scenario_id
        self._frames = frames
        self._rand_calls  = rand_calls
        self._collisions  = collisions
        self._time = time
        self._effort = effort 
        self._score = score
        self._success  = success 
        self._num_unique_collisions  = num_unique_collisions    
        self._total_time_enabled = total_time_enabled
        self._total_acceleration = total_acceleration
        self._total_distance_traveled = total_distance_traveled
        self._total_change_in_speed = total_change_in_speed
        self._total_degrees_turned = total_degrees_turned
        self._sum_total_of_instantaneous_acceleration = sum_total_of_instantaneous_acceleration  
        self._sum_total_of_instantaneous_kinetic_energies = sum_total_of_instantaneous_kinetic_energies
        self._average_kinetic_energy = average_kinetic_energy
        self._agent_complete  = agent_complete
        self._agent_success  = agent_success
        self._optimal_path_length  = optimal_path_length
        self._length_ratio = length_ratio
        self._optimal_time = optimal_time
        self._time_ratio = time_ratio
        
        
        self._agent_path_lengths = agent_path_lengths
        self._agent_time_enableds = agent_time_enableds
        self._agent_distance_traveleds = agent_distance_traveleds
        self._agent_completes = agent_completes
        self._agent_successes = agent_successes
    
    def getBenchmarkData(self, cur, n):
        cur.execute("SELECT * FROM " + self._table_name + " where " + self._id_name + " = "+ str(n))
        row = cur.fetchone()
        tmpB = Composite1Benchmark()
        tmpB._setValuesFromDict(row)
        return tmpB
    
    
    def setBenchmarkValuesFromDict(self, valuesDict):
        super(Composite1Benchmark,self).setBenchmarkValuesFromDict(valuesDict)
        
        self._scenario_id=valuesDict['scenario_id']
        self._frames=valuesDict['frames']
        self._rand_calls=valuesDict['rand_calls']
        self._collisions=valuesDict['collisions']
        self._time=valuesDict['time']
        self._effort=valuesDict['effort'] 
        self._score=valuesDict['score']
        self._success=valuesDict['success'] 
        self._num_unique_collisions=valuesDict['num_unique_collisions']    
        self._total_time_enabled=valuesDict['total_time_enabled']
        self._total_acceleration=valuesDict['total_acceleration']
        self._total_distance_traveled=valuesDict['total_distance_traveled']
        self._total_change_in_speed=valuesDict['total_change_in_speed']
        self._total_degrees_turned=valuesDict['total_degrees_turned']
        self._sum_total_of_instantaneous_acceleration=valuesDict['sum_total_of_instantaneous_acceleration']  
        self._sum_total_of_instantaneous_kinetic_energies=valuesDict['sum_total_of_instantaneous_kinetic_energies']
        self._average_kinetic_energy=valuesDict['average_kinetic_energy']
        self._agent_complete=valuesDict['agent_complete']
        self._agent_success=valuesDict['agent_success']
        self._optimal_path_length=valuesDict['optimal_path_length']
        self._length_ratio=valuesDict['length_ratio']
        self._optimal_time=valuesDict['optimal_time']
        self._time_ratio=valuesDict['time_ratio']
        self._agent_path_lengths=valuesDict['agent_path_lengths']
        self._agent_time_enableds=valuesDict['agent_time_enableds']
        self._agent_distance_traveleds=valuesDict['agent_distance_traveleds']
        self._agent_completes=valuesDict['agent_completes']
        self._agent_successes=valuesDict['agent_successes']

    def insertBenchmark(self, cur, 
                        scenario_id  ,
                        frames ,
                        rand_calls  ,
                        collisions  ,    
                        time ,    
                        effort ,    
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
                        agent_successes ):
        try:
            
            testID = self.insertTest2(cur)
            benchmark_data_id = testID
            
            
            cur.execute("INSERT INTO " + self._table_name + " " +
                        self._insert_order + " " +
                        "VALUES(%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", # 24
                        ( 
                            str(benchmark_data_id),
                            str(scenario_id  ),
                            frames ,
                            str(rand_calls  ),
                            str(collisions  ),    
                            str(time ),    
                            str(effort ),    
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
                        )
                    )
            return benchmark_data_id
            
        except psycopg2.DatabaseError, e:
            print 'Error insertComposite1Benchmark%s' % e    
            
    def insertBenchmark2(self, cur):
        return self.insertBenchmark(cur, 
            self._scenario_id  ,
            self._frames ,
            self._rand_calls  ,
            self._collisions  ,
            self._time ,
            self._effort , 
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
            self.get_agent_successes()
            )
        
    def get_id_name(self):
        return self.__id_name


    def get_table_name(self):
        return self.__table_name


    def get_insert_order(self):
        return self.__insert_order


    def get_scenario_id(self):
        return self._scenario_id


    def get_frames(self):
        return self._frames


    def get_rand_calls(self):
        return self._rand_calls


    def get_collisions(self):
        return self._collisions


    def get_time(self):
        return self._time


    def get_effort(self):
        return self._effort


    def get_score(self):
        return self._score


    def get_success(self):
        return self._success


    def get_num_unique_collisions(self):
        return self._num_unique_collisions


    def get_total_time_enabled(self):
        return self._total_time_enabled


    def get_total_acceleration(self):
        return self._total_acceleration


    def get_total_distance_traveled(self):
        return self._total_distance_traveled


    def get_total_change_in_speed(self):
        return self._total_change_in_speed


    def get_total_degrees_turned(self):
        return self._total_degrees_turned


    def get_sum_total_of_instantaneous_acceleration(self):
        return self._sum_total_of_instantaneous_acceleration


    def get_sum_total_of_instantaneous_kinetic_energies(self):
        return self._sum_total_of_instantaneous_kinetic_energies


    def get_average_kinetic_energy(self):
        return self._average_kinetic_energy


    def get_agent_complete(self):
        return self._agent_complete


    def get_agent_success(self):
        return self._agent_success


    def get_optimal_path_length(self):
        return self._optimal_path_length


    def get_length_ratio(self):
        return self._length_ratio


    def get_optimal_time(self):
        return self._optimal_time


    def get_time_ratio(self):
        return self._time_ratio


    def set_id_name(self, value):
        self._id_name = value


    def set_table_name(self, value):
        self._table_name = value


    def set_insert_order(self, value):
        self._insert_order = value


    def set_scenario_id(self, value):
        self._scenario_id = value


    def set_frames(self, value):
        self._frames = value


    def set_rand_calls(self, value):
        self._rand_calls = value


    def set_collisions(self, value):
        self._collisions = value


    def set_time(self, value):
        self._time = value


    def set_effort(self, value):
        self._effort = value


    def set_score(self, value):
        self._score = value


    def set_success(self, value):
        self._success = value


    def set_num_unique_collisions(self, value):
        self._num_unique_collisions = value


    def set_total_time_enabled(self, value):
        self._total_time_enabled = value


    def set_total_acceleration(self, value):
        self._total_acceleration = value


    def set_total_distance_traveled(self, value):
        self._total_distance_traveled = value


    def set_total_change_in_speed(self, value):
        self._total_change_in_speed = value


    def set_total_degrees_turned(self, value):
        self._total_degrees_turned = value


    def set_sum_total_of_instantaneous_acceleration(self, value):
        self._sum_total_of_instantaneous_acceleration = value


    def set_sum_total_of_instantaneous_kinetic_energies(self, value):
        self._sum_total_of_instantaneous_kinetic_energies = value


    def set_average_kinetic_energy(self, value):
        self._average_kinetic_energy = value


    def set_agent_complete(self, value):
        self._agent_complete = value


    def set_agent_success(self, value):
        self._agent_success = value


    def set_optimal_path_length(self, value):
        self._optimal_path_length = value


    def set_length_ratio(self, value):
        self._length_ratio = value


    def set_optimal_time(self, value):
        self._optimal_time = value


    def set_time_ratio(self, value):
        self._time_ratio = value


    def del_id_name(self):
        del self._id_name


    def del_table_name(self):
        del self._table_name


    def del_insert_order(self):
        del self._insert_order


    def del_scenario_id(self):
        del self._scenario_id


    def del_frames(self):
        del self._frames


    def del_rand_calls(self):
        del self._rand_calls


    def del_collisions(self):
        del self._collisions


    def del_time(self):
        del self._time


    def del_effort(self):
        del self._effort


    def del_score(self):
        del self._score


    def del_success(self):
        del self._success


    def del_num_unique_collisions(self):
        del self._num_unique_collisions


    def del_total_time_enabled(self):
        del self._total_time_enabled


    def del_total_acceleration(self):
        del self._total_acceleration


    def del_total_distance_traveled(self):
        del self._total_distance_traveled


    def del_total_change_in_speed(self):
        del self._total_change_in_speed


    def del_total_degrees_turned(self):
        del self._total_degrees_turned


    def del_sum_total_of_instantaneous_acceleration(self):
        del self._sum_total_of_instantaneous_acceleration


    def del_sum_total_of_instantaneous_kinetic_energies(self):
        del self._sum_total_of_instantaneous_kinetic_energies


    def del_average_kinetic_energy(self):
        del self._average_kinetic_energy


    def del_agent_complete(self):
        del self._agent_complete


    def del_agent_success(self):
        del self._agent_success


    def del_optimal_path_length(self):
        del self._optimal_path_length


    def del_length_ratio(self):
        del self._length_ratio


    def del_optimal_time(self):
        del self._optimal_time


    def del_time_ratio(self):
        del self._time_ratio

    id_name = property(get_id_name, set_id_name, del_id_name, "id_name's docstring")
    table_name = property(get_table_name, set_table_name, del_table_name, "table_name's docstring")
    insert_order = property(get_insert_order, set_insert_order, del_insert_order, "insert_order's docstring")
    scenario_id = property(get_scenario_id, set_scenario_id, del_scenario_id, "scenario_id's docstring")
    frames = property(get_frames, set_frames, del_frames, "frames's docstring")
    rand_calls = property(get_rand_calls, set_rand_calls, del_rand_calls, "rand_calls's docstring")
    collisions = property(get_collisions, set_collisions, del_collisions, "collisions's docstring")
    time = property(get_time, set_time, del_time, "time's docstring")
    effort = property(get_effort, set_effort, del_effort, "effort's docstring")
    score = property(get_score, set_score, del_score, "score's docstring")
    success = property(get_success, set_success, del_success, "success's docstring")
    num_unique_collisions = property(get_num_unique_collisions, set_num_unique_collisions, del_num_unique_collisions, "num_unique_collisions's docstring")
    total_time_enabled = property(get_total_time_enabled, set_total_time_enabled, del_total_time_enabled, "total_time_enabled's docstring")
    total_acceleration = property(get_total_acceleration, set_total_acceleration, del_total_acceleration, "total_acceleration's docstring")
    total_distance_traveled = property(get_total_distance_traveled, set_total_distance_traveled, del_total_distance_traveled, "total_distance_traveled's docstring")
    total_change_in_speed = property(get_total_change_in_speed, set_total_change_in_speed, del_total_change_in_speed, "total_change_in_speed's docstring")
    total_degrees_turned = property(get_total_degrees_turned, set_total_degrees_turned, del_total_degrees_turned, "total_degrees_turned's docstring")
    sum_total_of_instantaneous_acceleration = property(get_sum_total_of_instantaneous_acceleration, set_sum_total_of_instantaneous_acceleration, del_sum_total_of_instantaneous_acceleration, "sum_total_of_instantaneous_acceleration's docstring")
    sum_total_of_instantaneous_kinetic_energies = property(get_sum_total_of_instantaneous_kinetic_energies, set_sum_total_of_instantaneous_kinetic_energies, del_sum_total_of_instantaneous_kinetic_energies, "sum_total_of_instantaneous_kinetic_energies's docstring")
    average_kinetic_energy = property(get_average_kinetic_energy, set_average_kinetic_energy, del_average_kinetic_energy, "average_kinetic_energy's docstring")
    agent_complete = property(get_agent_complete, set_agent_complete, del_agent_complete, "agent_complete's docstring")
    agent_success = property(get_agent_success, set_agent_success, del_agent_success, "agent_success's docstring")
    optimal_path_length = property(get_optimal_path_length, set_optimal_path_length, del_optimal_path_length, "optimal_path_length's docstring")
    length_ratio = property(get_length_ratio, set_length_ratio, del_length_ratio, "length_ratio's docstring")
    optimal_time = property(get_optimal_time, set_optimal_time, del_optimal_time, "optimal_time's docstring")
    time_ratio = property(get_time_ratio, set_time_ratio, del_time_ratio, "time_ratio's docstring")

    def get_agent_path_lengths(self):
        
        return [float(i) for i in self._agent_path_lengths.strip('()').split(',')]


    def set_agent_path_lengths(self, value):
        self._agent_path_lengths = value


    def del_agent_path_lengths(self):
        del self._agent_path_lengths

    agent_path_lengths = property(get_agent_path_lengths, set_agent_path_lengths, del_agent_path_lengths, "agent_path_lengths's docstring")

    def get_agent_time_enableds(self):
        return [float(i) for i in self._agent_time_enableds.strip('()').split(',')]


    def get_agent_distance_traveleds(self):
        return [float(i) for i in self._agent_distance_traveleds.strip('()').split(',')]


    def set_agent_time_enableds(self, value):
        self._agent_time_enableds = value


    def set_agent_distance_traveleds(self, value):
        self._agent_distance_traveleds = value


    def del_agent_time_enableds(self):
        del self._agent_time_enableds


    def del_agent_distance_traveleds(self):
        del self.__agent_distance_traveleds

    agent_time_enableds = property(get_agent_time_enableds, set_agent_time_enableds, del_agent_time_enableds, "agent_time_enableds's docstring")
    agent_distance_traveleds = property(get_agent_distance_traveleds, set_agent_distance_traveleds, del_agent_distance_traveleds, "agent_distance_traveleds's docstring")

    def get_agent_completes(self):
        return [float(i) for i in self._agent_completes.strip('()').split(',')]


    def get_agent_successes(self):
        return [float(i) for i in self._agent_successes.strip('()').split(',')]


    def set_agent_completes(self, value):
        self._agent_completes = value

    def set_agent_successes(self, value):
        self._agent_successes = value


    def del_agent_completes(self):
        del self._agent_completes


    def del_agent_successes(self):
        del self._agent_successes

    agent_completes = property(get_agent_completes, set_agent_completes, del_agent_completes, "agent_completes's docstring")
    agent_successes = property(get_agent_successes, set_agent_successes, del_agent_successes, "agent_successes's docstring")
            
