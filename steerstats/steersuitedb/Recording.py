import psycopg2

# this is not completely encapsulated by another transaction so it should
# be used by the client when inserting data
class Recording(object):
    _table_name = "recording"
    _insert_order = "(recording_id, recording_data)"
    _id_name="recording_id"

#recording (
#  recording_id integer NOT NULL primary key references test(test_id),
#  recording_data bytea NOT NULL
#) ;

    def __init__(self, record_id=0, record_data=""):
        self._record_id=record_id
        self._record_data=record_data
    
    def getRecording(self, cur, n):
        cur.execute("SELECT recording_id, recording_data FROM " + self._table_name + " where " + self._id_name + " = %s", (str(n),))
        row = cur.fetchone()
        return Recording(row['recording_id'], row['recording_data'])
    
    def getRecordingData(self):
        return self._record_data

# will Return -1 if insert did not work
    def insertRecording2(self, cur, record_id=0, record_data=""):
        try:
            
            cur.execute("INSERT INTO " + self._table_name + " " +
                        self._insert_order + " " +
                        "VALUES(%s, %s)",(
                        str(record_id),
                        record_data)
                        )
            return record_id
            
        except psycopg2.DatabaseError, e:
            print 'Error Recording %s' % e    
            
    def insertRecording(self, cur):
        self.insertRecording2(cur, self._record_id, self._record_data)
            
