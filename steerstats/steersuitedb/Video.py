import psycopg2

# this is not ocmpletely incapsulated by another transaction so it should
# be used by the client when inserting data
class Video(object):
    """A simple example class"""
    _table_name = "video"
    _insert_order = "(video_id, video_data)"
    _id_name = "video_id"

#video 
#(
#  video_id integer NOT NULL primary key references test(test_id),
#  video_data bytea NOT NULL
#)

    def __init__(self, video_id=0, video_data=""):
        self._video_id=video_id
        self._video_data=video_data
    
    def getVideo(self, cur, n):
        cur.execute("SELECT video_data FROM " + self._table_name + " where " + self._id_name + " = %s", (n,))
        # cur.execute("SELECT video_data FROM " + self._table_name + " where " + self._id_name + " = " + str(n)) #%s", (str(n)))
        row = cur.fetchone()
        return Video(n, row['video_data'])
    
    def getVideoData(self):
        return self._video_data
    
    def getVideoID(self):
        return self._video_id

# will Return -1 if insert did not work
    def insertVideo2(self, cur, video_id=0, video_data=""):
        try:
            
            cur.execute("INSERT INTO " + self._table_name + " " +
                        self._insert_order + " " +
                        "VALUES(%s, %s)",(
                        str(video_id),
                        video_data)
                        )
            return video_id
            
        except psycopg2.DatabaseError, e:
            print 'Error %s' % e    
            
    def insertVideo(self, cur):
        self.insertVideo2(cur, self._video_id, self._video_data)
            
