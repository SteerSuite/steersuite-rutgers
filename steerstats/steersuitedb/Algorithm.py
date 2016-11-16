import psycopg2
import sys

class Algorithm(object):
    """Used to abstract the meta table that holds meta information 
    about an algorithm"""
    # Should define the name of the table for use in sql strings
    _table_name = "algorithm"
		# Should define the insert order to make insertion stronger.
    _insert_order = "(algorithm_id, name, metadata)"
    
#  algorithm 
#  (
#    algorithm_id int primary key,
#    name varchar(100) NOT NULL,
#    metadata text NOT NULL
#  )


    def __init__(self, id=0, name="", metadata=""):
        self._name = name
        self._id = id
        self._metadata = metadata

    def get_table_name(self):
        return self.__table_name


    def get_insert_order(self):
        return self.__insert_order


    def get_name(self):
        return self.__name


    def get_id(self):
        return self.__id


    def get_metadata(self):
        return self.__metadata


    def set_table_name(self, value):
        self.__table_name = value


    def set_insert_order(self, value):
        self.__insert_order = value


    def set_name(self, value):
        self.__name = value


    def set_id(self, value):
        self.__id = value


    def set_metadata(self, value):
        self.__metadata = value


    def del_table_name(self):
        del self.__table_name


    def del_insert_order(self):
        del self.__insert_order


    def del_name(self):
        del self.__name


    def del_id(self):
        del self.__id


    def del_metadata(self):
        del self.__metadata

        
    def getAlgorithmByIndex(self, cur, n):
        cur.execute("SELECT * FROM " + self._table_name + " where algorithm_id = " + str(n))
        row = cur.fetchone()
        return row
    
# return the index of the algorithm with the given name
# -1 will be returned if no algorithm is found
    def getAlgorithmIndex(self, cur, name):
        try:
            # print "name given to getAlgorithmIndex: " + name
            cur.execute("SELECT algorithm_id FROM " + self._table_name + " where name = '" + name + "'")
            index = cur.fetchone()[0]
            return index
        except psycopg2.DatabaseError, e:
            print 'Error getAlgorithmIndex: %s' % e   
            return -1 
    
    
    def insertAlgorithm(self, cur, id, name, metadata):
        try:
            cur.execute("INSERT INTO " + self._table_name + " " +
                        self._insert_order + " " +
                        "VALUES(" + str(id) + ",'" +
                        name + "','" +
                        metadata + "')")
        except psycopg2.DatabaseError, e:
            print 'Error %s' % e    
                        
    def insertAlgorithm(self, cur):
        try:
            cur.execute("INSERT INTO " + self._table_name + " " +
                        self._insert_order + " " +
                        "VALUES(" + str(self._id) + ",'" +
                        self._name + "'," +
                        "'" + self._metadata + "')")
                        
        except psycopg2.DatabaseError, e:
            print 'Error %s' % e    
    table_name = property(get_table_name, set_table_name, del_table_name, "table_name's docstring")
    insert_order = property(get_insert_order, set_insert_order, del_insert_order, "insert_order's docstring")
    name = property(get_name, set_name, del_name, "name's docstring")
    id = property(get_id, set_id, del_id, "id's docstring")
    metadata = property(get_metadata, set_metadata, del_metadata, "metadata's docstring")
