from subprocess import call
import os, re

def RunSteerSuite(params, config_file, commandlineFlag):
    import platform
    _system = platform.system()
    if _system == "Darwin" or _system == "Linux" or _system == 'Windows':
        pathToSteerSuite="../build/bin/steersim.exe"
    else:
        pathToSteerSuite="../build/win32/Release/steersim.exe"
        
    _RunSteerSuite(pathToSteerSuite, params, config_file, commandlineFlag)

def _RunSteerSuite(pathToSteerSuite, params, config_file, commandlineFlag):
    import subprocess
    print pathToSteerSuite
    print params
    if commandlineFlag == "":
        call([pathToSteerSuite, "-module", params, "-config", config_file], stderr=subprocess.STDOUT)
        print "call done"
    else:
        try:
            print "calling stuff"
            print [pathToSteerSuite, "-module", params, "-config", config_file, commandlineFlag ]
            call([pathToSteerSuite, "-module", params, "-config", config_file, commandlineFlag], stderr=subprocess.STDOUT)
            print "call done"
        except Exception as err:
            print ('There was an error call ing SteerSuite')
        
        

    # Not a good place to put this.
def purge(dir, pattern):
    for f in os.listdir(dir):
        if re.search(pattern, f):
            os.remove(os.path.join(dir, f))