import psycopg2
from Sequence import ConfigSequence

class Config(object):
    """A simple example class"""
    _id_name = "config_id"
    _table_name = "config"
    _insert_order = """  
(
    config_id,
    name ,
    fps ,
    grid_size_z ,
    grid_size_x ,
    grid_cells_z ,
    grid_cells_x ,
    config_xml
)"""

#config
#(
#   config_id integer NOT NULL primary key,
#    name varchar(100) NOT NULL,
#    fps integer not NULL,
#    grid_size_z integer not NULL,
#    grid_size_x integer not NULL,
#    grid_cells_z integer not NULL,
#    grid_cells_x integer not NULL,
#    config_xml text
#) ;

    def __init__(self, name="" ,
                    fps=0 ,
                    grid_size_z=0 ,
                    grid_size_x=0 ,
                    grid_cells_z=0 ,
                    grid_cells_x=0 ,
                    config_xml=""):
        self._name = name
        self._fps = fps
        self._grid_size_z = grid_size_z
        self._grid_size_x = grid_size_x
        self._grid_cells_z = grid_cells_z
        self._grid_cells_x = grid_cells_x
        self._config_xml = config_xml
    
    def getConfigData(self, cur, n):
        cur.execute("SELECT * FROM " + self._table_name + " where " + self._id_name + " = "+ str(n))
        row = cur.fetchone()
        return row
    
    def setValuesFromDict(self, valuesDict):
        self._name = valuesDict['name']
        self._fps = valuesDict['fps']
        self._grid_size_z = valuesDict['grid_size_z']
        self._grid_size_x = valuesDict['grid_size_x']
        self._grid_cells_z = valuesDict['grid_cells_z']
        self._grid_cells_x = valuesDict['grid_cells_x']
        self._config_xml = valuesDict['config_xml']

    def insertConfig(self, cur,     name ,    fps ,    grid_size_z , 
                        grid_size_x ,    grid_cells_z, grid_cells_x ,    config_xml):
        try:
            
            configSeq = ConfigSequence()
            next_id = configSeq.getNextVal(cur)
            
            
            cur.execute("INSERT INTO " + self._table_name + " " +
                        self._insert_order + " " +
                        "VALUES(%s,%s,%s,%s,%s,%s,%s,%s)",
                        ( 
                            next_id,
                            name,
                            fps,
                            grid_size_z,
                            grid_size_x,
                            grid_cells_z,
                            grid_cells_x,
                            config_xml
                        )
                        )
            return next_id
            
        except psycopg2.DatabaseError, e:
            print 'Error Config: %s' % e    
            
    def insertConfig2(self, cur):
        return self.insertConfig(cur, self._name, self._fps, self._grid_size_z, 
                                 self._grid_size_x, self._grid_cells_z,
                                 self._grid_cells_x, self._config_xml)
            # sys.exit(1)          
            
