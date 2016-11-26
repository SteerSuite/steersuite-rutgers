#!/usr/bin/python
# Replace <USERNAME_DATABASE>, <USERNAME>, and <PASSWORD> below with your actual DB, user, and password.

import psycopg2
import psycopg2.extras
import sys

# Append parent directory to access steersuite api.
sys.path.append("../")

from steersuitedb.Video import Video
from steersuitedb.Util import getTime
from steersuitedb.Scenario import Scenario
from steersuitedb.Algorithm import Algorithm
from steersuitedb.Benchmark import Benchmark
from steersuitedb.ConnectionManager import ConnectionManager

con = None

video_dir="videos/"

try:
     
    conM = ConnectionManager()
    con = conM.getConnection()
    test_id=4
    _video_ids = []
 
    # gets a Dict like cursor so that columns can be referenced by name.
    cur = con.cursor(cursor_factory=psycopg2.extras.DictCursor)
    
    cur.execute("select video_id from video")
    
    video_ids = cur.fetchall()
    for _video_id in video_ids:
        _video_ids.append(int(_video_id['video_id']))
    
    print _video_ids
    
    # provide a list of keys for the videos
    video = Video()

    # provide a list of keys for the videos
    for video_id in _video_ids:
        result = video.getVideo(cur, video_id)
    
        video_file = open(video_dir+str(video_id)+".mp4","w+b")
        video_file.write(result.getVideoData())
        video_file.close()


except psycopg2.DatabaseError, e:
    print 'Error pprAI-test: %s' % e    
    sys.exit(1)
    
    
finally:
    
    if con:
        con.close() 
    
    
