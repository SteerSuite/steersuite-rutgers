import psycopg2
import Algorithm
from Util import getTime
from Sequence import AlgorithmDataSequence

class AlgorithmData(object):
    """Designed to hold the basic information used for the join table
    for storing algorithm data"""
    
    _algorithm_reff = ""
    _table_name = "algorithm_data"
    _insert_order = "(algorithm_data_id, timestamp, algorithm_type)"
# (first, last, age, address, city, state)

# algorithm_data 
# (
#   algorithm_data_id int NOT NULL primary key,
#   timestamp timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
#   algorithm_type int NOT NULL references algorithm(algorithm_id)
# )

    def __init__(self, id=0, time=0, type=0):
        self._time = time
        self._id = id
        self._type = type
        
    def setAlgorithmReff(self, reff):
        self._algorithm_reff = reff
    
    def getAlgorithmData(self, cur, n):
        cur.execute("SELECT * FROM " + self._table_name + " where algorithm_data_id = "+ str(n))
        row = cur.fetchone()
        return row

# Will insert a new row in the algorithm_data table
# return is the value of the primary key for that row.
    def insertAlgorithmData(self, cur):
        try:
            # TODO: should think about separating out sequence accessors
            alDataSeq = AlgorithmDataSequence()
            next_id = alDataSeq.getNextVal(cur)
            
            algorithm = Algorithm.Algorithm()
            algorithmIndex = algorithm.getAlgorithmIndex(cur, self._algorithm_reff)
            
            # get the current timestamp
            timestamp = getTime(cur)
            
            cur.execute("INSERT INTO " + self._table_name + " " +
                        self._insert_order + " " +
                        "VALUES(%s, %s, %s)",
                        (
                            str(next_id),
                            str(timestamp),
                            str(algorithmIndex)
                        )
                        )
            return next_id
            
        except psycopg2.DatabaseError, e:
            print 'Error AlgorithmData: %s' % e    
            
    def insertAlgorithmData2(self, cur):
        return self.insertAlgorithmData(cur)

    def get_algorithm_reff(self):
        return self.__algorithm_reff


    def get_time(self):
        return self.__time


    def get_type(self):
        return self.__type


    def set_algorithm_reff(self, value):
        self.__algorithm_reff = value


    def set_time(self, value):
        self.__time = value


    def set_type(self, value):
        self.__type = value


    def del_algorithm_reff(self):
        del self.__algorithm_reff


    def del_time(self):
        del self.__time


    def del_type(self):
        del self.__type

    algorithm_reff = property(get_algorithm_reff, set_algorithm_reff, del_algorithm_reff, "algorithm_reff's docstring")
    time = property(get_time, set_time, del_time, "time's docstring")
    type = property(get_type, set_type, del_type, "type's docstring")
            
