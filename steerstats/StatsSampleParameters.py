from datetime import datetime
import os.path
import sys
import random
from multiprocessing import Process
# from pathos.multiprocessing import ProcessingPool as Pool
import time
from distutils.dir_util import copy_tree
from steersuite.SteerStatsOptions import getOptions
from steersuite.RunSteerSuite import purge
from steersuite.OptimizationExperiment import OptimizationExperiment
import copy
from multiprocessing import Semaphore
from steersuite.RunOptimizationExperiment import RunExperiment
from InsertStatistics import parseArg
from SteerStats import SteerStats
import InsertStatistics



# examples on how to execute this program

# clear; clear; python StatsOptimizeParameters.py --paramFile xml/config/params-config-test.xml --ai pprAI --numScenarios 2 --scenarioDIR scenarios/pprAI --scenarioSetInitId 2172 -c
# clear; clear; python StatsOptimizeParameters.py --paramFile xml/config/params-config-test.xml --ai pprAI --numScenarios 5 --scenarioDIR scenarios/pprAI --scenarioSetInitId 2172 -c --processes 3

# clear; clear; nice -18 python StatsOptimizeParameters.py --paramFile xml/config/param-config-test2.xml --ai pprAI --numScenarios 10000 --scenarioDIR data/scenarios/representativeSet --scenarioSetInitId 0 -c --processes 3

# clear; clear; nice -18 time python StatsOptimizeParameters.py --paramFile xml/config/param-config-test2.xml --ai pprAI --numScenarios 10000 --scenarioDIR data/scenarios/representativeSet --scenarioSetInitId 0 -c --processes 8

# clear; clear; nice -18 time python StatsOptimizeParameters.py --paramFile xml/config/param-config-test2.xml --ai pprAI --numScenarios 10000 --scenarioDIR data/scenarios/representativeSet --scenarioSetInitId 0 -c --processes 16 --db ppr_opt_steersuitedb

# clear; clear; nice -18 time python StatsOptimizeParameters.py --paramFile xml/config/param-config-test2.xml --ai pprAI --numScenarios 10 --scenarioDIR data/scenarios/representativeSet --scenarioSetInitId 0 -c --processes 2 --db density_opt_steersuitedb

# clear; clear; time nice -4 python StatsOptimizeParameters.py --paramFile xml/config/ppr-param-config.xml --ai pprAI --numScenarios 4 --scenarioDIR data/scenarios/performanceSanityCheck/ --scenarioSetInitId 0 -c --processes 14 --db density_opt_steersuitedb --dataDir /Volumes/Block0/SteerStatsDataPPRDensity/ --numFrames 100

# clear; clear; time nice -18 python StatsSampleParameters.py --paramFile xml/config/rvo2d-param-config.xml --ai rvo2dAI --numScenarios 10 --scenarioDIR data/scenarios/representativeSet_Intersections/ --scenarioSetInitId 0 -c --processes 1 --benchmark compositePLE

# clear; clear; time nice -18 python StatsSampleParameters.py --paramFile xml/config/ppr-dependant_test-config.xml --ai pprAI --numScenarios 10 --scenarioDIR data/scenarios/representativeSet_Intersections/ --scenarioSetInitId 0 -c --processes 1 --benchmark compositePLE
i=0
class StatsOptimizationParameters():


    def __init__(self, options=None):
        self._options=options
        
    def OptimizeParameters2(self, options):
        
        options = getOptions()
        options.noReturn=True
        availableProcesses=int(options.processes)
        options.processes=int(1)
        steerStats = SteerStats(options)
        steerStats.set_insert_func(InsertStatistics.InsertOptimizationConfig)
    
        param_xml_file = open(options.paramFile)
        param_config_data = param_xml_file.read()
        
        OptimExper = OptimizationExperiment(param_config_data)
        optimizingParameters = OptimExper.parseExperimentXML()
    
        # print "Algorithm param names: " + str(ai_param_names_list)
        experiements = []
        aiParams = []
        for param in optimizingParameters:
        
            # this will create a lot of directories but I won't have to worry about
            # syncroniztion and reuse of directories
            for n in range(int(param.get_descetization())):
                aiParams_tmp = {}
                """
                for param_ in optimizingParameters:
                    if param_.get_type() == "float":
                        aiParams_tmp[param_.get_name()] = str(float(param_.get_original()))
                    elif param_.get_type() == "integer":
                        aiParams_tmp[param_.get_name()] = str(int(param_.get_original()))
                """
                # calculate param value for this iteration
                if param.get_type() == "float":
                    paramValue = param.get_min() + (((param.get_max()-param.get_min())/param.get_descetization())*n)
                elif param.get_type() == "integer":
                    paramValue = int(param.get_min() + (((param.get_max()-param.get_min())/param.get_descetization())*n))
                aiParams_tmp[param.get_name()] = paramValue
                tmp_params = []
                tmp_param_names = []        
                for key, value in aiParams_tmp.items() :
                    tmp_params.append(value)
                    tmp_param_names.append(key)
                
                experiements.append(copy.deepcopy(tmp_params))
                aiParams.append(copy.deepcopy(tmp_param_names))
                
                #TODO stuff here.
        
        
        print "Number of experiments in-line: " + str(len(experiements))
        print "Size of process pool: " + str(availableProcesses)
        
        # sys.exit()
        processes_pool = Pool(availableProcesses)
        
        processes_pool.map(steerStats.RunStats, experiements, aiParams)        
        print "Waited for subprocess"
        
    def OptimizeParameters(self, options):

        availableProcesses=int(options.processes)
        _dataPath=options.dataPath
        _scenarioSetInitId=options.scenarioSetStartId
        benchmarkFileName=options.benchmarkFileName
        benchmarkTechnique=options.benchmark
        aiModuleName=options.steeringAlgorithm
        logfileName=options.benchmarkFileName
        scenarioSetPath=options.scenarioDirectory
        frameSize=options.frameSize
        _numScenarios=int(options.numScenarios) 
        
        steersuite_semaphore = Semaphore(availableProcesses)
        
        i =0 
        
        if options.commandLine:
            commandLine="-commandline" 
        else:
            commandLine=""
             
        # ../build/bin/steersim -module $moduleParams -config $configFile
        # valgrind ../build/bin/steersim -module $moduleParams -config $configFile
        
                  
        optimization_experiement_processes = []
        
        
        param_xml_file = open(options.paramFile)
        param_config_data = param_xml_file.read()
        
        OptimExper = OptimizationExperiment(param_config_data)
        optimizingParameters = OptimExper.parseExperimentXML()
        
        aiParams = {}
        
        # optimization_experiment = RunOptimizatinoExperiement()
        
        experiement_arguments = []
        
        for param in optimizingParameters:
        
            # this will create a lot of directories but I won't have to worry about
            # syncroniztion and reuse of directories
            for n in range(int(param.get_descetization())):
                
                dataPath=_dataPath+"/"+str(i)
                i = i + 1
                # print dataPath
            
                recordingDirectory=dataPath+"/"+options.recordingDirectory+"/"
                frameDirectory=dataPath+"/"+options.frameDirectory+"/"
                ailogFilePath=dataPath+"/"+aiModuleName+".log"
            
                # test if any of the files/directories that are going to be used already exist
                # if they do rename them.
                timeNow = str(datetime.now()) # this will keep all of the directories renamed
                # with the same timestamp
                if True:
                    if os.path.exists(dataPath+"/"+benchmarkFileName):
                        print "Moving old benchmark log file."
                        os.rename( dataPath+"/"+benchmarkFileName, dataPath+"/"+str(timeNow)+"-"+benchmarkFileName )
                        
                    if os.path.exists(ailogFilePath):
                        print "Moving old AI log file."
                        os.rename( ailogFilePath, dataPath+"/"+str(timeNow)+"-"+aiModuleName+".log" )
                        
                    if os.path.exists(recordingDirectory):
                        print "Moving old recording files:"
                        copy_tree(os.path.abspath(recordingDirectory),
                                  os.path.abspath(recordingDirectory)+"_bak_"+str(timeNow))
                        purge(recordingDirectory, "rec[0-9]*.rec")
                    
                    # Create directories if they do not exists         
                    if not os.path.exists(recordingDirectory):
                        print "Creating directory: " + recordingDirectory
                        os.makedirs(os.path.abspath(recordingDirectory))
                        
                    if not os.path.exists(frameDirectory):
                        print "Creating directory: " + frameDirectory
                        os.makedirs(os.path.abspath(frameDirectory))
                        
                # configFile="configs/footstep-config2.xml"
                configFile="configs/"+aiModuleName+"-config.xml"
                
                # sys.exit()
            
            
                # regular need scenario module parameters
                moduleParams="scenario,scenarioAI="+aiModuleName+",useBenchmark,benchmarkTechnique="+benchmarkTechnique+",benchmarkLog="+dataPath+"/"+benchmarkFileName+",checkAgentValid,reducedGoals,fixedSpeed,fixedDirection,checkAgentRelevant,minAgents=3,ailogFileName="+ailogFilePath #,randomSeed=120"
                if options.recordSimulation:
                    moduleParams=moduleParams+",recFile=rec,recFileFolder="+recordingDirectory # save record files for later benchmarking
                if options.numFrames != None:
                    moduleParams=moduleParams+",maxFrames="+str(options.numFrames)
                    
                moduleParams=moduleParams+",egocentric" # puts the reference agent right in the middle of the scenario
                
                # moduleParams=+moduleParams+",dumpFrames,framePath="+frameDirectory #save test frames to make videos
                if ( options.saveFrames and options.framePath != ""):
                   moduleParams=moduleParams+",dumpFrames,framePath="+str(options.framePath) 
                # one one of the following line should ever be included. THe operations exclude eachother.
            
                # print "number of scenarios to run will be: " + str(_numScenarios)
                moduleParams=moduleParams+",scenarioSetPath="+scenarioSetPath+",scenarioSetInitId="+str(_scenarioSetInitId)+",numScenarios="+str(_numScenarios) # can be used to load bunch of testcases
                for param_ in optimizingParameters:
                    if param_.get_type() == "float":
                        aiParams[param_.get_name()] = str(float(param_.get_original()))
                    elif param_.get_type() == "integer":
                        aiParams[param_.get_name()] = str(int(param_.get_original()))
                # calculate param value for this iteration
                paramValue=""
                if options.passAllParams:
                    for param__ in optimizingParameters:
                        if param_.get_type() == "float":
                            moduleParams=moduleParams+","+param__.get_name()+"="+str(float(param__.get_original()))
                        elif param_.get_type() == "integer":
                            moduleParams=moduleParams+","+param__.get_name()+"="+str(float(param__.get_original()))
                else:
                    paramValue = param.get_min() + (((param.get_max()-param.get_min())/param.get_descetization())*n)
                    moduleParams=moduleParams+","+param.get_name()+"="+str(paramValue)
                    aiParams[param.get_name()] = paramValue
                # this section is for ppr_ai parameters
                # moduleParams=moduleParams+",ped_max_speed=10"
                # moduleParams=moduleParams+",ped_typical_speed=8"
                moduleParams=moduleParams+",dbName="+options.dbName
        
                         
                # print aiParams
                # sys.exit()
                args_dict={}
                args_dict['path_to_steersuite'] = "../build/bin/steersim"
                args_dict['steersuite_params'] = moduleParams
                args_dict['config_file_path'] = configFile
                args_dict['commandline_flag'] = commandLine
                args_dict['ai_params'] = copy.deepcopy(aiParams)
                
                experiement_arguments.append(args_dict)
                
        
        # At this point a large list of dictionary objects will have been created that can be
        # passed to an optimization_experiement object to be run
        
        
        # print experiement_arguments
        
        print "Number of experiments in-line: " + str(i)
        
        # sys.exit()
        processes_pool = Pool(processes=availableProcesses)
        
        processes_pool.map(func=RunExperiment,
                            iterable=experiement_arguments)
        print "Waited for subprocess"
        
        
        # encode all of the videos
        # echo "tools/encodeVideos $frameDirectory $numScenarios $frameSize"
        # tools/encodeVideos "$frameDirectory" $numScenarios $frameSize
        
        #insert everything into database
        # python InsertStatistics.py $moduleParams $configFile
        
    def OptimizeParameter(self, options, params_list=None, _ai_params_so_far=""):

        if len(params_list) < 1:
            return [None,None]
        
        _dataPath=options.dataPath
        _scenarioSetInitId=options.scenarioSetStartId
        benchmarkFileName=options.benchmarkFileName
        benchmarkTechnique=options.benchmark
        aiModuleName=options.steeringAlgorithm
        logfileName=options.benchmarkFileName
        scenarioSetPath=options.scenarioDirectory
        frameSize=options.frameSize
        _numScenarios=int(options.numScenarios) 

        param = params_list.pop(0)
        
        if not hasattr(options, 'i'):
            i =0
        else:
            i = options.i 
        
        if options.commandLine:
            commandLine="-commandline" 
        else:
            commandLine=""
             
        # ../build/bin/steersim -module $moduleParams -config $configFile
        # valgrind ../build/bin/steersim -module $moduleParams -config $configFile
        
                  
        optimization_experiement_processes = []
        
        param_xml_file = open(options.paramFile)
        param_config_data = param_xml_file.read()
        
        OptimExper = OptimizationExperiment(param_config_data)
        optimizingParameters = OptimExper.parseExperimentXML()
        
        aiParams = {}
        
        # optimization_experiment = RunOptimizatinoExperiement()
        
        experiement_arguments = []
        out_results = []
       
       
        
        # this will create a lot of directories but I won't have to worry about
        # syncroniztion and reuse of directories
        
        if param.get_descetization() == 0:
            _result = self.OptimizeParameter(options, copy.deepcopy(params_list), _ai_params_so_far)
            if _result[0] != None:
                out_results.extend(_result[0])
                i= _result[1]
        else:
            for n in range(int(param.get_descetization())):
                
                dataPath=_dataPath+"/"+str(i)
                i = i + 1
                # print dataPath
            
                recordingDirectory=dataPath+"/"+options.recordingDirectory+"/"
                frameDirectory=dataPath+"/"+options.frameDirectory+"/"
                ailogFilePath=dataPath+"/"+aiModuleName+".log"
            
                # test if any of the files/directories that are going to be used already exist
                # if they do rename them.
                timeNow = str(datetime.now()) # this will keep all of the directories renamed
                # with the same timestamp
                if True:
                    if os.path.exists(dataPath+"/"+benchmarkFileName):
                        print "Moving old benchmark log file."
                        os.rename( dataPath+"/"+benchmarkFileName, dataPath+"/"+str(timeNow)+"-"+benchmarkFileName )
                        
                    if os.path.exists(ailogFilePath):
                        print "Moving old AI log file."
                        os.rename( ailogFilePath, dataPath+"/"+str(timeNow)+"-"+aiModuleName+".log" )
                        
                    if os.path.exists(recordingDirectory):
                        print "Moving old recording files:"
                        copy_tree(os.path.abspath(recordingDirectory),
                                  os.path.abspath(recordingDirectory)+"_bak_"+str(timeNow))
                        purge(recordingDirectory, "rec[0-9]*.rec")
                    
                    # Create directories if they do not exists         
                    if not os.path.exists(recordingDirectory):
                        print "Creating directory: " + recordingDirectory
                        os.makedirs(os.path.abspath(recordingDirectory))
                        
                    if not os.path.exists(frameDirectory):
                        print "Creating directory: " + frameDirectory
                        os.makedirs(os.path.abspath(frameDirectory))
                        
                # configFile="configs/footstep-config2.xml"
                configFile="configs/"+aiModuleName+"-config.xml"
                
                # sys.exit()
            
            
                # regular need scenario module parameters
                moduleParams="scenario,scenarioAI="+aiModuleName+",useBenchmark,benchmarkTechnique="+benchmarkTechnique+",benchmarkLog="+dataPath+"/"+benchmarkFileName+",checkAgentValid,reducedGoals,fixedSpeed,fixedDirection,checkAgentRelevant,minAgents=3,ailogFileName="+ailogFilePath #,randomSeed=120"
                if options.recordSimulation:
                    moduleParams=moduleParams+",recFile=rec,recFileFolder="+recordingDirectory # save record files for later benchmarking
                    
                # moduleParams=+moduleParams+",dumpFrames,framePath="+frameDirectory #save test frames to make videos
                
                # one one of the following line should ever be included. THe operations exclude eachother.
            
                # print "number of scenarios to run will be: " + str(_numScenarios)
                moduleParams=moduleParams+",scenarioSetPath="+scenarioSetPath+",scenarioSetInitId="+str(_scenarioSetInitId)+",numScenarios="+str(_numScenarios) # can be used to load bunch of testcases
                    
                # calculate param value for this iteration
                paramValue = param.get_min() + (((param.get_max()-param.get_min())/param.get_descetization())*n)
                moduleParams=moduleParams+","+param.get_name()+"="+str(paramValue)+_ai_params_so_far
                # this section is for ppr_ai parameters
                # moduleParams=moduleParams+",ped_max_speed=10"
                # moduleParams=moduleParams+",ped_typical_speed=8"
                moduleParams=moduleParams+",dbName="+options.dbName
        
                for param_ in optimizingParameters:
                    if param_.get_type() == "float":
                        aiParams[param_.get_name()] = str(float(param_.get_original()))
                    elif param_.get_type() == "integer":
                        aiParams[param_.get_name()] = str(int(param_.get_original()))
                         
                for key, value in parseArg(_ai_params_so_far).items():
                    aiParams[key]=value
                aiParams[param.get_name()] = paramValue
                
                
                # print aiParams
                # sys.exit()
                args_dict={}
                args_dict['path_to_steersuite'] = "../build/bin/steersim"
                args_dict['steersuite_params'] = moduleParams
                args_dict['config_file_path'] = configFile
                args_dict['commandline_flag'] = commandLine
                args_dict['ai_params'] = copy.deepcopy(aiParams)
                
                experiement_arguments.append(args_dict)
                options.i=i
                # _result.append(experiement_arguments)
                print moduleParams
                _result = self.OptimizeParameter(options, copy.deepcopy(params_list), _ai_params_so_far+","+param.get_name()+"="+str(paramValue))
                # print moduleParams
                if _result[0] != None:
                    out_results.extend(_result[0])
                    i= _result[1]
                    
                # need to see if this is the last parameter to be tested
                discretization_total = 0
                for paramy in params_list:
                    discretization_total = discretization_total + paramy.get_descetization()
                if discretization_total == 0 and (n == param.get_descetization() - 1):
                    out_results.extend(experiement_arguments)
            
        
        # print "******************** module_params " + str(i)
        return [out_results, i]
        # At this point a large list of dictionary objects will have been created that can be
        # passed to an optimization_experiement object to be run
        
        # encode all of the videos
        # echo "tools/encodeVideos $frameDirectory $numScenarios $frameSize"
        # tools/encodeVideos "$frameDirectory" $numScenarios $frameSize
        
        #insert everything into database
        # python InsertStatistics.py $moduleParams $configFile

    def depentantOptimizeParemters(self, options):
        availableProcesses=int(options.processes)
        _dataPath=options.dataPath
        _scenarioSetInitId=options.scenarioSetStartId
        benchmarkFileName=options.benchmarkFileName
        benchmarkTechnique=options.benchmark
        aiModuleName=options.steeringAlgorithm
        logfileName=options.benchmarkFileName
        scenarioSetPath=options.scenarioDirectory
        frameSize=options.frameSize
        _numScenarios=int(options.numScenarios) 
        
        steersuite_semaphore = Semaphore(availableProcesses)
        
        i =0 
        
        if options.commandLine:
            commandLine="-commandline" 
        else:
            commandLine=""
             
        # ../build/bin/steersim -module $moduleParams -config $configFile
        # valgrind ../build/bin/steersim -module $moduleParams -config $configFile
        
                  
        optimization_experiement_processes = []
        
        
        param_xml_file = open(options.paramFile)
        param_config_data = param_xml_file.read()
        
        OptimExper = OptimizationExperiment(param_config_data)
        optimizingParameters = OptimExper.parseExperimentXML()
        
        aiParams = {}
        
        # optimization_experiment = RunOptimizatinoExperiement()
        
        experiement_arguments = []
        
            # this will create a lot of directories but I won't have to worry about
            # syncroniztion and reuse of directories
       
        results = self.OptimizeParameter(options, optimizingParameters)
        options.i = results[1] 
        experiement_arguments.extend(results[0])
            
        #for experiment in experiement_arguments:
        #    print experiment['steersuite_params']
        
        # At this point a large list of dictionary objects will have been created that can be
        # passed to an optimization_experiement object to be run
        
        
        # print experiement_arguments
        
        print "Number of experiments in-line: " + str(options.i)
        
        # sys.exit()
        processes_pool = Pool(processes=availableProcesses)
        
        processes_pool.map(func=RunExperiment,
                            iterable=experiement_arguments)
        print "Waited for subprocess"
        
        
        # encode all of the videos
        # echo "tools/encodeVideos $frameDirectory $numScenarios $frameSize"
        # tools/encodeVideos "$frameDirectory" $numScenarios $frameSize
        
        #insert everything into database
        # python InsertStatistics.py $moduleParams $configFile
    

if __name__ == "__main__":

    options = getOptions()

    print "Options: " + str(options)
    
    statsOptimizeParams = StatsOptimizationParameters()
    # statsOptimizeParams.depentantOptimizeParemters(options)
    # statsOptimizeParams.OptimizeParameters(options)
    statsOptimizeParams.OptimizeParameters2(options)
    