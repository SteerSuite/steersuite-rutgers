
# Doesn't do anything for now
__all__ = ['LogParser', 'Configuration', 'TestCaseFile']

import sys
from ctypes import cdll, c_char_p, LibraryLoader
import ctypes
from numpy.ctypeslib import c_intp
from steersuite.LogParser import LogParser
from steersuite.LogData import *
import signal

_system = platform.system()
# the GLOBAL acess is required so that dynamically loaded libraries can access steerlib functions
libraryDirectory = '../build/bin/'
if _system == 'Darwin': # OSX
    libraryLocation=libraryDirectory+'steersimlib.dylib'
    lib = ct.cdll.LoadLibrary(libraryLocation)
elif _system == 'Windows':
    libraryLocation=libraryDirectory+'steersimlib.dll'
    print '======' + libraryLocation
    lib = ct.cdll.LoadLibrary(libraryLocation)
else: # Linux
    libraryLocation=libraryDirectory+'steersimlib.so'
    lib = ct.cdll.LoadLibrary(libraryLocation)
       
def Init_SteerSim(argc, argv):
    """
    Returns the data as a really long string
    """
    arr = (c_char_p * len(argv))()
    arr[:] = argv
    
    out = ctypes.c_char_p(lib.init_steersim2(argc, arr))
    # print out.value
    data=""
    for item in out.value:
        data = data + item
    return data

def init_steerSim(argc, argv):
    """
        Returns the data as a Object that encapsulates the useful data.
        A LogData object.
        
        Returns a list of dicts of all of the data from the simulation
        
        Note: for now lists are interpreted from strings.
    """
    lib = ctypes.CDLL(libraryLocation, mode=ctypes.RTLD_GLOBAL)
    arr = (c_char_p * len(argv))()
    arr[:] = argv
    try: 
        out = ctypes.c_void_p(lib.init_steersim2(argc, arr))
    except Exception as inst:
        print "Exception running SteerSuite " + str(inst)
        # terminate=True;
    # print out.value
    if out.value is None:
    	print out.value
    	raise Exception('SteerSuite fault')
    logData = LogData(out)
    data={}
    _dict = logData.getDictofLogItem(0)
    # print _dict
    for key, value in _dict.iteritems():
    	data[key] = []
    for i in range(0, logData.getLogDataLength(), 1):
        _dict = logData.getDictofLogItem(i)
        for key, value in _dict.iteritems():
    		data[key].append(value)
        # data.append(_dict)
    # print data
    return data
   
