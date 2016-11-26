import psycopg2
from steersuitedb.AlgorithmData import AlgorithmData
from steersuitedb.BaseAI import BaseAI

class FootstepAI(BaseAI):
    """A simple example class"""
    _table_name = "footstep_ai_data"
    _algorithm_reff="footstepAI"
    _insert_order = """(
  footstep_ai_id ,
  planning_number_of_times_executed,
  planning_total_ticks_accumulated,
  planning_shortest_execution,
  planning_longest_execution,
  planning_tick_frequency,
  planning_fastest_execution,
  planning_slowest_execution,
  planning_average_time_per_call,
  planning_total_time_of_all_calls,
  
  number_of_times_executed,
  total_ticks_accumulated,
  shortest_execution,
  longest_execution,
  fastest_execution,
  slowest_execution,
  average_time_per_call,
  total_time_of_all_calls,
  tick_frequency,
)"""
# (first, last, age, address, city, state)

#footstep_ai (
#  footstep_ai_id int NOT NULL references algorithm_data(algorithm_data_id),
#  planning_number_of_times_executed int,
#  planning_total_ticks_accumulated int,
#  planning_shortest_execution int,
#  planning_longest_execution int,
#  planning_tick_frequency real,
#  planning_fastest_execution real,
#  planning_slowest_execution real,
#  planning_average_time_per_call real,
#  planning_total_time_of_all_calls real,
#  number_of_times_executed int,
#  total_ticks_accumulated int,
#  shortest_execution int,
#  longest_execution int,
#  tick_frequency real,
#  fastest_execution real,
#  slowest_execution real,
#  average_time_per_call real,
#  total_time_of_all_calls real
#) ;

    def __init__(self,
              planning_number_of_times_executed=0,
              planning_total_ticks_accumulated=0,
              planning_shortest_execution=0,
              planning_longest_execution=0,
              planning_tick_frequency=0,
              planning_fastest_execution=0,
              planning_slowest_execution=0,
              planning_average_time_per_call=0,
              planning_total_time_of_all_calls=0,
              number_of_times_executed=0,
                
                total_ticks_accumulated=0,
                shortest_execution=0,
                longest_execution=0,
                fastest_execution=0,
                slowest_execution=0,
                average_time_per_call=0,
                total_time_of_all_calls=0,
                tick_frequency=0):
        
        self._planning_number_of_times_executed=planning_number_of_times_executed
        self._planning_total_ticks_accumulated=planning_total_ticks_accumulated
        self._planning_shortest_execution=planning_shortest_execution
        self._planning_longest_execution=planning_longest_execution
        self._planning_tick_frequency=planning_tick_frequency
        self._planning_fastest_execution=planning_fastest_execution
        self._planning_slowest_execution=planning_slowest_execution
        self._planning_average_time_per_call=planning_average_time_per_call
        self._planning_total_time_of_all_calls=planning_total_time_of_all_calls
        
        super(FootstepAI,self).__init__(number_of_times_executed,
                  total_ticks_accumulated,
                  shortest_execution,
                  longest_execution,
                  fastest_execution,
                  slowest_execution,
                  average_time_per_call,
                  total_time_of_all_calls,
                  tick_frequency)
        
    def getFootstepAI(self, cur, n):
        cur.execute("SELECT * FROM " + self._table_name + " where benchmark_id = %s", (str(n),))
        row = cur.fetchone()
        return row
    
    # Used to set the values of a class from a list of values
# Must be a very well formed dict for the values to be assigned correctly.
    def setValuesFromDict(self, valuesDict):
        self._planning_number_of_times_executed=valuesDict['planning_number_of_times_executed']
        self._planning_total_ticks_accumulated=valuesDict['planning_total_ticks_accumulated']
        self._planning_shortest_execution=valuesDict['planning_shortest_execution']
        self._planning_longest_execution=valuesDict['planning_longest_execution']
        self._planning_tick_frequency=valuesDict['planning_tick_frequency']
        self._planning_fastest_execution=valuesDict['planning_fastest_execution']
        self._planning_slowest_execution=valuesDict['planning_slowest_execution']
        self._planning_average_time_per_call=valuesDict['planning_average_time_per_call']
        self._planning_total_time_of_all_calls=valuesDict['planning_total_time_of_all_calls']
        
        super(FootstepAI,self).setValuesFromDict(valuesDict)  
      

    def insertAlgorithmData(self, cur, planning_number_of_times_executed,
                            planning_total_ticks_accumulated,
                            planning_shortest_execution,
                            planning_longest_execution,
                            planning_tick_frequency,
                            planning_fastest_execution,
                            planning_slowest_execution,
                            planning_average_time_per_call,
                            planning_total_time_of_all_calls,
                            number_of_times_executed,
                            total_ticks_accumulated,
                            shortest_execution,
                            longest_execution,
                            fastest_execution,
                            slowest_execution,
                            average_time_per_call,
                            total_time_of_all_calls,
                            tick_frequency):
        try:
            # insert the algorithmData reference
            ad = AlgorithmData()
            ad.setAlgorithmReff(self._algorithm_reff)
            next_val = ad.insertAlgorithmData2(cur)
            
            cur.execute("INSERT INTO " + self._table_name + " " +
                        self._insert_order + " " +
                        "VALUES(%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", # 18
                        (  
                         next_val,
                          planning_number_of_times_executed,
                          planning_total_ticks_accumulated,
                          planning_shortest_execution,
                          planning_longest_execution,
                          planning_tick_frequency,
                          planning_fastest_execution,
                          planning_slowest_execution,
                          planning_average_time_per_call,
                          planning_total_time_of_all_calls,
                        str(number_of_times_executed),
                        str(total_ticks_accumulated),
                        str(shortest_execution),
                        str(longest_execution),
                        str(fastest_execution),
                        str(slowest_execution),
                        str(average_time_per_call),
                        str(total_time_of_all_calls),
                        str(tick_frequency)
                        ))
            return next_val
            
        except psycopg2.DatabaseError, e:
            print 'Error %s' % e    
            
    def insertAlgorithmData2(self, cur):
        return self.insertAlgorithmData(cur, 
        self._planning_number_of_times_executed,
        self._planning_total_ticks_accumulated,
        self._planning_shortest_execution,
        self._planning_longest_execution,
        self._planning_tick_frequency,
        self._planning_fastest_execution,
        self._planning_slowest_execution,
        self._planning_average_time_per_call,
        self._planning_total_time_of_all_calls,
        self.get_number_of_times_executed(),
        self.get_total_ticks_accumulated(),
        self.get_shortest_execution(),
        self.get_longest_execution(),
        self.get_fastest_execution(),
        self.get_slowest_execution(),
        self.get_average_time_per_call(),
        self.get_total_time_of_all_calls(),
        self.get_tick_frequency())
               
            
