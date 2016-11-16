import psycopg2
from steersuitedb import AlgorithmData
from steersuitedb.BaseAI import BaseAI

class PprAI(BaseAI):
    """Class used to encapsulate the metric data for the PPRAI steering
    algorithm"""
    
    _id_name = "ppr_ai_id"
    _algorithm_reff = "pprAI"
    _table_name = "ppr_ai_data"
    _insert_order = """(ppr_ai_id, longplan, midplan, shortplan, perceptive, predictive, reactive,
   long_term_planning_number_of_times_executed,
    long_term_planning_total_ticks_accumulated,
    long_term_planning_shortest_execution,
    long_term_planning_longest_execution,
    long_term_planning_fastest_execution,
    long_term_planning_slowest_execution,
    long_term_planning_average_time_per_call,
    long_term_planning_total_time_of_all_calls,
    long_term_planning_tick_frequency,
    
    mid_term_planning_number_of_times_executed,
    mid_term_planning_total_ticks_accumulated,
    mid_term_planning_shortest_execution,
    mid_term_planning_longest_execution,
    mid_term_planning_fastest_execution,
    mid_term_planning_slowest_execution,
    mid_term_planning_average_time_per_call,
    mid_term_planning_total_time_of_all_calls,
    mid_term_planning_tick_frequency,
    
    short_term_planning_number_of_times_executed,
    short_term_planning_total_ticks_accumulated,
    short_term_planning_shortest_execution,
    short_term_planning_longest_execution,
    short_term_planning_fastest_execution,
    short_term_planning_slowest_execution,
    short_term_planning_average_time_per_call,
    short_term_planning_total_time_of_all_calls,
    short_term_planning_tick_frequency,
    
    perceptive_number_of_times_executed,
    perceptive_total_ticks_accumulated,
    perceptive_shortest_execution,
    perceptive_longest_execution,
    perceptive_fastest_execution,
    perceptive_slowest_execution,
    perceptive_average_time_per_call,
    perceptive_total_time_of_all_calls,
    perceptive_tick_frequency,
    
    predictive_number_of_times_executed,
    predictive_total_ticks_accumulated,
    predictive_shortest_execution,
    predictive_longest_execution,
    predictive_fastest_execution,
    predictive_slowest_execution,
    predictive_average_time_per_call,
    predictive_total_time_of_all_calls,
    predictive_tick_frequency,
    
    reactive_number_of_times_executed,
    reactive_total_ticks_accumulated,
    reactive_shortest_execution,
    reactive_longest_execution,
    reactive_fastest_execution,
    reactive_slowest_execution,
    reactive_average_time_per_call,
    reactive_total_time_of_all_calls,
    reactive_tick_frequency,
    
    steering_number_of_times_executed,
    steering_total_ticks_accumulated,
    steering_shortest_execution,
    steering_longest_execution,
    steering_fastest_execution,
    steering_slowest_execution,
    steering_average_time_per_call,
    steering_total_time_of_all_calls,
    steering_tick_frequency,
    
    number_of_times_executed,
    total_ticks_accumulated,
    shortest_execution,
    longest_execution,
    fastest_execution,
    slowest_execution,
    average_time_per_call,
    total_time_of_all_calls,
    tick_frequency,
    
    percent_mid_term,
    percent_short_term,
    percent_perceptive,
    percent_predictive,
    percent_reactive,
    percent_steering,
    average_per_agent,
    average_per_agent_4hz,
    average_per_agent_5hz,
    
    average_frequency_mid_term,  
    average_frequency_short_term,  
    average_frequency_perceptive,
    average_frequency_predictive, 
    average_frequency_reactive, 
    average_frequency_steering, 
    
    amortized_percent_mid_term,
    amortized_percent_short_term,
    amortized_percent_perceptive,
    amortized_percent_predictive,
    amortized_percent_reactive,
    amortized_percent_steering,
    average_per_agent_per_update
    )"""
# (first, last, age, address, city, state)


    """ppr_ai_data (
  ppr_ai_id integer NOT NULL references algorithm_data(algorithm_data_id),
  longplan integer,
  midplan integer,
  shortplan integer,
  perceptive integer,
  predictive integer,
  reactive integer,
_long_term_planning_number_of_times_executed bigint NOT NULL,
_long_term_planning_total_ticks_accumulated bigint NOT NULL,
_long_term_planning_shortest_execution bigint NOT NULL,
_long_term_planning_longest_execution bigint NOT NULL,
_long_term_planning_fastest_execution double precision NOT NULL,
_long_term_planning_slowest_execution double precision NOT NULL,
_long_term_planning_average_time_per_call double precision NOT NULL,
_long_term_planning_total_time_of_all_calls double precision NOT NULL,
_long_term_planning_tick_frequency double precision NOT NULL,

_mid_term_planning_number_of_times_executed bigint NOT NULL,
_mid_term_planning_total_ticks_accumulated bigint NOT NULL,
    _mid_term_planning_shortest_execution bigint NOT NULL,
    _mid_term_planning_longest_execution bigint NOT NULL,
    _mid_term_planning_fastest_execution double precision NOT NULL,
    _mid_term_planning_slowest_execution double precision NOT NULL,
    _mid_term_planning_average_time_per_call double precision NOT NULL,
    _mid_term_planning_total_time_of_all_calls double precision NOT NULL,
    _mid_term_planning_tick_frequency double precision NOT NULL,

    _short_term_planning_number_of_times_executed bigint NOT NULL,
    _short_term_planning_total_ticks_accumulated bigint NOT NULL,
    _short_term_planning_shortest_execution bigint NOT NULL,
    _short_term_planning_longest_execution bigint NOT NULL,
    _short_term_planning_fastest_execution double precision NOT NULL,
    _short_term_planning_slowest_execution double precision NOT NULL,
    _short_term_planning_average_time_per_call double precision NOT NULL,
    _short_term_planning_total_time_of_all_calls double precision NOT NULL,
    _short_term_planning_tick_frequency double precision NOT NULL,

    _perceptive_number_of_times_executed bigint NOT NULL,
    _perceptive_total_ticks_accumulated bigint NOT NULL,
    _perceptive_shortest_execution bigint NOT NULL,
    _perceptive_longest_execution bigint NOT NULL,
    _perceptive_fastest_execution double precision NOT NULL,
    _perceptive_slowest_execution double precision NOT NULL,
    _perceptive_average_time_per_call double precision NOT NULL,
    _perceptive_total_time_of_all_calls double precision NOT NULL,
    _perceptive_tick_frequency double precision NOT NULL,

    _predictive_number_of_times_executed bigint NOT NULL,
    _predictive_total_ticks_accumulated bigint NOT NULL,
    _predictive_shortest_execution bigint NOT NULL,
    _predictive_longest_execution bigint NOT NULL,
    _predictive_fastest_execution double precision NOT NULL,
    _predictive_slowest_execution double precision NOT NULL,
    _predictive_average_time_per_call double precision NOT NULL,
    _predictive_total_time_of_all_calls double precision NOT NULL,
    _predictive_tick_frequency double precision NOT NULL,

    _reactive_number_of_times_executed bigint NOT NULL,
    _reactive_total_ticks_accumulated bigint NOT NULL,
    _reactive_shortest_execution bigint NOT NULL,
    _reactive_longest_execution bigint NOT NULL,
    _reactive_fastest_execution double precision NOT NULL,
    _reactive_slowest_execution double precision NOT NULL,
    _reactive_average_time_per_call double precision NOT NULL,
    _reactive_total_time_of_all_calls double precision NOT NULL,
    _reactive_tick_frequency double precision NOT NULL,

    _steering_number_of_times_executed bigint NOT NULL,
    _steering_total_ticks_accumulated bigint NOT NULL,
    _steering_shortest_execution bigint NOT NULL,
    _steering_longest_execution bigint NOT NULL,
    _steering_fastest_execution double precision NOT NULL,
    _steering_slowest_execution double precision NOT NULL,
    _steering_average_time_per_call double precision NOT NULL,
    _steering_total_time_of_all_calls double precision NOT NULL,
    _steering_tick_frequency double precision NOT NULL,

    _total_number_of_times_executed bigint NOT NULL,
    _total_total_ticks_accumulated bigint NOT NULL,
    _total_shortest_execution bigint NOT NULL,
    _total_longest_execution bigint NOT NULL,
    _total_fastest_execution double precision NOT NULL,
    _total_slowest_execution double precision NOT NULL,
    _total_average_time_per_call double precision NOT NULL,
    _total_total_time_of_all_calls double precision NOT NULL,
    _total_tick_frequency double precision NOT NULL,

    _percent_mid_term double precision NOT NULL,
    _percent_short_term double precision NOT NULL,
    _percent_perceptive double precision NOT NULL,
    _percent_predictive double precision NOT NULL,
    _percent_reactive double precision NOT NULL,
    _percent_steering double precision NOT NULL,
    _average_per_agent double precision NOT NULL,
    _average_per_agent_4hz double precision NOT NULL,
    _average_per_agent_5hz double precision NOT NULL,

    --  TODO: 20 frames per second is a hard-coded assumption in the following calculations
    _average_frequency_mid-term double precision NOT NULL,  -- :   0.0542373 Hz (skipping 368.75 frames)
    _average_frequency_short-term double precision NOT NULL,  -- : 20 Hz (skipping 1 frames)
    _average_frequency_perceptive double precision NOT NULL,  -- : 20 Hz (skipping 1 frames)
    _average_frequency_predictive double precision NOT NULL,  -- : 0 Hz (skipping inf frames)
    _average_frequency_reactive double precision NOT NULL,  -- :   20 Hz (skipping 1 frames)
    _average_frequency_steering double precision NOT NULL,  -- :   20 Hz (skipping 1 frames)

    _amortized_percent_mid_term double precision NOT NULL,
    _amortized_percent_short_term double precision NOT NULL,
    _amortized_percent_perceptive double precision NOT NULL,
    _amortized_percent_predictive double precision NOT NULL,
    _amortized_percent_reactive double precision NOT NULL,
    _amortized_percent_steering double precision NOT NULL,
    _average_per_agent_per_update double precision NOT NULL,
) """

    def __init__(self, longplan=0, midplan=0, shortplan=0, perceptive=0, predictive=0, reactive=0,
                long_term_planning_number_of_times_executed=0,
                long_term_planning_total_ticks_accumulated=0,
                long_term_planning_shortest_execution=0,
                long_term_planning_longest_execution=0,
                long_term_planning_fastest_execution=0,
                long_term_planning_slowest_execution=0,
                long_term_planning_average_time_per_call=0,
                long_term_planning_total_time_of_all_calls=0,
                long_term_planning_tick_frequency=0,
                
                mid_term_planning_number_of_times_executed=0,
                mid_term_planning_total_ticks_accumulated=0,
                mid_term_planning_shortest_execution=0,
                mid_term_planning_longest_execution=0,
                mid_term_planning_fastest_execution=0,
                mid_term_planning_slowest_execution=0,
                mid_term_planning_average_time_per_call=0,
                mid_term_planning_total_time_of_all_calls=0,
                mid_term_planning_tick_frequency=0,
                
                short_term_planning_number_of_times_executed=0,
                short_term_planning_total_ticks_accumulated=0,
                short_term_planning_shortest_execution=0,
                short_term_planning_longest_execution=0,
                short_term_planning_fastest_execution=0,
                short_term_planning_slowest_execution=0,
                short_term_planning_average_time_per_call=0,
                short_term_planning_total_time_of_all_calls=0,
                short_term_planning_tick_frequency=0,
                
                perceptive_number_of_times_executed=0,
                perceptive_total_ticks_accumulated=0,
                perceptive_shortest_execution=0,
                perceptive_longest_execution=0,
                perceptive_fastest_execution=0,
                perceptive_slowest_execution=0,
                perceptive_average_time_per_call=0,
                perceptive_total_time_of_all_calls=0,
                perceptive_tick_frequency=0,
                
                predictive_number_of_times_executed=0,
                predictive_total_ticks_accumulated=0,
                predictive_shortest_execution=0,
                predictive_longest_execution=0,
                predictive_fastest_execution=0,
                predictive_slowest_execution=0,
                predictive_average_time_per_call=0,
                predictive_total_time_of_all_calls=0,
                predictive_tick_frequency=0,
                
                reactive_number_of_times_executed=0,
                reactive_total_ticks_accumulated=0,
                reactive_shortest_execution=0,
                reactive_longest_execution=0,
                reactive_fastest_execution=0,
                reactive_slowest_execution=0,
                reactive_average_time_per_call=0,
                reactive_total_time_of_all_calls=0,
                reactive_tick_frequency=0,
                
                steering_number_of_times_executed=0,
                steering_total_ticks_accumulated=0,
                steering_shortest_execution=0,
                steering_longest_execution=0,
                steering_fastest_execution=0,
                steering_slowest_execution=0,
                steering_average_time_per_call=0,
                steering_total_time_of_all_calls=0,
                steering_tick_frequency=0,
                
                number_of_times_executed=0,
                total_ticks_accumulated=0,
                shortest_execution=0,
                longest_execution=0,
                fastest_execution=0,
                slowest_execution=0,
                average_time_per_call=0,
                total_time_of_all_calls=0,
                tick_frequency=0,
                
                percent_mid_term=0,
                percent_short_term=0,
                percent_perceptive=0,
                percent_predictive=0,
                percent_reactive=0,
                percent_steering=0,
                average_per_agent=0,
                average_per_agent_4hz=0,
                average_per_agent_5hz=0,
                
                average_frequency_mid_term=0,  
                average_frequency_short_term=0,  
                average_frequency_perceptive=0,
                average_frequency_predictive=0, 
                average_frequency_reactive=0, 
                average_frequency_steering=0, 
                
                amortized_percent_mid_term=0,
                amortized_percent_short_term=0,
                amortized_percent_perceptive=0,
                amortized_percent_predictive=0,
                amortized_percent_reactive=0,
                amortized_percent_steering=0,
                average_per_agent_per_update=0):
        self._longplan = longplan
        self._midplan = midplan
        self._shortplan = shortplan
        self._perceptive = perceptive
        self._predictive = predictive
        self._reactive = reactive
        self._long_term_planning_number_of_times_executed=long_term_planning_number_of_times_executed
        self._long_term_planning_total_ticks_accumulated=long_term_planning_total_ticks_accumulated
        self._long_term_planning_shortest_execution=long_term_planning_shortest_execution
        self._long_term_planning_longest_execution=long_term_planning_longest_execution
        self._long_term_planning_fastest_execution=long_term_planning_fastest_execution
        self._long_term_planning_slowest_execution=long_term_planning_slowest_execution
        self._long_term_planning_average_time_per_call=long_term_planning_average_time_per_call
        self._long_term_planning_total_time_of_all_calls=long_term_planning_total_time_of_all_calls
        self._long_term_planning_tick_frequency=long_term_planning_tick_frequency

        self._mid_term_planning_number_of_times_executed=mid_term_planning_number_of_times_executed
        self._mid_term_planning_total_ticks_accumulated=mid_term_planning_total_ticks_accumulated
        self._mid_term_planning_shortest_execution=mid_term_planning_shortest_execution
        self._mid_term_planning_longest_execution=mid_term_planning_longest_execution
        self._mid_term_planning_fastest_execution=mid_term_planning_fastest_execution
        self._mid_term_planning_slowest_execution=mid_term_planning_slowest_execution
        self._mid_term_planning_average_time_per_call=mid_term_planning_average_time_per_call
        self._mid_term_planning_total_time_of_all_calls=mid_term_planning_total_time_of_all_calls
        self._mid_term_planning_tick_frequency=mid_term_planning_tick_frequency
    
        self._short_term_planning_number_of_times_executed=short_term_planning_number_of_times_executed
        self._short_term_planning_total_ticks_accumulated=short_term_planning_total_ticks_accumulated
        self._short_term_planning_shortest_execution=short_term_planning_shortest_execution
        self._short_term_planning_longest_execution=short_term_planning_longest_execution
        self._short_term_planning_fastest_execution=short_term_planning_fastest_execution
        self._short_term_planning_slowest_execution=short_term_planning_slowest_execution
        self._short_term_planning_average_time_per_call=short_term_planning_average_time_per_call
        self._short_term_planning_total_time_of_all_calls=short_term_planning_total_time_of_all_calls
        self._short_term_planning_tick_frequency=short_term_planning_tick_frequency
    
        self._perceptive_number_of_times_executed=perceptive_number_of_times_executed
        self._perceptive_total_ticks_accumulated=perceptive_total_ticks_accumulated
        self._perceptive_shortest_execution=perceptive_shortest_execution
        self._perceptive_longest_execution=perceptive_longest_execution
        self._perceptive_fastest_execution=perceptive_fastest_execution
        self._perceptive_slowest_execution=perceptive_slowest_execution
        self._perceptive_average_time_per_call=perceptive_average_time_per_call
        self._perceptive_total_time_of_all_calls=perceptive_total_time_of_all_calls
        self._perceptive_tick_frequency=perceptive_tick_frequency
    
        self._predictive_number_of_times_executed=predictive_number_of_times_executed
        self._predictive_total_ticks_accumulated=predictive_total_ticks_accumulated
        self._predictive_shortest_execution=predictive_shortest_execution
        self._predictive_longest_execution=predictive_longest_execution
        self._predictive_fastest_execution=predictive_fastest_execution
        self._predictive_slowest_execution=predictive_slowest_execution
        self._predictive_average_time_per_call=predictive_average_time_per_call
        self._predictive_total_time_of_all_calls=predictive_total_time_of_all_calls
        self._predictive_tick_frequency=predictive_tick_frequency
    
        self._reactive_number_of_times_executed=reactive_number_of_times_executed
        self._reactive_total_ticks_accumulated=reactive_total_ticks_accumulated
        self._reactive_shortest_execution=reactive_shortest_execution
        self._reactive_longest_execution=reactive_longest_execution
        self._reactive_fastest_execution=reactive_fastest_execution
        self._reactive_slowest_execution=reactive_slowest_execution
        self._reactive_average_time_per_call=reactive_average_time_per_call
        self._reactive_total_time_of_all_calls=reactive_total_time_of_all_calls
        self._reactive_tick_frequency=reactive_tick_frequency
    
        self._steering_number_of_times_executed=steering_number_of_times_executed
        self._steering_total_ticks_accumulated=steering_total_ticks_accumulated
        self._steering_shortest_execution=steering_shortest_execution
        self._steering_longest_execution=steering_longest_execution
        self._steering_fastest_execution=steering_fastest_execution
        self._steering_slowest_execution=steering_slowest_execution
        self._steering_average_time_per_call=steering_average_time_per_call
        self._steering_total_time_of_all_calls=steering_total_time_of_all_calls
        self._steering_tick_frequency=steering_tick_frequency
    
        super(PprAI,self).__init__(number_of_times_executed,
                total_ticks_accumulated,
                shortest_execution,
                longest_execution,
                fastest_execution,
                slowest_execution,
                average_time_per_call,
                total_time_of_all_calls,
                tick_frequency)
        """self._number_of_times_executed=number_of_times_executed
        self._total_ticks_accumulated=total_ticks_accumulated
        self._shortest_execution=shortest_execution
        self._longest_execution=longest_execution
        self._fastest_execution=fastest_execution
        self._slowest_execution=slowest_execution
        self._average_time_per_call=average_time_per_call
        self._total_time_of_all_calls=total_time_of_all_calls
        self._tick_frequency=tick_frequency"""
    
        self._percent_mid_term=percent_mid_term
        self._percent_short_term=percent_short_term
        self._percent_perceptive=percent_perceptive
        self._percent_predictive=percent_predictive
        self._percent_reactive=percent_reactive
        self._percent_steering=percent_steering
        self._average_per_agent=average_per_agent
        self._average_per_agent_=average_per_agent_4hz
        self._average_per_agent_=average_per_agent_5hz
    
        self._average_frequency_mid_term=average_frequency_mid_term  
        self._average_frequency_short_term=average_frequency_short_term  
        self._average_frequency_perceptive=average_frequency_perceptive  
        self._average_frequency_predictive=average_frequency_predictive  
        self._average_frequency_reactive=average_frequency_reactive 
        self._average_frequency_steering=average_frequency_steering 
    
        self._amortized_percent_mid_term=amortized_percent_mid_term
        self._amortized_percent_short_term=amortized_percent_short_term
        self._amortized_percent_perceptive=amortized_percent_perceptive
        self._amortized_percent_predictive=amortized_percent_predictive
        self._amortized_percent_reactive=amortized_percent_reactive
        self._amortized_percent_steering=amortized_percent_steering
        self._average_per_agent_per_update=average_per_agent_per_update
        
# Used to set the values of a class from a list of values
# Must be a very well formed dict for the values to be assigned correctly.
    def setValuesFromDict(self, valuesList):
        self._longplan=valuesList['longplan']
        self._midplan=valuesList['midplan']
        self._shortplan=valuesList['shortplan']
        self._perceptive=valuesList['perceptive']
        self._predictive=valuesList['predictive']
        self._reactive=valuesList['reactive']
        self._long_term_planning_number_of_times_executed=valuesList['long_term_planning_number_of_times_executed']
        self._long_term_planning_total_ticks_accumulated=valuesList['long_term_planning_total_ticks_accumulated']
        self._long_term_planning_shortest_execution=valuesList['long_term_planning_shortest_execution']
        self._long_term_planning_longest_execution=valuesList['long_term_planning_longest_execution']
        self._long_term_planning_fastest_execution=valuesList['long_term_planning_fastest_execution']
        self._long_term_planning_slowest_execution=valuesList['long_term_planning_slowest_execution']
        self._long_term_planning_average_time_per_call=valuesList['long_term_planning_average_time_per_call']
        self._long_term_planning_total_time_of_all_calls=valuesList['long_term_planning_total_time_of_all_calls']
        self._long_term_planning_tick_frequency=valuesList['long_term_planning_tick_frequency']

        self._mid_term_planning_number_of_times_executed=valuesList['mid_term_planning_number_of_times_executed']
        self._mid_term_planning_total_ticks_accumulated=valuesList['mid_term_planning_total_ticks_accumulated']
        self._mid_term_planning_shortest_execution=valuesList['mid_term_planning_shortest_execution']
        self._mid_term_planning_longest_execution=valuesList['mid_term_planning_longest_execution']
        self._mid_term_planning_fastest_execution=valuesList['mid_term_planning_fastest_execution']
        self._mid_term_planning_slowest_execution=valuesList['mid_term_planning_slowest_execution']
        self._mid_term_planning_average_time_per_call=valuesList['mid_term_planning_average_time_per_call']
        self._mid_term_planning_total_time_of_all_calls=valuesList['mid_term_planning_total_time_of_all_calls']
        self._mid_term_planning_tick_frequency=valuesList['mid_term_planning_tick_frequency']
    
        self._short_term_planning_number_of_times_executed=valuesList['short_term_planning_number_of_times_executed']
        self._short_term_planning_total_ticks_accumulated=valuesList['short_term_planning_total_ticks_accumulated']
        self._short_term_planning_shortest_execution=valuesList['short_term_planning_shortest_execution']
        self._short_term_planning_longest_execution=valuesList['short_term_planning_longest_execution']
        self._short_term_planning_fastest_execution=valuesList['short_term_planning_fastest_execution']
        self._short_term_planning_slowest_execution=valuesList['short_term_planning_slowest_execution']
        self._short_term_planning_average_time_per_call=valuesList['short_term_planning_average_time_per_call']
        self._short_term_planning_total_time_of_all_calls=valuesList['short_term_planning_total_time_of_all_calls']
        self._short_term_planning_tick_frequency=valuesList['short_term_planning_tick_frequency']
    
        self._perceptive_number_of_times_executed=valuesList['perceptive_number_of_times_executed']
        self._perceptive_total_ticks_accumulated=valuesList['perceptive_total_ticks_accumulated']
        self._perceptive_shortest_execution=valuesList['perceptive_shortest_execution']
        self._perceptive_longest_execution=valuesList['perceptive_longest_execution']
        self._perceptive_fastest_execution=valuesList['perceptive_fastest_execution']
        self._perceptive_slowest_execution=valuesList['perceptive_slowest_execution']
        self._perceptive_average_time_per_call=valuesList['perceptive_average_time_per_call']
        self._perceptive_total_time_of_all_calls=valuesList['perceptive_total_time_of_all_calls']
        self._perceptive_tick_frequency=valuesList['perceptive_tick_frequency']
    
        self._predictive_number_of_times_executed=valuesList['predictive_number_of_times_executed']
        self._predictive_total_ticks_accumulated=valuesList['predictive_total_ticks_accumulated']
        self._predictive_shortest_execution=valuesList['predictive_shortest_execution']
        self._predictive_longest_execution=valuesList['predictive_longest_execution']
        self._predictive_fastest_execution=valuesList['predictive_fastest_execution']
        self._predictive_slowest_execution=valuesList['predictive_slowest_execution']
        self._predictive_average_time_per_call=valuesList['predictive_average_time_per_call']
        self._predictive_total_time_of_all_calls=valuesList['predictive_total_time_of_all_calls']
        self._predictive_tick_frequency=valuesList['predictive_tick_frequency']
    
        self._reactive_number_of_times_executed=valuesList['reactive_number_of_times_executed']
        self._reactive_total_ticks_accumulated=valuesList['reactive_total_ticks_accumulated']
        self._reactive_shortest_execution=valuesList['reactive_shortest_execution']
        self._reactive_longest_execution=valuesList['reactive_longest_execution']
        self._reactive_fastest_execution=valuesList['reactive_fastest_execution']
        self._reactive_slowest_execution=valuesList['reactive_slowest_execution']
        self._reactive_average_time_per_call=valuesList['reactive_average_time_per_call']
        self._reactive_total_time_of_all_calls=valuesList['reactive_total_time_of_all_calls']
        self._reactive_tick_frequency=valuesList['reactive_tick_frequency']
    
        self._steering_number_of_times_executed=valuesList['steering_number_of_times_executed']
        self._steering_total_ticks_accumulated=valuesList['steering_total_ticks_accumulated']
        self._steering_shortest_execution=valuesList['steering_shortest_execution']
        self._steering_longest_execution=valuesList['steering_longest_execution']
        self._steering_fastest_execution=valuesList['steering_fastest_execution']
        self._steering_slowest_execution=valuesList['steering_slowest_execution']
        self._steering_average_time_per_call=valuesList['steering_average_time_per_call']
        self._steering_total_time_of_all_calls=valuesList['steering_total_time_of_all_calls']
        self._steering_tick_frequency=valuesList['steering_tick_frequency']
    
        super(PprAI,self).setValuesFromDict(valuesList)    
        """self._number_of_times_executed=valuesList['number_of_times_executed']
        self._total_ticks_accumulated=valuesList['total_ticks_accumulated']
        self._shortest_execution=valuesList['shortest_execution']
        self._longest_execution=valuesList['longest_execution']
        self._fastest_execution=valuesList['fastest_execution']
        self._slowest_execution=valuesList['slowest_execution']
        self._average_time_per_call=valuesList['average_time_per_call']
        self._total_time_of_all_calls=valuesList['total_time_of_all_calls']
        self._tick_frequency=valuesList['tick_frequency']"""
    
        self._percent_mid_term=valuesList['percent_mid_term']
        self._percent_short_term=valuesList['percent_short_term']
        self._percent_perceptive=valuesList['percent_perceptive']
        self._percent_predictive=valuesList['percent_predictive']
        self._percent_reactive=valuesList['percent_reactive']
        self._percent_steering=valuesList['percent_steering']
        self._average_per_agent=valuesList['average_per_agent']
        self._average_per_agent_4hz=valuesList['average_per_agent_4hz']
        self._average_per_agent_5hz=valuesList['average_per_agent_5hz']
    
        self._average_frequency_mid_term=valuesList['average_frequency_mid_term']  
        self._average_frequency_short_term=valuesList['average_frequency_short_term']  
        self._average_frequency_perceptive=valuesList['average_frequency_perceptive']  
        self._average_frequency_predictive=valuesList['average_frequency_predictive']  
        self._average_frequency_reactive=valuesList['average_frequency_reactive'] 
        self._average_frequency_steering=valuesList['average_frequency_steering'] 
    
        self._amortized_percent_mid_term=valuesList['amortized_percent_mid_term']
        self._amortized_percent_short_term=valuesList['amortized_percent_short_term']
        self._amortized_percent_perceptive=valuesList['amortized_percent_perceptive']
        self._amortized_percent_predictive=valuesList['amortized_percent_predictive']
        self._amortized_percent_reactive=valuesList['amortized_percent_reactive']
        self._amortized_percent_steering=valuesList['amortized_percent_steering']
        self._average_per_agent_per_update=valuesList['average_per_agent_per_update']
    
    def getpprAI(self, cur, n):
        cur.execute("SELECT * FROM " + self._table_name + " where " + self._id_name + " = %s", (str(n),))
        row = cur.fetchone()
        return row

    def insertAlgorithmData(self, cur, longplan, midplan, shortplan, perceptive, predictive, reactive,
                long_term_planning_number_of_times_executed,
                long_term_planning_total_ticks_accumulated,
                long_term_planning_shortest_execution,
                long_term_planning_longest_execution,
                long_term_planning_fastest_execution,
                long_term_planning_slowest_execution,
                long_term_planning_average_time_per_call,
                long_term_planning_total_time_of_all_calls,
                long_term_planning_tick_frequency,
                
                mid_term_planning_number_of_times_executed,
                mid_term_planning_total_ticks_accumulated,
                mid_term_planning_shortest_execution,
                mid_term_planning_longest_execution,
                mid_term_planning_fastest_execution,
                mid_term_planning_slowest_execution,
                mid_term_planning_average_time_per_call,
                mid_term_planning_total_time_of_all_calls,
                mid_term_planning_tick_frequency,
                
                short_term_planning_number_of_times_executed,
                short_term_planning_total_ticks_accumulated,
                short_term_planning_shortest_execution,
                short_term_planning_longest_execution,
                short_term_planning_fastest_execution,
                short_term_planning_slowest_execution,
                short_term_planning_average_time_per_call,
                short_term_planning_total_time_of_all_calls,
                short_term_planning_tick_frequency,
                
                perceptive_number_of_times_executed,
                perceptive_total_ticks_accumulated,
                perceptive_shortest_execution,
                perceptive_longest_execution,
                perceptive_fastest_execution,
                perceptive_slowest_execution,
                perceptive_average_time_per_call,
                perceptive_total_time_of_all_calls,
                perceptive_tick_frequency,
                
                predictive_number_of_times_executed,
                predictive_total_ticks_accumulated,
                predictive_shortest_execution,
                predictive_longest_execution,
                predictive_fastest_execution,
                predictive_slowest_execution,
                predictive_average_time_per_call,
                predictive_total_time_of_all_calls,
                predictive_tick_frequency,
                
                reactive_number_of_times_executed,
                reactive_total_ticks_accumulated,
                reactive_shortest_execution,
                reactive_longest_execution,
                reactive_fastest_execution,
                reactive_slowest_execution,
                reactive_average_time_per_call,
                reactive_total_time_of_all_calls,
                reactive_tick_frequency,
                
                steering_number_of_times_executed,
                steering_total_ticks_accumulated,
                steering_shortest_execution,
                steering_longest_execution,
                steering_fastest_execution,
                steering_slowest_execution,
                steering_average_time_per_call,
                steering_total_time_of_all_calls,
                steering_tick_frequency,
                
                number_of_times_executed,
                total_ticks_accumulated,
                shortest_execution,
                longest_execution,
                fastest_execution,
                slowest_execution,
                average_time_per_call,
                total_time_of_all_calls,
                tick_frequency,
                
                percent_mid_term,
                percent_short_term,
                percent_perceptive,
                percent_predictive,
                percent_reactive,
                percent_steering,
                average_per_agent,
                average_per_agent_4hz,
                average_per_agent_5hz,
                
                average_frequency_mid_term,  
                average_frequency_short_term,  
                average_frequency_perceptive,
                average_frequency_predictive, 
                average_frequency_reactive, 
                average_frequency_steering, 
                
                amortized_percent_mid_term,
                amortized_percent_short_term,
                amortized_percent_perceptive,
                amortized_percent_predictive,
                amortized_percent_reactive,
                amortized_percent_steering,
                average_per_agent_per_update):
        try:
            # insert the algorithmData reference
            ad = AlgorithmData.AlgorithmData()
            ad.setAlgorithmReff(self._algorithm_reff)
            next_val = ad.insertAlgorithmData2(cur)
            
            
            cur.execute("INSERT INTO " + self._table_name + " " +
                        self._insert_order + " " +
                        "VALUES(%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s," + 
                        "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s," + 
                        "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s," + 
                        "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s," + 
                        "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s)", # all 101 of them
                        (str(next_val),
                        str(longplan),
                        str(midplan),
                        str(shortplan),
                        str(perceptive),
                        str(predictive),
                        str(reactive),
                        str(long_term_planning_number_of_times_executed),
                        str(long_term_planning_total_ticks_accumulated),
                        str(long_term_planning_shortest_execution),
                        str(long_term_planning_longest_execution),
                        str(long_term_planning_fastest_execution),
                        str(long_term_planning_slowest_execution),
                        str(long_term_planning_average_time_per_call),
                        str(long_term_planning_total_time_of_all_calls),
                        str(long_term_planning_tick_frequency),
                        
                        str(mid_term_planning_number_of_times_executed),
                        str(mid_term_planning_total_ticks_accumulated),
                        str(mid_term_planning_shortest_execution),
                        str(mid_term_planning_longest_execution),
                        str(mid_term_planning_fastest_execution),
                        str(mid_term_planning_slowest_execution),
                        str(mid_term_planning_average_time_per_call),
                        str(mid_term_planning_total_time_of_all_calls),
                        str(mid_term_planning_tick_frequency),
                        
                        str(short_term_planning_number_of_times_executed),
                        str(short_term_planning_total_ticks_accumulated),
                        str(short_term_planning_shortest_execution),
                        str(short_term_planning_longest_execution),
                        str(short_term_planning_fastest_execution),
                        str(short_term_planning_slowest_execution),
                        str(short_term_planning_average_time_per_call),
                        str(short_term_planning_total_time_of_all_calls),
                        str(short_term_planning_tick_frequency),
                        
                        str(perceptive_number_of_times_executed),
                        str(perceptive_total_ticks_accumulated),
                        str(perceptive_shortest_execution),
                        str(perceptive_longest_execution),
                        str(perceptive_fastest_execution),
                        str(perceptive_slowest_execution),
                        str(perceptive_average_time_per_call),
                        str(perceptive_total_time_of_all_calls),
                        str(perceptive_tick_frequency),
                        
                        str(predictive_number_of_times_executed),
                        str(predictive_total_ticks_accumulated),
                        str(predictive_shortest_execution),
                        str(predictive_longest_execution),
                        str(predictive_fastest_execution),
                        str(predictive_slowest_execution),
                        str(predictive_average_time_per_call),
                        str(predictive_total_time_of_all_calls),
                        str(predictive_tick_frequency),
                        
                        str(reactive_number_of_times_executed),
                        str(reactive_total_ticks_accumulated),
                        str(reactive_shortest_execution),
                        str(reactive_longest_execution),
                        str(reactive_fastest_execution),
                        str(reactive_slowest_execution),
                        str(reactive_average_time_per_call),
                        str(reactive_total_time_of_all_calls),
                        str(reactive_tick_frequency),
                        
                        str(steering_number_of_times_executed),
                        str(steering_total_ticks_accumulated),
                        str(steering_shortest_execution),
                        str(steering_longest_execution),
                        str(steering_fastest_execution),
                        str(steering_slowest_execution),
                        str(steering_average_time_per_call),
                        str(steering_total_time_of_all_calls),
                        str(steering_tick_frequency),
                        
                        str(number_of_times_executed),
                        str(total_ticks_accumulated),
                        str(shortest_execution),
                        str(longest_execution),
                        str(fastest_execution),
                        str(slowest_execution),
                        str(average_time_per_call),
                        str(total_time_of_all_calls),
                        str(tick_frequency),
                        
                        str(percent_mid_term),
                        str(percent_short_term),
                        str(percent_perceptive),
                        str(percent_predictive),
                        str(percent_reactive),
                        str(percent_steering),
                        str(average_per_agent),
                        str(average_per_agent_4hz),
                        str(average_per_agent_5hz),
                        
                        str(average_frequency_mid_term),  
                        str(average_frequency_short_term),  
                        str(average_frequency_perceptive),
                        str(average_frequency_predictive), 
                        str(average_frequency_reactive), 
                        str(average_frequency_steering), 
                        
                        str(amortized_percent_mid_term),
                        str(amortized_percent_short_term),
                        str(amortized_percent_perceptive),
                        str(amortized_percent_predictive),
                        str(amortized_percent_reactive),
                        str(amortized_percent_steering),
                        str(average_per_agent_per_update)
                        
                        ))
            return next_val
            
        except psycopg2.DatabaseError, e:
            print 'Error insertPprAIData: %s' % e    
            
    def insertAlgorithmData2(self, cur):
        return self.insertAlgorithmData(cur, self._longplan, self._midplan, self._shortplan, self._perceptive, self._predictive, self._reactive,
                                    self._long_term_planning_number_of_times_executed,
                                    self._long_term_planning_total_ticks_accumulated,
                                    self._long_term_planning_shortest_execution,
                                    self._long_term_planning_longest_execution,
                                    self._long_term_planning_fastest_execution,
                                    self._long_term_planning_slowest_execution,
                                    self._long_term_planning_average_time_per_call,
                                    self._long_term_planning_total_time_of_all_calls,
                                    self._long_term_planning_tick_frequency,
                            
                                    self._mid_term_planning_number_of_times_executed,
                                    self._mid_term_planning_total_ticks_accumulated,
                                    self._mid_term_planning_shortest_execution,
                                    self._mid_term_planning_longest_execution,
                                    self._mid_term_planning_fastest_execution,
                                    self._mid_term_planning_slowest_execution,
                                    self._mid_term_planning_average_time_per_call,
                                    self._mid_term_planning_total_time_of_all_calls,
                                    self._mid_term_planning_tick_frequency,
                                
                                    self._short_term_planning_number_of_times_executed,
                                    self._short_term_planning_total_ticks_accumulated,
                                    self._short_term_planning_shortest_execution,
                                    self._short_term_planning_longest_execution,
                                    self._short_term_planning_fastest_execution,
                                    self._short_term_planning_slowest_execution,
                                    self._short_term_planning_average_time_per_call,
                                    self._short_term_planning_total_time_of_all_calls,
                                    self._short_term_planning_tick_frequency,
                                
                                    self._perceptive_number_of_times_executed,
                                    self._perceptive_total_ticks_accumulated,
                                    self._perceptive_shortest_execution,
                                    self._perceptive_longest_execution,
                                    self._perceptive_fastest_execution,
                                    self._perceptive_slowest_execution,
                                    self._perceptive_average_time_per_call,
                                    self._perceptive_total_time_of_all_calls,
                                    self._perceptive_tick_frequency,
                                
                                    self._predictive_number_of_times_executed,
                                    self._predictive_total_ticks_accumulated,
                                    self._predictive_shortest_execution,
                                    self._predictive_longest_execution,
                                    self._predictive_fastest_execution,
                                    self._predictive_slowest_execution,
                                    self._predictive_average_time_per_call,
                                    self._predictive_total_time_of_all_calls,
                                    self._predictive_tick_frequency,
                                
                                    self._reactive_number_of_times_executed,
                                    self._reactive_total_ticks_accumulated,
                                    self._reactive_shortest_execution,
                                    self._reactive_longest_execution,
                                    self._reactive_fastest_execution,
                                    self._reactive_slowest_execution,
                                    self._reactive_average_time_per_call,
                                    self._reactive_total_time_of_all_calls,
                                    self._reactive_tick_frequency,
                                
                                    self._steering_number_of_times_executed,
                                    self._steering_total_ticks_accumulated,
                                    self._steering_shortest_execution,
                                    self._steering_longest_execution,
                                    self._steering_fastest_execution,
                                    self._steering_slowest_execution,
                                    self._steering_average_time_per_call,
                                    self._steering_total_time_of_all_calls,
                                    self._steering_tick_frequency,
                                
                                    self.get_number_of_times_executed(),
                                    self.get_total_ticks_accumulated(),
                                    self.get_shortest_execution(),
                                    self.get_longest_execution(),
                                    self.get_fastest_execution(),
                                    self.get_slowest_execution(),
                                    self.get_average_time_per_call(),
                                    self.get_total_time_of_all_calls(),
                                    self.get_tick_frequency(),
                                
                                    self._percent_mid_term,
                                    self._percent_short_term,
                                    self._percent_perceptive,
                                    self._percent_predictive,
                                    self._percent_reactive,
                                    self._percent_steering,
                                    self._average_per_agent,
                                    self._average_per_agent,
                                    self._average_per_agent,
                                
                                    self._average_frequency_mid_term,  
                                    self._average_frequency_short_term,  
                                    self._average_frequency_perceptive,  
                                    self._average_frequency_predictive,  
                                    self._average_frequency_reactive, 
                                    self._average_frequency_steering, 
                                
                                    self._amortized_percent_mid_term,
                                    self._amortized_percent_short_term,
                                    self._amortized_percent_perceptive,
                                    self._amortized_percent_predictive,
                                    self._amortized_percent_reactive,
                                    self._amortized_percent_steering,
                                    self._average_per_agent_per_update)
            # sys.exit(1)          
            
