import psycopg2

class Table(object):
    """A simple example class"""
    _table_name = "algorithm"
    _insert_order = "(algorithm_id, name, metadata)"
    
#  algorithm 
#  (
#    algorithm_id int primary key,
#    name varchar(100) NOT NULL,
#    metadata text NOT NULL
#  )
        
    def getAlgorithm(self, cur, n):
        cur.execute("SELECT * FROM " + self._table_name + " where algorithm_id = " + str(n))
        row = cur.fetchone()
        return row
    
    
    def insertAlgorithm(cur, id, _name, metadata):
        try:
            cur.execute("INSERT INTO " + _table_name + " " +
                        _insert_order + " " +
                        "VALUES(" + str(id) + ",'" +
                        _name + "'," +
                        metadata + ")")
                        
        except psycopg2.DatabaseError, e:
            print 'Error %s' % e    
            sys.exit(1)
