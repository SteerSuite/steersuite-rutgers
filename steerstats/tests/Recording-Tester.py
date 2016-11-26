import psycopg2
import psycopg2.extensions
import sys

sys.path.append("../")

from psycopg2.extensions import b
from steersuitedb.Recording import Recording
from steersuitedb.ConnectionManager import ConnectionManager



try:
     
    conM = ConnectionManager()
    con = conM.getConnection() 
    
    cur = con.cursor()  


    # RecordFile = open('ppr/scenarioFrames/scenario0/scenario0.mp4', 'r+b') # +b for binarry
    recordFile = open('../ppr/rec0.rec', 'r+b') # +b for binarry
    recordArray = bytearray()


    bytes = recordFile.read();
    
    for byte in bytes:
        recordArray.append(byte)
        
    recording = Recording(3, recordArray)
    status = recording.insertRecording(cur)
    
    # print "record Array size is: " + recordArray
    
    
    
    # cur.execute("insert into record (record_id, record_data) values (%s,%s)",(4,recordArray))


    con.commit()
    
#    cur.execute("select video_data from video where video_id=%s",str(3) )
    
#    video_data = cur.fetchone()[0]
    
#    videoFileOut = open('ppr/scenarioFrames/scenario0/scenario1.mp4', 'w+b') # +b for binarry
    
#    videoFileOut.write(video_data)



    print "About to create large object"    
    # oid = psycopg2.extensions.lobject()
    
    recordFile.close()
    # videoFileOut.close()

except psycopg2.DatabaseError, e:
    print 'Error pprAI-test: %s' % e    
    
    
finally:
    
    if con:
        con.close() 
        

    