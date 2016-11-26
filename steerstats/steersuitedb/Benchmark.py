import psycopg2
import sys
from steersuitedb.Sequence import BenchmarkSequence 

class Benchmark(object):
    """A simple example class"""
    _table_name = "benchmark"
    _insert_order = "(benchmark_id, name, metadata)"
    
# benchmark 
# (
#   benchmark_id integer NOT NULL primary key,
#   name varchar(100) NOT NULL,
#   metadata text NOT NULL
# )

    def __init__(self, id=0, name="", metadata=""):
        self._name = name
        self._id = id
        self._metadata = metadata
        
    def getBenchmark(self, cur, n):
        cur.execute("SELECT * FROM " + self._table_name + " where benchmark_id = "+ str(n))
        row = cur.fetchone()
        tmpBench = Benchmark()
        tmpBench.setValuesFromDict(row)
        return tmpBench
    
    # return the index of the algorithm with the given name
    # -1 will be returned if no algorithm is found
    def getBenchmarkIndex(self, cur, name):
        try:
            # print "name given to getBenchmarkIndex: " + name
            cur.execute("SELECT benchmark_id FROM " + self._table_name + " where name = '" + name + "'")
            index = cur.fetchone()['benchmark_id']
            return index
        except psycopg2.DatabaseError, e:
            print 'Error getBenchmarkIndex: %s' % e    
            
    def setValuesFromDict(self, valuesDict):
        self._name=valuesDict['name']
        self._metadata=valuesDict['metadata']
        self._id=valuesDict['benchmark_id']
    
    def insertBenchmark(self, cur, name, metadata):
        try:
            
            benchSeq = BenchmarkSequence()
            next_id = benchSeq.getNextVal(cur)
            
            cur.execute("INSERT INTO " + self._table_name + " " +
                        self._insert_order + " " +
                        "VALUES(" + 
                        str(next_id) + ",'" +
                        name + "','" +
                        metadata + "')")
                        
        except psycopg2.DatabaseError, e:
            print 'Error %s' % e    
            sys.exit(1)
            
    def insertBenchmark2(self, cur):
        self.insertBenchmark(cur, self._name, self._metadata)
        
        