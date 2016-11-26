import psycopg2
import psycopg2.extensions
from psycopg2.extensions import b
from steersuitedb.Video import Video


try:
     
    con = psycopg2.connect(database='steersuitedb', user='steeruser', password='steersuite') 
    con.set_isolation_level(0) 
    
    cur = con.cursor()  


    videoFile = open('ppr/scenarioFrames/scenario0/scenario0.mp4', 'r+b') # +b for binarry
    videoArray = bytearray()


    bytes = videoFile.read();
    
    for byte in bytes:
        videoArray.append(byte)
        
    video = Video(4, videoArray)
    status = video.insertVideo(cur)
    
    # print "Video Array size is: " + videoArray
    
    
    
    # cur.execute("insert into video (video_id, video_data) values (%s,%s)",(4,videoArray))


    con.commit()
    
#    cur.execute("select video_data from video where video_id=%s",str(3) )
    
#    video_data = cur.fetchone()[0]
    
#    videoFileOut = open('ppr/scenarioFrames/scenario0/scenario1.mp4', 'w+b') # +b for binarry
    
#    videoFileOut.write(video_data)



    print "About to create large object"    
    # oid = psycopg2.extensions.lobject()
    
    videoFile.close()
    # videoFileOut.close()

except psycopg2.DatabaseError, e:
    print 'Error pprAI-test: %s' % e    
    
    
finally:
    
    if con:
        con.close() 
        

    