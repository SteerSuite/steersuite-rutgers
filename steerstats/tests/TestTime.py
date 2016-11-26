from datetime import datetime
import os.path

print datetime.now()


dataDirectory="data/"
logfileName="test.log"

if os.path.exists(dataDirectory+logfileName):
    os.rename(dataDirectory+logfileName,dataDirectory+str(datetime.now())+"-"+logfileName )