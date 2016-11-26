import psycopg2
from steersuitedb.Test import Test
from steersuitedb.Composite2Benchmark import Composite2Benchmark
class CompositePLEBenchmark(Composite2Benchmark):
    """A simple example class"""
    _id_name = "benchmark_id"
    _table_name = "compositeple_benchmark_technique"
    # Ensures proper ordering when inserting data
    _insert_order = """(
    benchmark_id,
    scenario_id  ,
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
    ple_energy_dummy,
    ple_energy,
    ple_energy_optimal,
    ple_energy_ratio,
    agent_ple_energys
)""" 

#composite02_benchmark_technique 
#(
#    benchmark_id integer NOT NULL references test(test_id),
#    scenario_id integer NOT NULL,
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
#    time_ratio double precision NOT NULL
#    ple_energy_dummy double precision NOT NULL,
#    ple_energy double precision NOT NULL,
#    ple_energy_optimal double precision NOT NULL,
#    ple_energy_ratio double precision NOT NULL,
#)

    def __init__(self, algorithm_data_id=0, test_comments="", benchmark_id=0, test_case="", test_status=0, scenario_group=0,    
                scenario_id=0,
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
                ple_energy=0,
                ple_energy_optimal=0,
                ple_energy_dummy=0,
                ple_energy_ratio=0,
                agent_ple_energys=""):

        self._ple_energy_dummy=ple_energy_dummy
        self._ple_energy=ple_energy
        self._ple_energy_optimal=ple_energy_optimal
        self._ple_energy_ratio=ple_energy_ratio
        self._agent_ple_energys=agent_ple_energys
        # call super constructor
        super(CompositePLEBenchmark,self).__init__(algorithm_data_id, test_comments, benchmark_id, test_case, test_status, scenario_group,    
                scenario_id,
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
        super(CompositePLEBenchmark,self).setBenchmarkValuesFromDict(valuesDict)
        
        self._ple_energy_dummy=float(valuesDict['ple_energy_dummy'])
        self._ple_energy=float(valuesDict['ple_energy'])
        self._ple_energy_optimal=float(valuesDict['ple_energy_optimal'])
        self._ple_energy_ratio=float(valuesDict['ple_energy_ratio'])
        self._agent_ple_energys=valuesDict['agent_ple_energys']
    
    def insertBenchmark(self, cur, 
                        scenario_id  ,
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
                        time_ratio ,
                        agent_path_lengths,
                        agent_time_enableds,
                        agent_distance_traveleds,
                        agent_completes,
                        agent_successes,
                        ple_energy_dummy ,
                        ple_energy ,
                        ple_energy_optimal ,
                        ple_energy_ratio,
                        agent_ple_energys):
    
        try:
            
            testID = self.insertTest2(cur)
            benchmark_data_id = testID
            
            #print ple_energy_dummy
            
            cur.execute("INSERT INTO " + self._table_name + " " +
                        self._insert_order + " " +
                        "VALUES(%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s" +
                        ",%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s" + 
                        ",%s,%s,%s,%s)", # 28
                        ( 
                            str(benchmark_data_id),
                            str(scenario_id  ),
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
                            str(ple_energy_dummy),
                            str(ple_energy),
                            str(ple_energy_optimal),
                            str(ple_energy_ratio),
                            agent_ple_energys,
                        )
                    )
            return benchmark_data_id
            
        except psycopg2.DatabaseError, e:
            print 'Error insertCompositePLEBenchmark %s' % e    
            
    def insertBenchmark2(self, cur):
        return self.insertBenchmark(cur, 
            self._scenario_id  ,
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
            self._time_ratio,
            self.get_agent_path_lengths(),
            self.get_agent_time_enableds(),
            self.get_agent_distance_traveleds(), 
            self.get_agent_completes(),
            self.get_agent_successes(),
            self._ple_energy_dummy,
            self._ple_energy,
            self._ple_energy_optimal,
            self._ple_energy_ratio,
            self.get_agent_ple_energys() )

    def get_ple_energy_dummy(self):
        return self._ple_energy_dummy


    def get_ple_energy(self):
        return self._ple_energy


    def get_ple_energy_optimal(self):
        return self._ple_energy_optimal


    def get_ple_energy_ratio(self):
        return self._ple_energy_ratio


    def set_ple_energy_dummy(self, value):
        self._ple_energy_dummy = value


    def set_ple_energy(self, value):
        self._ple_energy = value


    def set_ple_energy_optimal(self, value):
        self._ple_energy_optimal = value


    def set_ple_energy_ratio(self, value):
        self._ple_energy_ratio = value


    def del_ple_energy_dummy(self):
        del self._ple_energy_dummy


    def del_ple_energy(self):
        del self._ple_energy


    def del_ple_energy_optimal(self):
        del self._ple_energy_optimal


    def del_ple_energy_ratio(self):
        del self._ple_energy_ratio

    ple_energy_dummy = property(get_ple_energy_dummy, set_ple_energy_dummy, del_ple_energy_dummy, "ple_energy_dummy's docstring")
    ple_energy = property(get_ple_energy, set_ple_energy, del_ple_energy, "ple_energy's docstring")
    ple_energy_optimal = property(get_ple_energy_optimal, set_ple_energy_optimal, del_ple_energy_optimal, "ple_energy_optimal's docstring")
    ple_energy_ratio = property(get_ple_energy_ratio, set_ple_energy_ratio, del_ple_energy_ratio, "ple_energy_ratio's docstring")

    def get_agent_ple_energys(self):
        return [float(i) for i in self._agent_ple_energys.strip('()').split(',')]


    def set_agent_ple_energys(self, value):
        self._agent_ple_energys = value


    def del_agent_ple_energys(self):
        del self._agent_ple_energys

    agent_ple_energys = property(get_agent_ple_energys, set_agent_ple_energys, del_agent_ple_energys, "agent_ple_energys's docstring")
        
            
