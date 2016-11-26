#!/usr/bin/python
# Replace <USERNAME_DATABASE>, <USERNAME>, and <PASSWORD> below with your actual DB, user, and password.

import psycopg2
import psycopg2.extras
import sys

# Append parent directory to access steersuite api.
sys.path.append("../")

from steersuitedb.Recording import Recording
from steersuitedb.Util import getTime
from steersuitedb.Scenario import Scenario
from steersuitedb.Algorithm import Algorithm
from steersuitedb.Benchmark import Benchmark
from steersuitedb.ConnectionManager import ConnectionManager

con = None

recording_dir="recordings/"

try:
     
    conM = ConnectionManager()
    con = conM.getConnection()
    test_id=4
    _recording_ids = []
 
    # gets a Dict like cursor so that columns can be referenced by name.
    cur = con.cursor(cursor_factory=psycopg2.extras.DictCursor)
    
    cur.execute("select recording_id from recording")
    
    recording_ids = cur.fetchall()
    for _recording_id in recording_ids:
        _recording_ids.append(int(_recording_id['recording_id']))
    
    print _recording_ids
    
    # provide a list of keys for the recordings
    recording = Recording()

    # provide a list of keys for the recordings
    for recording_id in _recording_ids:
        result = recording.getRecording(cur, recording_id)
    
        recording_file = open(recording_dir+str(recording_id)+".rec","w+b")
        recording_file.write(result.getRecordingData())
        recording_file.close()


except psycopg2.DatabaseError, e:
    print 'Error pprAI-test: %s' % e    
    sys.exit(1)
    
    
finally:
    
    if con:
        con.close() 
    
    
