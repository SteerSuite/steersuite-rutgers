import psycopg2
import copy
from steersuitedb.AlgorithmData import AlgorithmData

class BaseAI(AlgorithmData):
    """This class holds the base algorithm metrics that all algorithms
    must have."""
    

    def __init__(self, number_of_times_executed=0,
                total_ticks_accumulated=0,
                shortest_execution=0,
                longest_execution=0,
                fastest_execution=0,
                slowest_execution=0,
                average_time_per_call=0,
                total_time_of_all_calls=0,
                tick_frequency=0):
        self._number_of_times_executed=number_of_times_executed
        self._total_ticks_accumulated=total_ticks_accumulated
        self._shortest_execution=shortest_execution
        self._longest_execution=longest_execution
        self._fastest_execution=fastest_execution
        self._slowest_execution=slowest_execution
        self._average_time_per_call=average_time_per_call
        self._total_time_of_all_calls=total_time_of_all_calls
        self._tick_frequency=tick_frequency        
        
        
    def setValuesFromDict(self, valuesDict):
        self.set_number_of_times_executed(valuesDict['number_of_times_executed'])
        self.set_total_ticks_accumulated(valuesDict['total_ticks_accumulated'])
        self.set_shortest_execution(valuesDict['shortest_execution'])
        self.set_longest_execution(valuesDict['longest_execution'])
        self.set_fastest_execution(valuesDict['fastest_execution'])
        self.set_slowest_execution(valuesDict['slowest_execution'])
        self.set_average_time_per_call(valuesDict['average_time_per_call'])
        self.set_total_time_of_all_calls(valuesDict['total_time_of_all_calls'])
        self.set_tick_frequency(valuesDict['tick_frequency'])
        
    def setAIValuesFromDict(self, valuesDict):
        self.setValuesFromDict(valuesDict)

    def get_number_of_times_executed(self):
        return self._number_of_times_executed


    def get_total_ticks_accumulated(self):
        return self._total_ticks_accumulated


    def get_shortest_execution(self):
        return self._shortest_execution


    def get_longest_execution(self):
        return self._longest_execution


    def get_fastest_execution(self):
        return self._fastest_execution


    def get_slowest_execution(self):
        return self._slowest_execution


    def get_average_time_per_call(self):
        return self._average_time_per_call


    def get_total_time_of_all_calls(self):
        return self._total_time_of_all_calls


    def get_tick_frequency(self):
        return self._tick_frequency


    def set_number_of_times_executed(self, value):
        self._number_of_times_executed = value


    def set_total_ticks_accumulated(self, value):
        self._total_ticks_accumulated = value


    def set_shortest_execution(self, value):
        self._shortest_execution = value


    def set_longest_execution(self, value):
        self._longest_execution = value


    def set_fastest_execution(self, value):
        self._fastest_execution = value


    def set_slowest_execution(self, value):
        self._slowest_execution = value


    def set_average_time_per_call(self, value):
        self._average_time_per_call = value


    def set_total_time_of_all_calls(self, value):
        self._total_time_of_all_calls = value


    def set_tick_frequency(self, value):
        self._tick_frequency = value


    def del_number_of_times_executed(self):
        del self._number_of_times_executed


    def del_total_ticks_accumulated(self):
        del self._total_ticks_accumulated


    def del_shortest_execution(self):
        del self._shortest_execution


    def del_longest_execution(self):
        del self._longest_execution


    def del_fastest_execution(self):
        del self._fastest_execution


    def del_slowest_execution(self):
        del self._slowest_execution


    def del_average_time_per_call(self):
        del self._average_time_per_call


    def del_total_time_of_all_calls(self):
        del self._total_time_of_all_calls


    def del_tick_frequency(self):
        del self._tick_frequency

    number_of_times_executed = property(get_number_of_times_executed, set_number_of_times_executed, del_number_of_times_executed, "number_of_times_executed's docstring")
    total_ticks_accumulated = property(get_total_ticks_accumulated, set_total_ticks_accumulated, del_total_ticks_accumulated, "total_ticks_accumulated's docstring")
    shortest_execution = property(get_shortest_execution, set_shortest_execution, del_shortest_execution, "shortest_execution's docstring")
    longest_execution = property(get_longest_execution, set_longest_execution, del_longest_execution, "longest_execution's docstring")
    fastest_execution = property(get_fastest_execution, set_fastest_execution, del_fastest_execution, "fastest_execution's docstring")
    slowest_execution = property(get_slowest_execution, set_slowest_execution, del_slowest_execution, "slowest_execution's docstring")
    average_time_per_call = property(get_average_time_per_call, set_average_time_per_call, del_average_time_per_call, "average_time_per_call's docstring")
    total_time_of_all_calls = property(get_total_time_of_all_calls, set_total_time_of_all_calls, del_total_time_of_all_calls, "total_time_of_all_calls's docstring")
    tick_frequency = property(get_tick_frequency, set_tick_frequency, del_tick_frequency, "tick_frequency's docstring")
        
        
    