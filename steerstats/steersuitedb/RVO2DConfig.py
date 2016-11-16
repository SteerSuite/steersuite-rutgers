import psycopg2
# from Sequence import ConfigSequence

class RVO2DConfig(object):
    _id_name = "rvo2d_ai_config_id"
    _table_name = "rvo2d_ai_config"
    _insert_order = """  
(rvo2d_ai_config_id,
rvo_neighbor_distance ,
rvo_time_horizon ,
rvo_max_speed ,
rvo_time_horizon_obstacles ,
rvo_max_neighbors
)"""

    """
    rvo2d_ai_config
(

rvo2d_ai_config_id integer NOT NULL references config(config_id),
rvo_neighbor_distance double precision not null,
rvo_time_horizon double precision not null,
rvo_max_speed double precision not null,
rvo_time_horizon_obstacles double precision not null,
rvo_max_neighbors integer not null,
CONSTRAINT rvo2d_ai_config_pkey PRIMARY KEY (rvo2d_ai_config_id)

) ;"""

    # data should be a properly formed dictionary
    def __init__(self, data):
        self._data=data
    
    def getConfigData(self, cur, n):
        cur.execute("SELECT * FROM " + self._table_name + " where " + self._id_name + " = "+ str(n))
        row = cur.fetchone()
        return RVO2DConfig(row)
    
    def setValuesFromDict(self, valuesDict):
        self._data = valuesDict

    def insertConfig(self, cur, data):
        try:
            

            
            cur.execute("INSERT INTO " + self._table_name + " " +
                        self._insert_order + " " +
                        "VALUES(%s,%s,%s,%s,%s,%s)",
                        ( 
                            data['rvo2d_ai_config_id'],
                            data['rvo_neighbor_distance'],
                            data['rvo_time_horizon'],
                            data['rvo_max_speed'],
                            data['rvo_time_horizon_obstacles'],
                            data['rvo_max_neighbors']
                            
                        )
                        )
            return data['rvo2d_ai_config_id']
            
        except psycopg2.DatabaseError, e:
            print 'Error RVO2DConfig: %s' % e    
            
    def insertConfig2(self, cur):
        return self.insertConfig(cur, self._data)
            # sys.exit(1)          
            
