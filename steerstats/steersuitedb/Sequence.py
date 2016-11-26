# Defines some sequence accessors


class Sequence(object):
    _sequence_name=""
    
    def __init__(self, sequence_name=""):
        self._sequence_name = sequence_name 
        
    def getNextVal(self, cur):
        try:
            cur.execute("Select * from nextval('" + self._sequence_name + "')")
            next_id = cur.fetchone()[0] # gets the first column of the fetch row
            return next_id
        
        except psycopg2.DatabaseError, e:
            print "Error get nextval error for " + self._sequence_name + "  : %s" % e  
    

class AlgorithmDataSequence(Sequence):
    _sequence_name="algorithm_data_seq"
    
# algorithm_data_seq START 1 INCREMENT BY 1;
    
    def __init__(self, sequence_name=_sequence_name):
        self._sequence_name = sequence_name 
        

class ScenarioSequence(Sequence):
    _sequence_name="scenario_seq"

# scenario_seq START 1 INCREMENT BY 1;

    def __init__(self, sequence_name=_sequence_name):
        self._sequence_name = sequence_name 
    

class TestSequence(Sequence):
    _sequence_name="test_seq"

# test_seq START 1 INCREMENT BY 1;

    def __init__(self, sequence_name=_sequence_name):
        self._sequence_name = sequence_name 
        
class BenchmarkSequence(Sequence):
    _sequence_name="benchmark_seq"

# benchmark_seq START 1 INCREMENT BY 1;

    def __init__(self, sequence_name=_sequence_name):
        self._sequence_name = sequence_name 
        
class ConfigSequence(Sequence):
    _sequence_name="config_seq"

# config_seq START 1 INCREMENT BY 1;

    def __init__(self, sequence_name=_sequence_name):
        self._sequence_name = sequence_name 
        
        
        
        