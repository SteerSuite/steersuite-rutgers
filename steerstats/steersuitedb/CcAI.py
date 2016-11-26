import psycopg2
from steersuitedb.AlgorithmData import AlgorithmData
from steersuitedb.BaseAI import BaseAI

class CcAI(BaseAI):
    """A simple example class"""
    _id_name = "cc_ai_id"
    _algorithm_reff = "ccAI"
    _table_name = "cc_ai_data"
    _insert_order = """(
  cc_ai_id, 
  compute_density_field_number_of_times_executed,
  compute_density_field_total_ticks_accumulated,
  compute_density_field_shortest_execution,
  compute_density_field_longest_execution,
  compute_density_field_tick_frequency,
  compute_density_field_fastest_execution,
  compute_density_field_slowest_execution,
  compute_density_field_average_time_per_call,
  compute_density_field_total_time_of_all_calls,
  
  create_grouping_number_of_times_executed,
  create_grouping_total_ticks_accumulated,
  create_grouping_shortest_execution,
  create_grouping_longest_execution,
  create_grouping_tick_frequency,
  create_grouping_fastest_execution,
  create_grouping_slowest_execution,
  create_grouping_average_time_per_call,
  create_grouping_total_time_of_all_calls,
  
  draw_number_of_times_executed,
  draw_total_ticks_accumulated,
  draw_shortest_execution,
  draw_longest_execution,
  draw_tick_frequency,
  draw_fastest_execution,
  draw_slowest_execution,
  draw_average_time_per_call,
  draw_total_time_of_all_calls,
  
  potential_field_construction_number_of_times_executed,
  potential_field_construction_total_ticks_accumulated,
  potential_field_construction_shortest_execution,
  potential_field_construction_longest_execution,
  potential_field_construction_tick_frequency,
  potential_field_construction_fastest_execution,
  potential_field_construction_slowest_execution,
  potential_field_construction_average_time_per_call,
  potential_field_construction_total_time_of_all_calls,
  
  number_of_times_executed ,
  total_ticks_accumulated ,
  shortest_execution ,
  longest_execution ,
  fastest_execution ,
  slowest_execution ,
  average_time_per_call ,
  total_time_of_all_calls,
  tick_frequency
    )"""

# table description
    """cc_ai_data 
(
  cc_ai_id integer NOT NULL references algorithm_data(algorithm_data_id) ON DELETE CASCADE,
  compute_density_field_number_of_times_executed integer,
  compute_density_field_total_ticks_accumulated bigint,
  compute_density_field_shortest_execution integer,
  compute_density_field_longest_execution integer,
  compute_density_field_tick_frequency double precision,
  compute_density_field_fastest_execution double precision,
  compute_density_field_slowest_execution double precision,
  compute_density_field_average_time_per_call double precision,
  compute_density_field_total_time_of_all_calls double precision,
  
  create_grouping_number_of_times_executed integer,
  create_grouping_total_ticks_accumulated bigint,
  create_grouping_shortest_execution integer,
  create_grouping_longest_execution integer,
  create_grouping_tick_frequency double precision,
  create_grouping_fastest_execution double precision,
  create_grouping_slowest_execution double precision,
  create_grouping_average_time_per_call double precision,
  create_grouping_total_time_of_all_calls double precision,
  
  draw_number_of_times_executed integer,
  draw_total_ticks_accumulated bigint,
  draw_shortest_execution integer,
  draw_longest_execution integer,
  draw_tick_frequency double precision,
  draw_fastest_execution double precision,
  draw_slowest_execution double precision,
  draw_average_time_per_call double precision,
  draw_total_time_of_all_calls double precision,
  
  potential_field_construction_number_of_times_executed integer,
  potential_field_construction_total_ticks_accumulated bigint,
  potential_field_construction_shortest_execution integer,
  potential_field_construction_longest_execution integer,
  potential_field_construction_tick_frequency double precision,
  potential_field_construction_fastest_execution double precision,
  potential_field_construction_slowest_execution double precision,
  potential_field_construction_average_time_per_call double precision,
  potential_field_construction_total_time_of_all_calls double precision,
  
  number_of_times_executed integer,
  total_ticks_accumulated bigint,
  shortest_execution integer,
  longest_execution integer,
  tick_frequency double precision,
  fastest_execution double precision,
  slowest_execution double precision,
  average_time_per_call double precision,
  total_time_of_all_calls double precision,
  CONSTRAINT cc_ai_pkey PRIMARY KEY (cc_ai_id)
) TABLESPACE steer_store;"""

    def __init__(self, 
                    compute_density_field_number_of_times_executed=0,
                    compute_density_field_total_ticks_accumulated=0,
                    compute_density_field_shortest_execution=0,
                    compute_density_field_longest_execution=0,
                    compute_density_field_tick_frequency=0,
                    compute_density_field_fastest_execution=0,
                    compute_density_field_slowest_execution=0,
                    compute_density_field_average_time_per_call=0,
                    compute_density_field_total_time_of_all_calls=0,
                    
                    create_grouping_number_of_times_executed=0,
                    create_grouping_total_ticks_accumulated=0,
                    create_grouping_shortest_execution=0,
                    create_grouping_longest_execution=0,
                    create_grouping_tick_frequency=0,
                    create_grouping_fastest_execution=0,
                    create_grouping_slowest_execution=0,
                    create_grouping_average_time_per_call=0,
                    create_grouping_total_time_of_all_calls=0,
                    
                    draw_number_of_times_executed=0,
                    draw_total_ticks_accumulated=0,
                    draw_shortest_execution=0,
                    draw_longest_execution=0,
                    draw_tick_frequency=0,
                    draw_fastest_execution=0,
                    draw_slowest_execution=0,
                    draw_average_time_per_call=0,
                    draw_total_time_of_all_calls=0,
                    
                    potential_field_construction_number_of_times_executed=0,
                    potential_field_construction_total_ticks_accumulated=0,
                    potential_field_construction_shortest_execution=0,
                    potential_field_construction_longest_execution=0,
                    potential_field_construction_tick_frequency=0,
                    potential_field_construction_fastest_execution=0,
                    potential_field_construction_slowest_execution=0,
                    potential_field_construction_average_time_per_call=0,
                    potential_field_construction_total_time_of_all_calls=0,
                    
                    number_of_times_executed=0,
                    total_ticks_accumulated=0,
                    shortest_execution=0,
                    longest_execution=0,
                    fastest_execution=0,
                    slowest_execution=0,
                    average_time_per_call=0,
                    total_time_of_all_calls=0,
                    tick_frequency=0):

        self._compute_density_field_number_of_times_executed=compute_density_field_number_of_times_executed
        self._compute_density_field_total_ticks_accumulated=compute_density_field_total_ticks_accumulated
        self._compute_density_field_shortest_execution=compute_density_field_shortest_execution
        self._compute_density_field_longest_execution=compute_density_field_longest_execution
        self._compute_density_field_tick_frequency=compute_density_field_tick_frequency
        self._compute_density_field_fastest_execution=compute_density_field_fastest_execution
        self._compute_density_field_slowest_execution=compute_density_field_slowest_execution
        self._compute_density_field_average_time_per_call=compute_density_field_average_time_per_call
        self._compute_density_field_total_time_of_all_calls=compute_density_field_total_time_of_all_calls
        
        self._create_grouping_number_of_times_executed=create_grouping_number_of_times_executed
        self._create_grouping_total_ticks_accumulated=create_grouping_total_ticks_accumulated
        self._create_grouping_shortest_execution=create_grouping_shortest_execution
        self._create_grouping_longest_execution=create_grouping_longest_execution
        self._create_grouping_tick_frequency=create_grouping_tick_frequency
        self._create_grouping_fastest_execution=create_grouping_fastest_execution
        self._create_grouping_slowest_execution=create_grouping_slowest_execution
        self._create_grouping_average_time_per_call=create_grouping_average_time_per_call
        self._create_grouping_total_time_of_all_calls=create_grouping_total_time_of_all_calls
        
        self._draw_number_of_times_executed=draw_number_of_times_executed
        self._draw_total_ticks_accumulated=draw_total_ticks_accumulated
        self._draw_shortest_execution=draw_shortest_execution
        self._draw_longest_execution=draw_longest_execution
        self._draw_tick_frequency=draw_tick_frequency
        self._draw_fastest_execution=draw_fastest_execution
        self._draw_slowest_execution=draw_slowest_execution
        self._draw_average_time_per_call=draw_average_time_per_call
        self._draw_total_time_of_all_calls=draw_total_time_of_all_calls
        
        self._potential_field_construction_number_of_times_executed=potential_field_construction_number_of_times_executed
        self._potential_field_construction_total_ticks_accumulated=potential_field_construction_total_ticks_accumulated
        self._potential_field_construction_shortest_execution=potential_field_construction_shortest_execution
        self._potential_field_construction_longest_execution=potential_field_construction_longest_execution
        self._potential_field_construction_tick_frequency=potential_field_construction_tick_frequency
        self._potential_field_construction_fastest_execution=potential_field_construction_fastest_execution
        self._potential_field_construction_slowest_execution=potential_field_construction_slowest_execution
        self._potential_field_construction_average_time_per_call=potential_field_construction_average_time_per_call
        self._potential_field_construction_total_time_of_all_calls=potential_field_construction_total_time_of_all_calls
        
        super(CcAI,self).__init__(number_of_times_executed,
                total_ticks_accumulated,
                shortest_execution,
                longest_execution,
                fastest_execution,
                slowest_execution,
                average_time_per_call,
                total_time_of_all_calls,
                tick_frequency)
        
# Used to set the values of a class from a list of values
# Must be a very well formed dict for the values to be assigned correctly.
    def setValuesFromDict(self, valuesDict):
        self._compute_density_field_number_of_times_executed=valuesDict['compute_density_field_number_of_times_executed']
        self._compute_density_field_total_ticks_accumulated=valuesDict['compute_density_field_total_ticks_accumulated']
        self._compute_density_field_shortest_execution=valuesDict['compute_density_field_shortest_execution']
        self._compute_density_field_longest_execution=valuesDict['compute_density_field_longest_execution']
        self._compute_density_field_tick_frequency=valuesDict['compute_density_field_tick_frequency']
        self._compute_density_field_fastest_execution=valuesDict['compute_density_field_fastest_execution']
        self._compute_density_field_slowest_execution=valuesDict['compute_density_field_slowest_execution']
        self._compute_density_field_average_time_per_call=valuesDict['compute_density_field_average_time_per_call']
        self._compute_density_field_total_time_of_all_calls=valuesDict['compute_density_field_total_time_of_all_calls']
         
        self._create_grouping_number_of_times_executed=valuesDict['create_grouping_number_of_times_executed']
        self._create_grouping_total_ticks_accumulated=valuesDict['create_grouping_total_ticks_accumulated']
        self._create_grouping_shortest_execution=valuesDict['create_grouping_shortest_execution']
        self._create_grouping_longest_execution=valuesDict['create_grouping_longest_execution']
        self._create_grouping_tick_frequency=valuesDict['create_grouping_tick_frequency']
        self._create_grouping_fastest_execution=valuesDict['create_grouping_fastest_execution']
        self._create_grouping_slowest_execution=valuesDict['create_grouping_slowest_execution']
        self._create_grouping_average_time_per_call=valuesDict['create_grouping_average_time_per_call']
        self._create_grouping_total_time_of_all_calls=valuesDict['create_grouping_total_time_of_all_calls']
        
        self._draw_number_of_times_executed=valuesDict['draw_number_of_times_executed']
        self._draw_total_ticks_accumulated=valuesDict['draw_total_ticks_accumulated']
        self._draw_shortest_execution=valuesDict['draw_shortest_execution']
        self._draw_longest_execution=valuesDict['draw_longest_execution']
        self._draw_tick_frequency=valuesDict['draw_tick_frequency']
        self._draw_fastest_execution=valuesDict['draw_fastest_execution']
        self._draw_slowest_execution=valuesDict['draw_slowest_execution']
        self._draw_average_time_per_call=valuesDict['draw_average_time_per_call']
        self._draw_total_time_of_all_calls=valuesDict['draw_total_time_of_all_calls']
        
        self._potential_field_construction_number_of_times_executed=valuesDict['potential_field_construction_number_of_times_executed']
        self._potential_field_construction_total_ticks_accumulated=valuesDict['potential_field_construction_total_ticks_accumulated']
        self._potential_field_construction_shortest_execution=valuesDict['potential_field_construction_shortest_execution']
        self._potential_field_construction_longest_execution=valuesDict['potential_field_construction_longest_execution']
        self._potential_field_construction_tick_frequency=valuesDict['potential_field_construction_tick_frequency']
        self._potential_field_construction_fastest_execution=valuesDict['potential_field_construction_fastest_execution']
        self._potential_field_construction_slowest_execution=valuesDict['potential_field_construction_slowest_execution']
        self._potential_field_construction_average_time_per_call=valuesDict['potential_field_construction_average_time_per_call']
        self._potential_field_construction_total_time_of_all_calls=valuesDict['potential_field_construction_total_time_of_all_calls']
        
        super(CcAI, self).setValuesFromDict(valuesDict)
    
    def getccAI(self, cur, n):
        cur.execute("SELECT * FROM " + self._table_name + " where " + self._id_name + " = %s", (str(n),))
        row = cur.fetchone()
        tmpccAI = CcAI()
        tmpccAI.setValuesFromDict(row)
        return tmpccAI

    def insertAlgorithmData(self, cur, 
                    compute_density_field_number_of_times_executed,
                    compute_density_field_total_ticks_accumulated,
                    compute_density_field_shortest_execution,
                    compute_density_field_longest_execution,
                    compute_density_field_tick_frequency,
                    compute_density_field_fastest_execution,
                    compute_density_field_slowest_execution,
                    compute_density_field_average_time_per_call,
                    compute_density_field_total_time_of_all_calls,
                    
                    create_grouping_number_of_times_executed,
                    create_grouping_total_ticks_accumulated,
                    create_grouping_shortest_execution,
                    create_grouping_longest_execution,
                    create_grouping_tick_frequency,
                    create_grouping_fastest_execution,
                    create_grouping_slowest_execution,
                    create_grouping_average_time_per_call,
                    create_grouping_total_time_of_all_calls,
                    
                    draw_number_of_times_executed,
                    draw_total_ticks_accumulated,
                    draw_shortest_execution,
                    draw_longest_execution,
                    draw_tick_frequency,
                    draw_fastest_execution,
                    draw_slowest_execution,
                    draw_average_time_per_call,
                    draw_total_time_of_all_calls,
                    
                    potential_field_construction_number_of_times_executed,
                    potential_field_construction_total_ticks_accumulated,
                    potential_field_construction_shortest_execution,
                    potential_field_construction_longest_execution,
                    potential_field_construction_tick_frequency,
                    potential_field_construction_fastest_execution,
                    potential_field_construction_slowest_execution,
                    potential_field_construction_average_time_per_call,
                    potential_field_construction_total_time_of_all_calls,
                    
                    number_of_times_executed ,
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
                        "VALUES(%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s," + 
                        "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s," + 
                        "%s,%s,%s,%s,%s)", # all 46 of them
                        (
                        next_val,
                        compute_density_field_number_of_times_executed,
                        compute_density_field_total_ticks_accumulated,
                        compute_density_field_shortest_execution,
                        compute_density_field_longest_execution,
                        compute_density_field_tick_frequency,
                        compute_density_field_fastest_execution,
                        compute_density_field_slowest_execution,
                        compute_density_field_average_time_per_call,
                        compute_density_field_total_time_of_all_calls,
                        
                        create_grouping_number_of_times_executed,
                        create_grouping_total_ticks_accumulated,
                        create_grouping_shortest_execution,
                        create_grouping_longest_execution,
                        create_grouping_tick_frequency,
                        create_grouping_fastest_execution,
                        create_grouping_slowest_execution,
                        create_grouping_average_time_per_call,
                        create_grouping_total_time_of_all_calls,
                        
                        draw_number_of_times_executed,
                        draw_total_ticks_accumulated,
                        draw_shortest_execution,
                        draw_longest_execution,
                        draw_tick_frequency,
                        draw_fastest_execution,
                        draw_slowest_execution,
                        draw_average_time_per_call,
                        draw_total_time_of_all_calls,
                        
                        potential_field_construction_number_of_times_executed,
                        potential_field_construction_total_ticks_accumulated,
                        potential_field_construction_shortest_execution,
                        potential_field_construction_longest_execution,
                        potential_field_construction_tick_frequency,
                        potential_field_construction_fastest_execution,
                        potential_field_construction_slowest_execution,
                        potential_field_construction_average_time_per_call,
                        potential_field_construction_total_time_of_all_calls,
                        
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
            print 'Error insertPprAIData: %s' % e    
            
    def insertAlgorithmData2(self, cur):
        return self.insertAlgorithmData(cur, 
                    self._compute_density_field_number_of_times_executed,
                    self._compute_density_field_total_ticks_accumulated,
                    self._compute_density_field_shortest_execution,
                    self._compute_density_field_longest_execution,
                    self._compute_density_field_tick_frequency,
                    self._compute_density_field_fastest_execution,
                    self._compute_density_field_slowest_execution,
                    self._compute_density_field_average_time_per_call,
                    self._compute_density_field_total_time_of_all_calls,
                    
                    self._create_grouping_number_of_times_executed,
                    self._create_grouping_total_ticks_accumulated,
                    self._create_grouping_shortest_execution,
                    self._create_grouping_longest_execution,
                    self._create_grouping_tick_frequency,
                    self._create_grouping_fastest_execution,
                    self._create_grouping_slowest_execution,
                    self._create_grouping_average_time_per_call,
                    self._create_grouping_total_time_of_all_calls,
                    
                    self._draw_number_of_times_executed,
                    self._draw_total_ticks_accumulated,
                    self._draw_shortest_execution,
                    self._draw_longest_execution,
                    self._draw_tick_frequency,
                    self._draw_fastest_execution,
                    self._draw_slowest_execution,
                    self._draw_average_time_per_call,
                    self._draw_total_time_of_all_calls,
                    
                    self._potential_field_construction_number_of_times_executed,
                    self._potential_field_construction_total_ticks_accumulated,
                    self._potential_field_construction_shortest_execution,
                    self._potential_field_construction_longest_execution,
                    self._potential_field_construction_tick_frequency,
                    self._potential_field_construction_fastest_execution,
                    self._potential_field_construction_slowest_execution,
                    self._potential_field_construction_average_time_per_call,
                    self._potential_field_construction_total_time_of_all_calls,
                    
                    self.get_number_of_times_executed(),
                    self.get_total_ticks_accumulated(),
                    self.get_shortest_execution(),
                    self.get_longest_execution(),
                    self.get_fastest_execution(),
                    self.get_slowest_execution(),
                    self.get_average_time_per_call(),
                    self.get_total_time_of_all_calls(),
                    self.get_tick_frequency())
            # sys.exit(1)          
            
