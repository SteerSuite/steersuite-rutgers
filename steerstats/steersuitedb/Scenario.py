import psycopg2
from Sequence import ScenarioSequence

class Scenario(object):
    """A simple example class"""
    _id_name = "scenario_id"
    _table_name = "scenario"
    _insert_order = """  
    (scenario_id ,
      algorithm_type ,
      benchmark_type ,
      config_id,
      scenario_description )"""
#scenario 
#(
#  scenario_id integer NOT NULL primary key,
#  algorithm_type integer NOT NULL references algorithm(algorithm_id),
#  benchmark_type integer NOT NULL references benchmark(benchmark_id),
#  config_id integer NOT NULL references config(config_id),
#  scenario_description tsvector
#) ;

    def __init__(self, algorithm_type=0, benchmark_type=0, config_id=0, scenario_description=""):
        self._id = id
        self._algorithm_type = algorithm_type
        self._benchmark_type = benchmark_type
        self._config_id = config_id
        self._scenario_description = scenario_description
    
    def getScenarioData(self, cur, n):
        cur.execute("SELECT * FROM " + self._table_name + " where " + self._id_name + " = "+ str(n))
        row = cur.fetchone()
        return row

    def insertScenario(self, cur, algorithm_type, benchmark_type, config_id, scenario_description):
        try:
            
            alDataSeq = ScenarioSequence()
            next_id = alDataSeq.getNextVal(cur)
            scenario_description=str(scenario_description.replace(',', ' '))
            # scenario_description=scenario_description[100:110]
            # print scenario_description
            cur.execute("INSERT INTO " + self._table_name + " " +
                            self._insert_order + " " +
                            "VALUES(%s,%s,%s,%s,%s)",
                            ( 
                                str(next_id),
                                str(algorithm_type),
                                str(benchmark_type),
                                str(config_id),
                                str(scenario_description),
                            )
                        )
            self._id = next_id
            return next_id
            
        except psycopg2.DatabaseError, e:
            print 'Scenario Error %s' % e    
            
    def insertScenario2(self, cur):
        return self.insertScenario(cur, self._algorithm_type, self._benchmark_type, self._config_id, self._scenario_description)
            # sys.exit(1)          
            
