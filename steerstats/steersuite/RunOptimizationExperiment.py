from RunSteerSuite import RunSteerSuite
from multiprocessing import Semaphore
from InsertStatistics import InsertOptimizationConfig
import sys

sys.path.append("../")
  
# class RunOptimizatinoExperiement():
__db_processes=1
__dbSem=Semaphore(__db_processes)


#def __init__(self):
#    self.__dbSem = Semaphore(self.__db_processes)

def __RunExperiment(path_to_steer_suite, steersuite_params, 
                  config_file_path, commandline_flag, ai_params):
    # print "Run Experiment ai params"
    # print ai_params
    run_full_params=False
    if run_full_params:
        RunSteerSuite(path_to_steer_suite, steersuite_params+reconstuctAIParams(ai_params),
                   config_file_path, commandline_flag)
    else:
        RunSteerSuite(path_to_steer_suite, steersuite_params,
                   config_file_path, commandline_flag)
    __dbSem.acquire()
    InsertOptimizationConfig(steersuite_params, config_file_path,
                              ai_params)
    __dbSem.release()
    
def RunExperiment(args_dict):
    __RunExperiment(args_dict['path_to_steersuite'],
                          args_dict['steersuite_params'],
                          args_dict['config_file_path'],
                          args_dict['commandline_flag'],
                          args_dict['ai_params'])
    
def reconstuctAIParams(aiParams):
    out = ""
    for aiparam in aiParams:
        out = out + "," + aiparam + "=" + str(aiParams[aiparam])
    return out


