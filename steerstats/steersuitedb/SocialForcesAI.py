import psycopg2
import copy
from steersuitedb.AlgorithmData import AlgorithmData
from steersuitedb.BaseAI import BaseAI

class SocialForcesAI(BaseAI):
    _id_name = "sf_ai_id"
    _algorithm_reff = "sfAI"
    _table_name = "socialforces_ai_data"
    _insert_order = """(sf_ai_id,
  number_of_times_executed ,
  total_ticks_accumulated ,
  shortest_execution ,
  longest_execution ,
  fastest_execution ,
  slowest_execution ,
  average_time_per_call ,
  total_time_of_all_calls,
  tick_frequency) 
    """
    
    
#socialforces_ai_data 
#(
#  sf_ai_id integer NOT NULL references algorithm_data(algorithm_data_id) ON DELETE CASCADE,
#  number_of_times_executed bigint,
#  total_ticks_accumulated bigint,
#  shortest_execution bigint,
#  longest_execution bigint,
#  tick_frequency double precision,
#  fastest_execution double precision,
#  slowest_execution double precision,
#  average_time_per_call double precision,
#  total_time_of_all_calls double precision,
#  CONSTRAINT sf_ai_pkey PRIMARY KEY (sf_ai_id)
#) ;

    def __init__(self,  number_of_times_executed=0,
                total_ticks_accumulated=0,
                shortest_execution=0,
                longest_execution=0,
                fastest_execution=0,
                slowest_execution=0,
                average_time_per_call=0,
                total_time_of_all_calls=0,
                tick_frequency=0):
        super(SocialForcesAI,self).__init__(number_of_times_executed,
                total_ticks_accumulated,
                shortest_execution,
                longest_execution,
                fastest_execution,
                slowest_execution,
                average_time_per_call,
                total_time_of_all_calls,
                tick_frequency)
        
        
        
    def getRVO2dAI(self, cur, n):
        cur.execute("SELECT * FROM " + self._table_name + " where " + self._id_name + " = "+ str(n))
        row = cur.fetchone()
        return SocialForcesAI(row)
    
    def setValuesFromDict(self, valuesDict):
        super(SocialForcesAI, self).setValuesFromDict(valuesDict)
        
    def insertAlgorithmData(self, cur, number_of_times_executed ,
                          total_ticks_accumulated ,
                          shortest_execution ,
                          longest_execution ,
                          fastest_execution ,
                          slowest_execution ,
                          average_time_per_call ,
                          total_time_of_all_calls,
                          tick_frequency):
        try:
            # insert the algorithmData reference
            ad = AlgorithmData()
            ad.setAlgorithmReff(self._algorithm_reff)
            next_val = ad.insertAlgorithmData2(cur)
            
            
            cur.execute("INSERT INTO " + self._table_name + " " +
                        self._insert_order + " " +
                        "VALUES(%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", # 18
                        (  
                         next_val,
                          number_of_times_executed ,
                          total_ticks_accumulated ,
                          shortest_execution ,
                          longest_execution ,
                          fastest_execution ,
                          slowest_execution ,
                          average_time_per_call ,
                          total_time_of_all_calls,
                          tick_frequency
                          ))
            return next_val
            
        except psycopg2.DatabaseError, e:
            print 'Error SocialForces Insert %s' % e  
        
        
    def insertAlgorithmData2(self, cur):
        return self.insertAlgorithmData(cur,self.get_number_of_times_executed(),
                                    self.get_total_ticks_accumulated(),
                                    self.get_shortest_execution(),
                                    self.get_longest_execution(),
                                    self.get_fastest_execution(),
                                    self.get_slowest_execution(),
                                    self.get_average_time_per_call(),
                                    self.get_total_time_of_all_calls(),
                                    self.get_tick_frequency())
        
    