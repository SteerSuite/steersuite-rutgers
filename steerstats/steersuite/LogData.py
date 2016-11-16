import steersuite
import platform
import ctypes as ct
libraryDirectory = '../build/bin/'
_system = platform.system()
print '========================' + _system
if _system == 'Darwin':
	lib = ct.cdll.LoadLibrary(libraryDirectory+'libutil.dylib')
elif _system == 'Windows':
    lib = ct.cdll.LoadLibrary(libraryDirectory+'util.dll')
else: # Linux
	lib = ct.cdll.LoadLibrary(libraryDirectory+'libutil.so')

(_Integer, _Float, _LongLong, _String)  = (0, 1, 2, 3)
"""
    Very useful!!!!!
    
    If you set the argtypes or restype attributes of ctypes functions, they will return the right Python object without the need for a cast.

"""

class LogObject(object):
    def __init__(self, logobj=None):
        if logobj:
            self.obj = logobj
        else:
            self.obj = lib.LogObject_new()

    def addFloatItem(self, ingredient):
        lib._addLogObjectFloat(self.obj, ct.c_float(ingredient))
    def addIntItem(self, ingredient):   
        lib._addLogObjectInt(self.obj, ingredient)
    def getIntegerLogData(self, index):
        lib.getIntegerLogData.restype=ct.c_int
        return lib.getIntegerLogData(self.obj, index)
    def getFloatLogData(self, index):
        lib.getFloatLogData.restype=ct.c_float
        return lib.getFloatLogData(self.obj, index)
    def getLongLongLogData(self, index):
        lib.getLongLongLogData.restype=ct.c_longlong
        return lib.getLongLongLogData(self.obj, index)
    def getStringLogData(self, index):
    	"""
    	data format
    	'(28.748154,13.263118,29.410814,17.909399,12.241623,24.213131)' 
    	
    	"""
        lib.getStringLogData.restype=ct.c_char_p
        data = lib.getStringLogData(self.obj, index)
        data = [float(i) for i in data.strip('()').split(',')] 
        return data
    def getLogObjectData(self, log, index):
        """A more Pythonic way of handling things."""
        if log.getLoggerFieldDataType(index) == _Integer:
            return self.getIntegerLogData(index)
        elif log.getLoggerFieldDataType(index) == _Float:
            return self.getFloatLogData(index)
        elif log.getLoggerFieldDataType(index) == _LongLong:
            return self.getLongLongLogData(index)
        elif log.getLoggerFieldDataType(index) == _String:
            return self.getStringLogData(index)
        else:
            print "Data type not supported"
            sys.exit(4)
    def getLogObjectSize(self):
        return lib.getLogObjectSize(self.obj)
    
class Logger(object):
    def __init__(self, log=None):
        if log:
            self.obj = log
        else:
            self.obj = lib.new_Logger()

    def addIntergerDataField(self, label):
        lib.addIntegerDataField(self.obj, label)
    def addFloatDataField(self, label):
        lib.addFloatDataField(self.obj, label)

    def printLogData(self, logData):
        out = ct.c_char_p(lib.printLogData(self.obj, logData.obj))
        return out.value

    def getLogMetaData(self):
        lib.getLogMetaData.restype=ct.c_char_p
        out = lib.getLogMetaData(self.obj)
        return out
    
    def getFieldName(self, index):
        lib.getFieldName.restype=ct.c_char_p
        return lib.getFieldName(self.obj, index)
    
    def getLoggerFieldDataType(self, index):
        lib.getLoggerFieldDataType.restype=ct.c_int
        return lib.getLoggerFieldDataType(self.obj, index)
    

class LogData(object):
    
    def __init__(self, logData):
        self.obj = logData
        
    def getLogger(self):
        lib.getLogger.restype=ct.c_void_p
        return Logger(lib.getLogger(self.obj))
    def getLogDataAt(self, position):
        lib.getLogObjectAt.restype=ct.c_void_p
        return LogObject(lib.getLogObjectAt(self.obj, position))
    def getLogDataLength(self):
        return ct.c_uint64(lib.logDataLength(self.obj)).value
    def getDictofLogItem(self, index):
        dict = {}
        i=0
        log = self.getLogger()
        logobj = self.getLogDataAt(index)
        for i in range(logobj.getLogObjectSize()):
            key = log.getFieldName(i)
            data = logobj.getLogObjectData(log, i)
            # print "Adding key: " + str(key) + ", with data: " + str(data) 
            dict[key]=data
            
        return dict
    def __del__(self):
        # Allow Python to call the C++ object's destructor.
        return lib.logDelete(self.obj)
        
        
        
    
    
    
