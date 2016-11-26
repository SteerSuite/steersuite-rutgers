import psycopg2
import psycopg2.extras
from steersuitedb.Util import getTime
from Sequence import TestSequence


# this is not completely encapsulated by another transaction so it should
# be used by the client when inserting data
class Test(object):
    """A simple example class"""
    __id_name = "test_id"
    __table_name = "test"
    __insert_order = "(test_id, algorithm_data_id, test_timestamp, test_comments, benchmark_type, test_case, test_status, scenario_group, num_agents, num_obstacles)"

#test 
#(
#  test_id integer NOT NULL primary key,
#  algorithm_data_id int NOT NULL references algorithm_data(algorithm_data_id),
#  test_timestamp timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
#  test_comments text NOT NULL,
#  benchmark_type int NOT NULL references benchmark(benchmark_id),
#  test_case text NOT NULL, 
#  test_status int NOT NULL,
#  scenario_group int Not NULL references scenario(scenario_group)
#)

    def __init__(self, algorithm_data_id=0, test_comments="", benchmark_type=0,
                  test_case="", test_status=0, scenario_group=0, num_agents=0, num_obstacles=0):
        self._algorithm_data_id = algorithm_data_id
        self._test_comments = test_comments
        self._benchmark_type = benchmark_type
        self._test_case = test_case
        self._test_status = test_status
        self._scenario_group = scenario_group
        self._num_agents = num_agents
        self._num_obstacles = num_obstacles

# Returns a Test object
    def getTestData(self, cur, n):
        cur.execute("SELECT * FROM " + self.__table_name + " where " + self.__id_name + " = %s", (n,))
        row = cur.fetchone()
        testy = Test(row['algorithm_data_id'], row['test_comments'], row['benchmark_type'], row['test_case'], row['test_status'], row['scenario_group'], row['num_agents'], row['num_obstacles'])
        return testy
    
    def getTestXML(self):
        return self._test_case
    
    def setBenchmarkValuesFromDict(self, valuesDict):
        self._algorithm_data_id=valuesDict['algorithm_data_id']
        self._test_comments=valuesDict['test_comments']
        self._benchmark_type=valuesDict['benchmark_type']
        self._test_case=valuesDict['test_case']
        self._test_status=valuesDict['test_status']
        self._scenario_group=valuesDict['scenario_group']
        self._num_agents=valuesDict['num_agents']
        self._num_obstacles=valuesDict['num_obstacles']
        
        
# will Return -1 if insert did not work
    def insertTest(self, cur, algorithm_data_id, test_comments, benchmark_type, test_case, test_status, scenario_group, num_agents, num_obstacles):
        try:
            testSeq = TestSequence()
            next_id = testSeq.getNextVal(cur)
            
            # print "obstacles: " + str(num_obstacles)
            # print "agents: " + str(num_agents)
            
            # get the current timestamp
            timestamp = getTime(cur)
            
            cur.execute("INSERT INTO " + self.__table_name + " " +
                        self.__insert_order + " " +
                        "VALUES(%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)" ,
                        ( 
                            str(next_id) ,
                            str(algorithm_data_id) ,
                            str(timestamp) ,
                            test_comments , 
                            str(benchmark_type),
                            test_case ,
                            str(test_status) ,
                            str(scenario_group),
                            num_agents,
                            num_obstacles
                        )
            
                    )
            return next_id
            
        except psycopg2.DatabaseError, e:
            print 'Error Test.py %s' % e    
            
    def insertTest2(self, cur):
        return self.insertTest(cur, self._algorithm_data_id, self._test_comments, self._benchmark_type, self._test_case, self._test_status, self._scenario_group,
                               self._num_agents, self._num_obstacles)

    def get_id_name(self):
        return self._id_name


    def get_table_name(self):
        return self._table_name


    def get_insert_order(self):
        return self._insert_order


    def get_algorithm_data_id(self):
        return self._algorithm_data_id


    def get_test_comments(self):
        return self._test_comments


    def get_benchmark_type(self):
        return self._benchmark_type


    def get_test_case(self):
        return self._test_case


    def get_test_status(self):
        return self._test_status


    def get_scenario_group(self):
        return self._scenario_group


    def get_num_agents(self):
        return self._num_agents


    def get_num_obstacles(self):
        return self._num_obstacles


    def set_id_name(self, value):
        self._id_name = value


    def set_table_name(self, value):
        self._table_name = value


    def set_insert_order(self, value):
        self._insert_order = value


    def set_algorithm_data_id(self, value):
        self._algorithm_data_id = value


    def set_test_comments(self, value):
        self._test_comments = value


    def set_benchmark_type(self, value):
        self._benchmark_type = value


    def set_test_case(self, value):
        self._test_case = value


    def set_test_status(self, value):
        self._test_status = value


    def set_scenario_group(self, value):
        self._scenario_group = value


    def set_num_agents(self, value):
        self._num_agents = value


    def set_num_obstacles(self, value):
        self._num_obstacles = value


    def del_id_name(self):
        del self._id_name


    def del_table_name(self):
        del self._table_name


    def del_insert_order(self):
        del self._insert_order


    def del_algorithm_data_id(self):
        del self._algorithm_data_id


    def del_test_comments(self):
        del self._test_comments


    def del_benchmark_type(self):
        del self._benchmark_type


    def del_test_case(self):
        del self._test_case


    def del_test_status(self):
        del self._test_status


    def del_scenario_group(self):
        del self._scenario_group


    def del_num_agents(self):
        del self._num_agents


    def del_num_obstacles(self):
        del self._num_obstacles

    id_name = property(get_id_name, set_id_name, del_id_name, "id_name's docstring")
    table_name = property(get_table_name, set_table_name, del_table_name, "table_name's docstring")
    insert_order = property(get_insert_order, set_insert_order, del_insert_order, "insert_order's docstring")
    algorithm_data_id = property(get_algorithm_data_id, set_algorithm_data_id, del_algorithm_data_id, "algorithm_data_id's docstring")
    test_comments = property(get_test_comments, set_test_comments, del_test_comments, "test_comments's docstring")
    benchmark_type = property(get_benchmark_type, set_benchmark_type, del_benchmark_type, "benchmark_type's docstring")
    test_case = property(get_test_case, set_test_case, del_test_case, "test_case's docstring")
    test_status = property(get_test_status, set_test_status, del_test_status, "test_status's docstring")
    scenario_group = property(get_scenario_group, set_scenario_group, del_scenario_group, "scenario_group's docstring")
    num_agents = property(get_num_agents, set_num_agents, del_num_agents, "num_agents's docstring")
    num_obstacles = property(get_num_obstacles, set_num_obstacles, del_num_obstacles, "num_obstacles's docstring")
            # sys.exit(1)          
            
