import psycopg2

class ConnectionManager(object):
    _dbname='ppr_opt_steersuitedb'
    
    
    def __init__(self, dbname='steersuitedb', dbuser='steeruser', dbpassword='steersuite'):
        self._dbname=dbname
        self._dbuser=dbuser
        self._dbpassword = dbpassword
        
        self._conn = psycopg2.connect(database=self._dbname, user=self._dbuser,
                                       password=self._dbpassword)
        self._conn.set_isolation_level(0)
        
    def getConnection(self):
        # self._conn = psycopg2.connect(database='steersuitedb', user='postuser', password='suitesteer')
        return self._conn 
    
    def getCursor(self):
        return self._conn.cursor(cursor_factory=psycopg2.extras.DictCursor)
    
    def dispose(self):
        self._conn.close() 
        
    def setDataBase(self, dbname=""):
        self._dbname=dbname
        
    def initConnection(self):
        self._conn = psycopg2.connect(database=self._dbname, user=self._dbuser,
                                       password=self._dbpassword)
        self._conn.set_isolation_level(0)
        
