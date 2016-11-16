# from __future__ import division

from datetime import datetime
import os.path
import sys
import copy
import random
from math import sqrt
from subprocess import call
from multiprocessing import Pool
from multiprocessing import Process, Queue
import os, re
import time
import InsertStatistics
import cProfile, pstats, io
from distutils.dir_util import copy_tree
from steersuite.SteerStatsOptions import getOptions
from functools import total_ordering
from steersuite.OptimizationExperiment import OptimizationExperiment
from tools.scenarioGeneration.subspace import *
from tools.FFmpegWrapper import framesToVideo
import gc
from steersuite.RunSteerSuite import RunSteerSuite
import re

# python SteerStats.py ai_name recording_dir glframe_dir testcase_dir number_of_scenarios_to_run directory_to_store_data scenarioSetInitId

# python SteerStats.py pprAI scenarioRecordings scenarioFrames testCases 2
# python SteerStats.py pprAI scenarioRecordings scenarioFrames testCases 1724 data 2172

# clear; clear; nice -18 time python SteerStats.py --ai pprAI --checkAgentInteraction --numScenarios 10 --benchmark compositeGraph -c --db cluster_steersuitedb

# clear; clear; time nice -18 python SteerStats.py --ai pprAI --checkAgentInteraction --numScenarios 10 --benchmark compositeGraph -c --db clustering_steersuitedb --recordSimulation

# for running a set of already produced scenarios
#  clear; clear; time nice -18 python SteerStats.py --ai hidacAI --checkAgentInteraction --numScenarios 1000 --benchmark compositePLE -c --statsOnly --scenarioSetInitId 0 --scenarioDIR data/scenarios/representativeSet_Intersections/

# for running while checking for static path interactions.
# clear; clear; time nice -18 python SteerStats.py --ai pprAI --checkAgentInteraction --numScenarios 10 --benchmark compositeGraph -c --scenarioSetInitId 0 --scenarioDIR data/scenarios/representativeSet_Intersections/ --checkStaticInteractions

# Use this to have the scenario module calculate the max number of frames and for using a specific config
# clear; clear; time nice -18 python SteerStats.py --ai pprAI --checkAgentInteraction --numScenarios 1 --benchmark compositeGraph --scenarioSetInitId 0 --scenarioDIR data/scenarios/evaluations/ --checkStaticInteractions --forceConfig configs/512x512-config.xml -c --calculateMaxFrames

# Use for test of subspace generation method
# clear; clear; time nice -18 python SteerStats.py --ai sfAI --checkAgentInteraction --numScenarios 10 --benchmark compositePLE --statsOnly --scenarioSetInitId 0 --subspace=../subspaces/hallway-one-way.xml --subspaceParams="(2.2;3.0)"
point_pattern = re.compile('p[0-9]+_')
class SteerStats():
    
    def __init__(self, options):
        self._options=options
        self._ai_param_names=[]
        self._results=None
        self._insert_func=None
        
    def set_penalty_func(self, func):
        self._penalty_func = func
        
    def set_insert_func(self, func):
        self._insert_func = func
        
    def set_log_File(self, _file):
        self._log_file=_file
        
    def close_data_log(self):
            self._log_file.close()
    
    def getBoundObjective(self, objective):
        import inspect
        methods = inspect.getmembers(self, predicate=inspect.ismethod)
# print methods
        for method in methods:
            if method[0] == objective:
                return method[1]
               
	def getBoundPenaltyFunc(self, objective):
		import inspect
		import optimization
        methods = inspect.getmembers(optimization, predicate=inspect.ismethod)
# print methods
        for method in methods:
            if method[0] == objective:
                return method[1]
        
    def log(self, data):
        """
            data should be a list that will create one comma delimited line
            in the log file
        """
        out=''
        for item in data:
            out = out + str(item) + ','
            
        out = out[:-1] + '\n'
        self._log_file.write(out)
        
        
        
    def generate_subspace(self, subspace, subspaceParams, dir, samples):
        # subspaceParams
        scenarios = hallway_one_way(subspaceParams, samples)
        i=0
        for scenario in scenarios:
            # print subspaceParams
            file=dir+"testcase-"+str(i)+".xml"
            # print "**** saving file to " + file
            scenarioFile = open(file,"w")
            scenarioFile.write(scenario.getTextPretty())
            scenarioFile.close()
            i=i+1
  
           
    def RunStats(self, aiParams=None, paramNames=None, options=None):
        
        timeNow = str(datetime.now().strftime('%Y-%m-%d-%H-%M-%S.%f'))
        time.sleep(0.00001) 
        if paramNames:
            self.setParamNameDict(paramNames)
        
        if options is None:
            options = self._options
        availableProcesses=int(options.processes)
        _dataPath=options.dataPath
        _scenarioSetInitId=int(options.scenarioSetStartId)
        benchmarkFileName=options.benchmarkFileName+'.log'
        benchmarkTechnique=options.benchmark
        aiModuleName=options.steeringAlgorithm
        logfileName=options.benchmarkFileName
        scenarioSetPath=options.scenarioDirectory
        frameSize=options.frameSize
        _numScenarios=int(options.numScenarios) 
        parameterOptimization = options.paramFile
        frameDirectorys=[]
        
        outdata = []
        
        if options.commandLine:
            commandLine="-commandline" 
        else:
            commandLine=""
             
        # ../build/bin/steersim -module $moduleParams -config $configFile
        # valgrind ../build/bin/steersim -module $moduleParams -config $configFile

        def purge(dir, pattern):
            for f in os.listdir(dir):
                if re.search(pattern, f):
                    os.remove(os.path.join(dir, f))
                    
        steersimProcesses = []
        insertStatProcesses = []
        
        statsQueue = Queue()
        # statsQueue = None
        for i in range(0,availableProcesses):
        
            dataPath=_dataPath+str(i)
            recordingDirectory=dataPath+"/"+options.recordingDirectory+"/"
            frameDirectory=dataPath+"/"+options.frameDirectory+"/"
            tmp_frameDirectory=frameDirectory+'scenario'+str(i)+'/'
            ailogFilePath=dataPath+"/"+aiModuleName+".log"
            testCasePath=dataPath+"/"+options.testcaseDirectory+"/"
            # test if any of the files/directories that are going to be used already exist
            # if they do rename them.
            # with the same timestamp
            if not options.noSimulation:
                if os.path.exists(dataPath):
                    if options.verbose :
                        print "Moving old log information."
                    os.rename( dataPath, dataPath+"_"+timeNow )
                
                # Create directories if they do not exists         
                if not os.path.exists(recordingDirectory):
                    if options.verbose :
                        print "Creating directory: " + recordingDirectory
                    os.makedirs(os.path.abspath(recordingDirectory))
                    
                if not os.path.exists(tmp_frameDirectory):
                    if options.verbose :
                        print "Creating directory: " + tmp_frameDirectory
                    os.makedirs(os.path.abspath(tmp_frameDirectory))
                    
                if not os.path.exists(testCasePath):
                    if options.verbose :
                        print "Creating directory: " + testCasePath
                    os.makedirs(os.path.abspath(testCasePath))
        
            # configFile="configs/footstep-config2.xml"
            if options.forceConfig != "":
                configFile=options.forceConfig
            elif options.configFile:
                configFile=options.configFile
            else:
                configFile="configs/"+aiModuleName+"-config.xml"
            # sys.exit()
        
        
            # regular need scenario module parameters
            moduleParams="scenario,scenarioAI="+aiModuleName+",useBenchmark,benchmarkTechnique="+benchmarkTechnique+",benchmarkLog="+dataPath+"/"+benchmarkFileName+",checkAgentValid,reducedGoals,fixedSpeed,checkAgentRelevant,minAgents=3,ailogFileName="+ailogFilePath #,randomSeed=120"
            if options.numFrames != None:
                moduleParams=moduleParams+",maxFrames="+str(options.numFrames)
            if options.scenarioFilePrefix != "":
                moduleParams=moduleParams+",scenarioPrefix="+str(options.scenarioFilePrefix)
            if options.checkStaticPathInteractions:
                moduleParams=moduleParams+",checkStaticPathInteractions"
            if options.calculateMaxFrames:
                moduleParams=moduleParams+",calculateMaxFrames"
            if options.scenarioTimeThreshold != "":
                moduleParams=moduleParams+",scenarioTimeThreshold="+options.scenarioTimeThreshold
            if options.maxNodesToExpandForSearch != "":
                moduleParams=moduleParams+",maxNodesToExpandForSearch="+options.maxNodesToExpandForSearch
            if options.recordSimulation:
                moduleParams=moduleParams+",recFile=rec,recFileFolder="+recordingDirectory # save record files for later benchmarking
            if options.saveFrames:
                moduleParams=moduleParams+",dumpFrames,framePath="+frameDirectory #save test frames to make videos
            if options.makeVideo:
                frameDirectorys.append(tmp_frameDirectory)
            if options.checkAgentInteraction:
                moduleParams=moduleParams+",checkAgentInteraction"
                if options.checkAgentInteractionRadius > 0:
                    moduleParams=moduleParams+",checkAgentInteractionRadius="+str(options.checkAgentInteractionRadius)
            # moduleParams=moduleParams+",contextID=0"
            moduleParams=moduleParams+",egocentric" # puts the reference agent right in the middle of the scenario
            
            if options.realDataFileName != "":
                moduleParams=moduleParams+",RealDataName="+options.realDataFileName
                
            scenarioSetInitId = _scenarioSetInitId + (_numScenarios/availableProcesses)*i
                
            if ( i == (availableProcesses-1)): # this cases handles where numScenario % availableProcesses != 0
                numScenarios = (_numScenarios/availableProcesses +
                                 (_numScenarios - ((_numScenarios/availableProcesses)*availableProcesses)))
            else:
                numScenarios = _numScenarios/availableProcesses
            # only one of the following lines should ever be included. THe operations exclude eachother.
            if options.dumpTestcases == True:
                moduleParams=moduleParams+",dumpTestCases,testCasePath="+testCasePath # part for dumping testcases
            if options.subspace != "":
                if options.verbose :
                    print "number of scenarios to run will be: " + str(numScenarios)
                    print "with subspace params " + str(options.subspaceParams) 
                # Need to have a random seed to generate subspace properly each time.
                if options.subspaceObsType == 'circle':
                    moduleParams=moduleParams+",randomSeed="+str(options.randomSeed)+",numScenarios="+str(numScenarios)+',subspace='+options.subspace+',subspaceParams='
                elif options.subspaceObsType == 'square':
                    moduleParams=moduleParams+",randomSeed="+str(options.randomSeed)+",numScenarios="+str(numScenarios)+',subspace='+options.subspace+',subspaceWallParams='
                if isinstance(options.subspaceParams, basestring):
                    moduleParams=moduleParams+str(options.subspaceParams) 
                else:
                    moduleParams=moduleParams+str(numpy_array_to_params(options.subspaceParams)) # can be used to load bunch of testcases
            elif options.randScenarios:
                moduleParams=moduleParams+",randomSeed="+str(options.randomSeed)+",numScenarios="+str(numScenarios)+",scenarioSetInitId="+str(scenarioSetInitId)
            else:
                # math to compute the scenarioSetId depending on the number of available processes
                # i.e. where to start
                # if options.verbose :
                #    print "number of scenarios to run will be: " + str(numScenarios)
                moduleParams=moduleParams+",scenarioSetPath="+scenarioSetPath+",scenarioSetInitId="+str(scenarioSetInitId)+",numScenarios="+str(numScenarios) # can be used to load bunch of testcases
                
            
            # this section is for ppr_ai parameters
            # moduleParams=moduleParams+",ped_max_speed=10"
            # moduleParams=moduleParams+",ped_typical_speed=8"
            # adds the name of the database to use
            # default is steersuitedb
            moduleParams=moduleParams+",dbName="+options.dbName
            if options.skipInsert:
                moduleParams=moduleParams+",skipInsert=True"

                
            # update the optimization parameter values for this execution
            aiConfigParams={}
            if (aiParams != None):
                assert len(aiParams) == len(self._ai_param_names), ("the param names for " + str(aiParams) + 
                            " do not match for " + str(self._ai_param_names) + "; " + str(len(aiParams)) + 
                            " != " + str(len(self._ai_param_names ))) 
                for param_name,param_value in zip(self._ai_param_names,aiParams):
                    moduleParams=moduleParams+"," + str(param_name) + "=" + str(param_value)
                    aiConfigParams[param_name] = param_value
            
            if options.verbose :
                print moduleParams
            if not options.noSimulation: 
                # Maybe we don't want to simulate
                steersimProcesses.append(Process(target=RunSteerSuite, args=(moduleParams, configFile, commandLine,)))
            if self._insert_func:
                # ugly solution for now but does the trick
                # For StatsSampleParameters so that the config is inserted properly
                insertStatProcesses.append(Process(target=self._insert_func, args=(moduleParams, configFile, aiConfigParams)))
                statsQueue=None
            else:
                insertStatProcesses.append(Process(target=InsertStatistics.InsertStatistics, args=(moduleParams, configFile, statsQueue)))
        
        if options.logMetrics:
            # There should be atleast 0
            statsDataFilePath=dataPath+"/statsData.log"
            self.set_log_File(open(statsDataFilePath, 'w+'))
        
        
        for proc in steersimProcesses:
            proc.start()
            time.sleep(0.01)
        
        for proc in steersimProcesses:
            proc.join()
        
        # after all of the processes have finished go through them one at a time and
        # insert them into the database
        
            
        if statsQueue:
            for proc in insertStatProcesses:
                proc.start()
            for proc in insertStatProcesses:
                outdata.extend(statsQueue.get())
            for proc in insertStatProcesses:
                proc.join()
        else:
            for proc in insertStatProcesses:
                proc.start()
                proc.join()

        if options.verbose :        
            print "Waited for subprocess"
            
        if options.makeVideo:
            print "Video rendering not implemented yet."
            try:
                for dir in frameDirectorys:
                    print "frame directory: " + str(dir)
                    print "Rendering video:"
                    v_file = framesToVideo(dir, 1200, 800, str(dir)+'video')
                    # print "basename " + str(os.path.basename(v_file))
                    # print "basename2 " + str(os.path.dirname(dir))
                    # os.rename( v_file,  dir+'../'+str(os.path.basename(v_file)))
                    pattern='^(frame\d+)\.ppm$'
                    for f in os.listdir(dir):
                        if re.search(pattern, f):
                            os.remove(os.path.join(dir, f))
            except Exception as inst:
                print inst
                
                
        
        if options.noReturn is not True:
            # print outdata
            return outdata
    
    
        
        # encode all of the videos
        # echo "tools/encodeVideos $frameDirectory $numScenarios $frameSize"
        # tools/encodeVideos "$frameDirectory" $numScenarios $frameSize
        
        #insert everything into database
        # python InsertStatistics.py $moduleParams $configFile

    def simulationTimeMetric(self, ai_params, results=None, options=None):
        if self._options.verbose:
            print ai_params
        if results == None:
            results = self.RunStats(ai_params,options=options)
        
        
        time_sum = float(0)
        time_Opt_sum = float(0)
        # print "The length of the results is: " + str(len(results))
        # print str(results[0])
        i=0
        for result in results:
            # if int(result.get_agent_complete()) == 1:
            # print "Agent success is " + str(result.get_agent_success())
            if int(result.get_agent_success()) == 1:
                # this is time the egocentric agent is enabled
                # print "time enabled: " + str(float(result.get_total_time_enabled()))
                time_sum = time_sum + (float(result.get_optimal_time())/float(result.get_total_time_enabled()))
                # with respect to desired speed = 1.33f, calculated with respect to optimal distance
                # time_Opt_sum = time_Opt_sum + float(result.get_optimal_time())
                i=i+1
        
        # this is used in the case no scenarios are passed.
        # the math is still solid but I don't want to divide by 0.
        """if time_Opt_sum != 0:
            # most likely no scenarios passed
            simulation_time_ratio = 1 - (time_Opt_sum/ time_sum)
        else:
            simulation_time_ratio = 1
            """
        simulation_time_ratio = 1 - (time_sum/float(self._options.numScenarios))
        print "original simulation_time_sum: " + str(time_sum) + " with " + str(i) + " scenerios passed"
        # if simulation_time_ratio < 0:
            # The simulation was very good
            # simulation_time_ratio = 0
        print "*** evaluation result for simulatinoTime: " + str(simulation_time_ratio)
        return simulation_time_ratio

    def pleMetric(self, ai_params, results=None, options=None):
        if self._options.verbose:
            print ai_params
        subpars = list()
        if results == None:
            if self._options.subspaceParams == '':
                for param_name,param_value in zip(self._ai_param_names,ai_params):
                    if point_pattern.match(param_name) == None:
                        continue
                    subpars.append(param_value)
                options.subspaceParams = subpars
            results = self.RunStats(ai_params,options=options)
        
        
        ple_sum = float(0)
        ple_opt_sum = float(0)
        
        # print "The length of the results is: " + str(len(results))
        # print str(results[0])
        i=0
        for result in results:
            # if int(result.get_agent_complete()) == 1:
            if int(result.get_agent_success()) == 1:
                ple_sum = ple_sum + (float(result.get_ple_energy_optimal())/float(result.get_ple_energy()))
                # ple_sum = ple_sum + float(result.get_ple_energy())
                # ple_opt_sum = ple_opt_sum + float(result.get_ple_energy_optimal())
                i=i+1
        
        print "PLE sum is: " + str(ple_sum) + " with " + str(i) + " scenerios passed"
        """
        if ple_opt_sum != 0:
            ple_energy = 1 - (ple_opt_sum/ ple_sum)
        else:
            ple_energy = 1
            """
        ple_energy = 1 - (ple_sum/float(self._options.numScenarios))
        print "*** evaluation result for PLE: " + str(ple_energy)
        return ple_energy
    
    def distanceMetric(self, ai_params, results=None, options=None):
        """
        There is a real issue with this metric in that it might discourage 
        passing scenarios. 
        It might not because it still has to do well in the
        scenarios that is passes
        """
        if results == None:
            results = self.RunStats(ai_params,options=options)
        
        num_scenarios = 0 # do not rely on experiment completing the scenarios it is supposed to
        total_distance_traveled = 0
        total_optimal_distance_traveled = 0
        distance_sum = 0
        
        # print "The length of the results is: " + str(len(results))
        for result in results:
            if int(result.get_agent_success()) == 1:
                num_scenarios = num_scenarios + 1
                distance_sum = distance_sum + (float(result.get_optimal_path_length())/float(result.get_total_distance_traveled()))
                # total_distance_traveled = total_distance_traveled + float(result.get_total_distance_traveled())
                # total_optimal_distance_traveled = total_optimal_distance_traveled + float(result.get_optimal_path_length())
        
            
        print "distance_sum: " + str( distance_sum )
        """
        if total_optimal_distance_traveled != 0:
            # most likely no scenarios passed
            distance_ratio = (1- (total_optimal_distance_traveled/ total_distance_traveled))
        else:
            distance_ratio = 1
        
        print "Distance ratio: " + str(distance_ratio)
        """
        
        fun_value = 1 - (distance_sum/float(self._options.numScenarios))
        #if fun_value < 0:
        #    fun_value = 0
        print "*** evaluation result for distance: " + str(fun_value)
        return fun_value
    
    def distanceMetricGlobal(self, ai_params, results=None, options=None):
        """
        There is a real issue with this metric in that it might discourage 
        passing scenarios. 
        It might not because it still has to do well in the
        scenarios that is passes
        """
        if results == None:
            options.subspaceParams = ai_params
            results = self.RunStats(ai_params,options=options)
        
        num_scenarios = 0 # do not rely on experiment completing the scenarios it is supposed to
        distance_sum = 0
        distance_sum_tmp = 0
        
        # print "agent paths: " + str(results_list[0].get_agent_path_lengths())
        # print "agent times: " + str(results_list[0].get_agent_time_enableds())
        # print "agent distances: " + str(results_list[0].get_agent_distance_traveleds())
        # print "The length of the results is: " + str(len(results))
        for result in results:
            num_scenarios = num_scenarios + 1
            for (optimal_path, distance, complete) in zip(result.get_agent_path_lengths(), result.get_agent_distance_traveleds(), result.get_agent_completes()):
                # print "path length: " + str(path) +" distance " + str(distance) 
                if complete == "1" or complete == 1:
                    distance_sum_tmp = distance_sum_tmp + (float(optimal_path)/float(distance))
                    
                # total_distance_traveled = total_distance_traveled + float(result.get_total_distance_traveled())
                # total_optimal_distance_traveled = total_optimal_distance_traveled + float(result.get_optimal_path_length())
            distance_sum = distance_sum + (distance_sum_tmp/float(result.get_num_agents()))
            distance_sum_tmp=0
        
            
        print "distance_sum: " + str( distance_sum )
        
        fun_value = 1 - (distance_sum/float(self._options.numScenarios))
        #if fun_value < 0:
        #    fun_value = 0
        print "*** evaluation result for distanceGlobal: " + str(fun_value)
        return fun_value
    
    def timeMetricGlobal(self, ai_params, results=None, options=None):
        """
        There is a real issue with this metric in that it might discourage 
        passing scenarios. 
        It might not because it still has to do well in the
        scenarios that is passes
        """
        if results == None:
            options.subspaceParams = ai_params
            results = self.RunStats(ai_params,options=options)
        
        num_scenarios = 0 # do not rely on experiment completing the scenarios it is supposed to
        time_sum = 0
        time_sum_tmp = 0
        
        # needed to adjust optimal path to get optimal_time 
        perfered_speed = 1.33
        # print "results: " + str(results)
        
        # print "agent paths: " + str(results_list[0].get_agent_path_lengths())
        # print "agent times: " + str(results_list[0].get_agent_time_enableds())
        # print "agent distances: " + str(results_list[0].get_agent_distance_traveleds())
        # print "The length of the results is: " + str(len(results))
        for result in results:
            num_scenarios = num_scenarios + 1
            for (optimal_path, time_enabled, complete) in zip(result.get_agent_path_lengths(), result.get_agent_time_enableds(), result.get_agent_completes()):
                # print "complete: " + str(complete) +" time_enabled " + str(time_enabled)  
                if complete == "1" or complete == 1:
                    optimal_time = float(optimal_path) / perfered_speed
                    time_sum_tmp = time_sum_tmp + (float(optimal_time)/(float(time_enabled)))
                    
                # total_distance_traveled = total_distance_traveled + float(result.get_total_distance_traveled())
                # total_optimal_distance_traveled = total_optimal_distance_traveled + float(result.get_optimal_path_length())
            time_sum = time_sum + (time_sum_tmp/float(result.get_num_agents()))
            time_sum_tmp=0
        
            
        print "time_sum: " + str( time_sum )
        
        fun_value = 1 - (time_sum/float(self._options.numScenarios))
        #if fun_value < 0:
        #    fun_value = 0
        print "*** evaluation result for timeGlobal: " + str(fun_value)
        return fun_value
    
    def agentFlowMetricGlobal(self, ai_params, results=None, options=None):
        subpars = list()
        if results == None:
            if self._options.subspaceParams == '':
                for param_name,param_value in zip(self._ai_param_names,ai_params):
                    if point_pattern.match(param_name) == None:
                        continue
                    subpars.append(param_value)
                options.subspaceParams = subpars
            results = self.RunStats(ai_params,options=options)
        
        num_scenarios = 0 # do not rely on experiment completing the scenarios it is supposed to
        time_sum_tmp = 0
        agent_flow_total=0.0        
        fps = 20.0 # Configureation.getFPS()
        u=1.0

        for result in results:
            num_scenarios = num_scenarios + 1
            # print "Time enableds" + str(result.get_agent_time_enableds())
            # print "Min time enabled" + str(min(result.get_agent_time_enableds()))
            num_agents=0
            for (time_enabled, complete) in zip(result.get_agent_time_enableds(), result.get_agent_completes()):
                # print "complete: " + str(complete) +" time_enabled " + str(time_enabled)
                num_agents+=1  
                if complete == "1" or complete == 1:
                    # time_sum_tmp = time_sum_tmp + ((float(time_enabled) - float(min_time)))
                    time_sum_tmp = time_sum_tmp + ((float(time_enabled)))
                else:
                    # worst time possible
                    time_sum_tmp = time_sum_tmp + ((float(self._options.numFrames) / fps)*u)
                    
            #Adding the penalty time for the Agents not completed which is (TOTAL_AGENTS - AGENTS_COMPLETED) * AVG_COMPUTED_TIME_FOR_2000_FRAMES
            time_avg = time_sum_tmp / float(result.get_num_agents())
            # agent_flow =  time_sum / float(num_agents_completed)
            num_agents_completed = sum(result.get_agent_completes())
            # sim_crash_penalty = (result.get_num_agents() - num_agents) * ((float(self._options.numFrames) / fps)*u)
            agent_flow =  (float(num_agents_completed) / time_avg)
            agent_flow_total = agent_flow_total + agent_flow
            time_sum_tmp=0.0
        
            
        # print "Agent flow time sum: " + str( agent_flow_total )
        
        # Helps penalize the system if for some reason the system crashes.
        
        # scenario_crash_penalty = ( float(self._options.numScenarios) - num_scenarios) * 100.0 * ((float(self._options.numFrames) / fps)*u)
        fun_value = ((agent_flow_total) /float(self._options.numScenarios))
        #if fun_value < 0:
        #    fun_value = 0
        print "*** evaluation result for agent flow: " + str(fun_value * -1.0)
        return (fun_value * -1.0)
    
    def simulationTimeMetricGlobal(self, ai_params, results=None, options=None):
        if results == None:
            options.subspaceParams = ai_params
            results = self.RunStats(ai_params,options=options)
        
        num_scenarios = 0 # do not rely on experiment completing the scenarios it is supposed to
        time_sum = 0
        time_sum_tmp = 0        

        for result in results:
            num_scenarios = num_scenarios + 1
            for (time_enabled, complete) in zip(result.get_agent_time_enableds(), result.get_agent_completes()):
                # print "complete: " + str(complete) +" time_enabled " + str(time_enabled)  
                if complete == "1" or complete == 1:
                    time_sum_tmp = time_sum_tmp + ((float(time_enabled)))
                    
                # total_distance_traveled = total_distance_traveled + float(result.get_total_distance_traveled())
                # total_optimal_distance_traveled = total_optimal_distance_traveled + float(result.get_optimal_path_length())
            
            #Adding the penalty time for the Agents not completed which is (TOTAL_AGENTS - AGENTS_COMPLETED) * AVG_COMPUTED_TIME_FOR_2000_FRAMES
            
            time_frames_completion_2000 = 52.5 #average time in seconds computed by running multiple simulations of 2000 frames
            num_agents_completed = sum(result.get_agent_completes())
            if result.get_num_agents() != num_agents_completed:
                time_sum = float(result.get_num_agents() - num_agents_completed) * time_frames_completion_2000
            else:
                time_sum = time_sum_tmp / float(result.get_num_agents()) 
            time_sum_tmp=0            
            # time_sum = time_sum + (time_sum_tmp/float(result.get_num_agents()))
            # time_sum_tmp=0
        
            
        print "Average Simulation time sum: " + str( time_sum )
        
        # Helps penalize the system if for some reason the system crashes.
        fun_value = (time_sum/float(self._options.numScenarios))
        #if fun_value < 0:
        #    fun_value = 0
        print "*** evaluation result for simulationTimeGlobal: " + str(fun_value)
        return fun_value
    
    def pleMetricGlobal(self, ai_params, results=None, options=None):
        """
        There is a real issue with this metric in that it might discourage 
        passing scenarios. 
        It might not because it still has to do well in the
        scenarios that is passes
        
        computing pleEnergyOptimal = 2 m L sqrt (E_S * E_W )
        """
        if results == None:
            if self._options.subspaceParams == '':
                options.subspaceParams = ai_params
            results = self.RunStats(ai_params,options=options)
        
        num_scenarios = 0 # do not rely on experiment completing the scenarios it is supposed to
        ple_sum = 0
        ple_sum_tmp = 0
        
        # needed to adjust optimal path to get optimal_time 
        E_S=2.23
        E_W=1.26
        MASS=1
        
        # print "agent paths: " + str(results_list[0].get_agent_path_lengths())
        # print "agent times: " + str(results_list[0].get_agent_time_enableds())
        # print "agent distances: " + str(results_list[0].get_agent_distance_traveleds())
        # print "The length of the results is: " + str(len(results))
        for result in results:
            num_scenarios = num_scenarios + 1
            for (optimal_path, ple_energy, complete) in zip(result.get_agent_path_lengths(), result.get_agent_ple_energys(), result.get_agent_completes()):
                # print "path length: " + str(path) +" distance " + str(distance) 
                if complete == "1" or complete == 1:
                    optimal_ple = float(2) * MASS * float(optimal_path) * sqrt(E_S * E_W )
                    ple_sum_tmp = ple_sum_tmp + (float(optimal_ple)/(float(ple_energy)))
                    
                # total_ple_traveled = total_ple_traveled + float(result.get_total_distance_traveled())
                # total_optimal_distance_traveled = total_optimal_distance_traveled + float(result.get_optimal_path_length())
            ple_sum = ple_sum + (ple_sum_tmp/float(result.get_num_agents()))
            ple_sum_tmp=0
        
            
        print "ple_sum: " + str( ple_sum )
        
        fun_value = 1 - (ple_sum/float(self._options.numScenarios))
        #if fun_value < 0:
        #    fun_value = 0
        print "*** evaluation result for pleGlobal: " + str(fun_value)
        return fun_value
    
    def entropyMetric(self, ai_params, results=None, options=None):
        """
            This is the Entropy metric...
            Example command
            nice -4 python SteerStats.py --ai=rvo2dAI --checkAgentInteraction --numScenarios=1 -c --scenarioSetInitId=0 --scenarioDIR=data/scenarios/representativeSet_Intersections/ --dataDir=data/ --paramFile xml/config/ppr-param-config.xml --statsOnly --processes=2 --benchmark compositeEntropy --metric=varopt --metricWeights=simulationTime=0,ple=0,coverage=0,distance=0,computation=0,entropy=1 --aiParams=2.6,14,1.7,1.31,1.15,1,0.77,0.5,1.9,0.16,0.55,0.26,0.95,1.5,10,0.94,0.99,-0.95,-0.95,0.55,8,0.8,4,5,1.1,0.3,0.3,0.05,0.15,0.9,0.1,3,0.4,0.5,0.2,20,50,20 --maxFrames=2000 --RealDataName=data/RealWorldData/uo-050-180-180_combined_MB.txt
            or
            nice -4 python SteerStats.py --ai=sfAI --checkAgentInteraction --numScenarios=1 -c --scenarioSetInitId=0 --scenarioDIR=data/RealWorldData/ou-060-180-180/ --dataDir=data/ --paramFile xml/config/sf-param-config-multi.xml --statsOnly --processes=1 --benchmark compositeEntropy --metric=varopt --metricWeights=simulationTime=0,ple=0,coverage=0,distance=0,computation=0,entropy=1 --aiParams=0.5,0.3,0.3,1500.0,1500.0,3000.0,0.08,25.0,0.08,25.0 --maxFrames=2000 --RealDataName=data/RealWorldData/ou-060-180-180/uo-060-180-180_combined_MB.txt -v
            nice -4 python SteerStats.py --ai=sfAI --checkAgentInteraction --numScenarios=1 -c --scenarioSetInitId=0 --scenarioDIR=data/RealWorldData/b140_combined/ --dataDir=data/ --paramFile xml/config/sf-param-config-multi.xml --statsOnly --processes=1 --benchmark compositeEntropy --metric=varopt --metricWeights=simulationTime=0,ple=0,coverage=0,distance=0,computation=0,entropy=1 --aiParams=0.5,0.3,0.3,1500.0,1500.0,3000.0,0.08,25.0,0.08,25.0 --maxFrames=2000 --RealDataName=data/RealWorldData/b140_combined/b140_combined.txt
            Newer uses realWorlDataDir instead
            nice -4 python SteerStats.py --ai=sfAI --checkAgentInteraction --numScenarios=1 --scenarioSetInitId=0 --scenarioDIR=data/scenarios/customSet/bottleneck-hallway/ --dataDir=data/ --paramFile xml/config/sf-param-config-multi.xml --statsOnly --processes=1 --benchmark compositePLE --metric=varopt --metricWeights=simulationTime=0,ple=1,coverage=0,distance=0,computation=0,entropy=1 --aiParams=0.5,0.3,0.3,1500.0,1500.0,3000.0,0.08,25.0,0.08,25.0 -v --numFrames 2500 -c --RealDataDir=data/RealWorldData/b140_combined/
        """
        results=None
        if self._options.verbose:
            print ai_params
        if results == None:
            numEntropyScenarios=1
            options = copy.deepcopy(self._options)
            options.scenarioSetStartId=0
            options.scenarioDirectory=options.realDataDirName
            options.realDataFileName=options.realDataDirName+"/testcase-0.txt"
            options.benchmark="compositeEntropy"
            options.numScenarios=numEntropyScenarios
            options.forceConfig="configs/Entropy-config.xml"
            if int(options.processes) > numEntropyScenarios:
                options.processes=numEntropyScenarios
            
            # sets up a new SteerStats object with the altered settings
            try:
                otherE = SteerStats(options)
                otherE.setParamNameDict(self._ai_param_names)
                results = otherE.RunStats(ai_params,options=options)
            except Exception as inst:
                return 10.0
                
        # print results
        try:
            
            entropyResult = float(results[0].get_score())
        except IndexError as err:
            print "found index error"
            entropyResult=10.0
        
        print "Entropy: " + str(entropyResult)
        # fun_value = (float(entropyResult)/(float(3)*0.8)) - 1
        fun_value = (float(entropyResult))
        print "*** evaluation result for Entropy: " + str(fun_value)
        return fun_value
    
    def coverageMetric(self, ai_params, results=None, options=None):
    
        if results == None:
            results = self.RunStats(ai_params,options=options)
        
        # print ai_params
    
        scenarios_passed = 0
        
        print "The length of the results is: " + str(len(results))
        for result in results:
            scenarios_passed = scenarios_passed + int(result.get_agent_success())
        
            
        # options number of scenarios is used encase of simulation problems
        print "Number of scenarios passed " + str(scenarios_passed)
        scenarios_passed_value = ((scenarios_passed))/float(self._options.numScenarios)
        
        # scenarios_failed = 1 - ((scenarios_passed) / float(self._options.numScenarios))
        
        print "percentage scenarios passed: " + str(scenarios_passed_value)
        
        fun_value = ((scenarios_passed_value))
        print "*** evaluation result for coverage: " + str(fun_value)
        return fun_value
       
    def inverseCoverageMetric(self, ai_params, results=None, options=None):
    
        if results == None:
            results = self.RunStats(ai_params,options=options)
        
        # print ai_params
    
        scenarios_passed = 0
        
        print "The length of the results is: " + str(len(results))
        for result in results:
            scenarios_passed = scenarios_passed + int(result.get_agent_success())
        
            
        # options number of scenarios is used encase of simulation problems
        print "Number of scenarios passed " + str(scenarios_passed)
        scenarios_passed_value = ((float(self._options.numScenarios) - scenarios_passed))/float(self._options.numScenarios)
        
        # scenarios_failed = 1 - ((scenarios_passed) / float(self._options.numScenarios))
        
        print "percentage scenarios failed: " + str(scenarios_passed_value)
        
        fun_value = ((scenarios_passed_value))
        print "*** evaluation result for coverage: " + str(fun_value)
        return fun_value
    
    def completedMetric(self, ai_params, results=None, options=None):
    
        if results == None:
            results = self.RunStats(ai_params,options=options)
        
        # print ai_params
    
        scenarios_passed = 0
        
        print "The length of the results is: " + str(len(results))
        for result in results:
            scenarios_passed = scenarios_passed + int(result.get_agent_complete())
        
            
        # options number of scenarios is used encase of simulation problems
        print "Number of scenarios completed " + str(scenarios_passed)
        scenarios_passed_value = ((float(self._options.numScenarios) - scenarios_passed))/float(self._options.numScenarios)
        
        # scenarios_failed = 1 - ((scenarios_passed) / float(self._options.numScenarios))
        
        print "percentage scenarios not completed: " + str(scenarios_passed_value)
        
        fun_value = ((scenarios_passed_value))
        print "*** evaluation result for completed: " + str(fun_value)
        return fun_value
    
    def completedAvgMetric(self, ai_params, results=None, options=None):
    
        if results == None:
            results = self.RunStats(ai_params,options=options)
        
        # print ai_params
    
        scenarios_passed = 0
        completed_percent_total = float(0)
        total_num_agents = 0
        sum_agent_completes = 0
        
        print "The length of the results is: " + str(len(results))
        for result in results:
            sum_agent_completes = sum_agent_completes + sum (result.get_agent_completes())
            total_num_agents = total_num_agents + (result.get_num_agents())  
        
            
        # options number of scenarios is used encase of simulation problems
        print "Total number of agents: " + str(total_num_agents)
        print "Sum of agent completes: " + str(sum_agent_completes)
        averge_agents_complete = sum_agent_completes/float(total_num_agents)
        print "average number of agent completed per scenario completed " + str(averge_agents_complete)
        
        # scenarios_failed = 1 - ((scenarios_passed) / float(self._options.numScenarios))
        
        
        fun_value = ((averge_agents_complete))
        print "*** evaluation result for completed: " + str(fun_value)
        return fun_value
       
    def findInterestingCases(self, ai_params, results=None, options=None):
    
        if results == None:
            results = self.RunStats(ai_params,options=options)
        
        # print ai_params
    
        scenarios_passed = 0
        completed_percent_total = float(0)
        total_num_agents = 0
        sum_agent_completes = 0
        scenario_num = 0
        
        print "The length of the results is: " + str(len(results))
        for result in results:
			if all(result.get_agent_distance_traveleds()) and not all(result.get_agent_completes()):
				print "scenario number: " + str(scenario_num)
        		
			scenario_num = scenario_num + 1 
			sum_agent_completes = sum_agent_completes + sum(result.get_agent_completes())
			total_num_agents = total_num_agents + (result.get_num_agents())  
        
            
        # options number of scenarios is used encase of simulation problems
        print "Total number of agents: " + str(total_num_agents)
        print "Sum of agent completes: " + str(sum_agent_completes)
        averge_agents_complete = sum_agent_completes/float(total_num_agents)
        print "average number of agent completed per scenario completed " + str(averge_agents_complete)
        
        # scenarios_failed = 1 - ((scenarios_passed) / float(self._options.numScenarios))
        
        
        fun_value = ((averge_agents_complete))
        print "*** evaluation result for completed: " + str(fun_value)
        return fun_value
    
    def computationMetric2(self, ai_params, results=None, options=None):
    
        if results == None:
            results = self.RunStats(ai_params,options=options)
        
        print ai_params
    
        computation_time = 0
        
        print "The length of the results is: " + str(len(results))
        for result in results:
            computation_time = computation_time + int(result['total_ticks_accumulated'])
        
            
        print "total_computation time: " + str(computation_time)
        
        fun_value = ((computation_time))
        print "*** evaluation result for computation metric 2: " + str(fun_value)
        return fun_value
    
    def performanceMetric(self, ai_params, results=None, options=None):
    
    
        """ max time to update AI for 1 frame = 10/100 * 1/30 = 0.0033 seconds
            max time to update AI for 1 frame for 1 agents, T_max = 0.0033 / N seconds 
            
            Let T_avg = average time to update AI / frame / agent 
            
            Normalized performance metric m_p = 
            1 if T_avg < T_max 
            T_avg / T_max else """
        if results == None:
            results = self.RunStats(ai_params,options=options)
        
        average_performance_time=0
        # print ai_params
        #  
        # T_max=0.001 # 100fps
        T_max=0.0033 # 30 fps
        performance_time = 0
        
        # print "The length of the results is: " + str(len(results))
        for result in results:
            # print result
            performance_time = (performance_time + 
                            ((T_max)/float(result.get_num_agents()))/(float(result.get_average_time_per_call()) / 
                float(result.get_num_agents())))

        average_performance_time=performance_time/float(self._options.numScenarios)
        
        print "average performance time: " + str(average_performance_time)
        
        fun_value = 1 - ((average_performance_time))
        
        #if fun_value < 0:
        #    fun_value = 0
        print "*** evaluation result for performance: " + str(fun_value)
        return fun_value
    
    def framesMetric(self, ai_params, results=None, options=None):
    
        if results == None:
            results = self.RunStats(ai_params,options=options)
        
        total_frames = 0
        num_scenarios = 0 # do not rely on experiment completeing the scenarios it is supposed to
        
        # print "The length of the results is: " + str(len(results))
        for result in results:
            num_scenarios = num_scenarios + 1
            total_frames = total_frames + float(result['frames'])
        
            
        frames_percentage = (total_frames / ( num_scenarios * self._options.numFrames))
        
        print "percentage of total frames: " + str(frames_percentage)
        
        
        fun_value = frames_percentage
        print "*** evaluation result for frames: " + str(fun_value)
        return fun_value
    
    def collisionsMetric(self, ai_params, results=None, options=None):
        print "l"
        if results == None:
            results = self.RunStats(ai_params,options=options)

        num_collisions=0
        num_unique_collisions=0        
        # print "The length of the results is: " + str(len(results))
        for result in results:
            num_collisions = num_collisions + int(result.get_collisions())
            num_unique_collisions = num_unique_collisions + int(result.get_num_unique_collisions())
        
            
        print "total collisions: " + str(num_collisions)
        print "total unique collisions: " + str(num_unique_collisions)
        
        
        fun_value = num_collisions
        print "*** evaluation result for collisions: " + str(fun_value)
        return fun_value
    
    
    def coveragePlusFramesMetric(self, ai_params, results=None, options=None):
    
        if results == None:
            results = self.RunStats(ai_params,options=options)
        
        scenarios_failed_percent = self.coverageMetric(ai_params, results)
        percent_of_total_frames = self.framesMetric(ai_params, results)
        
        fun_value = ((scenarios_failed_percent)) + percent_of_total_frames
        print "*** evaluation result: " + str(fun_value)
        return fun_value

    def coveragePlusDistanceMetric(self, ai_params, results=None, options=None):
    
        if results == None:
            results = self.RunStats(ai_params,options=options)
        
        scenarios_failed_percent = self.coverageMetric(ai_params, results)
        distance_ratio = self.distanceMetric(ai_params, results)
        
        fun_value = ((scenarios_failed_percent)) + distance_ratio
        print "*** evaluation result: " + str(fun_value)
        return fun_value
    
    def coveragePlusPerformanceMetric(self, ai_params, results=None, options=None):
    
        if results == None:
            results = self.RunStats(ai_params,options=options)
        
        scenarios_failed_percent = self.coverageMetric(ai_params, results)
        
        # the performance metric is messured over a different find of scenario
        # the small number of agent in the representative set do not test performance
        # well enough so we use a performance set.
        
        # want to change numFrames to numFrames/10 
        # scenarioDir = performanceSet
        # scenarioSetInitDir to 0
        # numScenarios to 4
        
        # The dict should pick up the latest values so...
        options = copy.deepcopy(self._options)
        options.numFrames=100
        # options.processes
        options.scenarioSetStartId=0
        options.scenarioDirectory="data/scenarios/performanceSanityCheck"
        options.numScenarios=4
        if int(options.processes) > 4:
            options.processes=4
        
        other = SteerStats(options)
        other.setParamNameDict(self._ai_param_names)
        
        computation_ratio = other.performanceMetric(ai_params)
        
        fun_value = ((scenarios_failed_percent)* 0.5) + (computation_ratio * 0.5)
        print "*** evaluation result: " + str(fun_value)
        return fun_value
    
    def coveragePlusDistancePlusComputationMetric(self, ai_params, results=None, options=None):
        # print ai_params
        # sys.exit()
        if results == None:
            results = self.RunStats(ai_params,options=options)
        
        scenarios_failed_percent = self.coverageMetric(ai_params, results)
        distance_ratio = self.distanceMetric(ai_params, results)
        
        # the performance metric is messured over a different find of scenario
        # the small number of agent in the representative set do not test performance
        # well enough so we use a performance set.
        
        # want to change numFrames to numFrames/10 
        # scenarioDir = performanceSet
        # scenarioSetInitDir to 0
        # numScenarios to 4
        
        # The dict should pick up the latest values so...
        options = copy.deepcopy(self._options)
        options.numFrames=100
        # options.processes
        options.scenarioSetStartId=0
        options.scenarioDirectory="data/scenarios/performanceSanityCheck"
        options.numScenarios=4
        if int(options.processes) > 4:
            options.processes=4
        
        other = SteerStats(options)
        other.setParamNameDict(self._ai_param_names)
        
        computation_ratio = other.performanceMetric(ai_params)
        
        fun_value = (((scenarios_failed_percent)*0.33) + (distance_ratio * 0.33)
                 + (computation_ratio * 0.33))
        print "*** evaluation result for coveragePlusDistancePlusComputation: " + str(fun_value)
        return fun_value
    
    
            
    def varOpt(self, ai_params, results=None, weights=None, options=None):
        # print ai_params
        # sys.exit()
        metricValues = {}
        if weights is None:
			weights = options.config.getMetricWeights()
			# print "Weights " + str(weights)
        
        dataFile = open(self._options.cmaFilePrefix+"evalData.dat", "a")
        # if its weight is 0 then this value does not matter, otherwise it will be updated
        metricValues['computation_ratio'] = 0
        metricValues['scenarios_failed_percent'] = 0
        metricValues['distance_ratio'] = 0
        metricValues['ple_ratio'] = 0
        metricValues['simulation_time_ratio'] = 0
        metricValues['entropy_result'] = 0
        # check to make sure there is a point to running these computations
        if (results == None) and ((float(weights['coverage']) != 0) or
                                  (float(weights['ple']) != 0) or
                                  (float(weights['distance']) != 0) or
                                  (float(weights['simulationTime']) != 0)) :
            results = self.RunStats(ai_params,options=options)
        if float(weights['coverage']) != 0:
            metricValues['scenarios_failed_percent'] = self.inverseCoverageMetric(ai_params, results, options=options)
        if float(weights['distance']) != 0:
            metricValues['distance_ratio'] = self.distanceMetric(ai_params, results, options=options)
        if float(weights['ple']) != 0:
            metricValues['ple_ratio'] = self.pleMetric(ai_params, results, options=options)
        if float(weights['simulationTime']) != 0:
            metricValues['simulation_time_ratio'] = self.simulationTimeMetric(ai_params, results, options=options)
        
        # if its weight is 0 then this value does not matter, otherwise it will be updated
        if results != None and len(results) > 0:
            print "The length of the results is: " + str(len(results))
        
        if float(weights['entropy']) != 0:
            # Set up options for entropy metric
            metricValues['entropy_result'] = self.entropyMetric(ai_params, options=options)
        
        if float(weights['computation']) != 0:
            numPerformanceScenarios=7
            validationnumPerformanceScenarios=3
            # the performance metric is measured over a different kind of scenario
            # the small number of agents in the representative set does not test performance
            # well enough so we use a performance set.
            
            # want to change numFrames to numFrames/10 
            # scenarioDir = performanceSet
            # scenarioSetInitDir to 0
            # numScenarios to 4
            
            # The dict should pick up the latest values so...
            _options = copy.deepcopy(options)
            # total number of frames to be executed for performance benchmarks
            _options.numFrames=300 ## TODO
            _options.scenarioDirectory="data/scenarios/performanceSanityCheck"
            _options.dataDir="data/"
            # options.processes
            if options.validation:
                _options.scenarioSetStartId=7
                _options.numScenarios=validationnumPerformanceScenarios
                print "Running validation"
            else:
                _options.scenarioSetStartId=0
                _options.numScenarios=numPerformanceScenarios
            if int(_options.processes) > _options.numScenarios:
                _options.processes=_options.numScenarios
            
            # 
            other = SteerStats(_options)
            other.setParamNameDict(self._ai_param_names)
            
            metricValues['computation_ratio'] = other.performanceMetric(ai_params, options=_options)
        if self._options.verbose :
            print weights
            print metricValues

        # Write out a bunch of information to a data file
        # I don't want to use a loop to add dict data incase order changes
        # Weights first
        
        dataFile.write(str(float(weights['coverage'])) + "," +
                       str(float(weights['distance'])) + "," +
                       str(float(weights['computation'])) + "," +
                       str(float(weights['ple'])) + "," +
                       str(float(weights['simulationTime'])) + "," +
                       str(float(weights['entropy'])) + ",")
        # then metric values
        dataFile.write(str(metricValues['scenarios_failed_percent']) + "," +
                       str(metricValues['distance_ratio']) + "," +
                       str(metricValues['computation_ratio']) + "," +
                       str(metricValues['ple_ratio']) + "," +
                       str(metricValues['simulation_time_ratio']) + "," +
                       str(metricValues['entropy_result']))
        
        for param in ai_params:
            dataFile.write("," + str(param))

        dataFile.write("\n")        
        fun_value = (((metricValues['scenarios_failed_percent'])* float(weights['coverage'])) + 
                     (metricValues['distance_ratio'] * float(weights['distance'])) +
                     (metricValues['computation_ratio'] * float(weights['computation'])) +
                     (metricValues['ple_ratio'] * float(weights['ple'])) + 
                     (metricValues['simulation_time_ratio'] * float(weights['simulationTime'])) + 
                     (metricValues['entropy_result'] * float(weights['entropy'])) )
        print "*** evaluation result for varOpt " + str(fun_value)
        return fun_value
    
    def varOpt2(self, ai_params, results=None, weights=None, options=None):
        """Does not override the performance set
        I think this one is depreciated """
        # print ai_params
        # sys.exit()
        metricValues = {}
        
        dataFile = open(self._options.cmaFilePrefix+"evalData.dat", "a")
        # if its weight is 0 then this value does not matter, otherwise it will be updated
        metricValues['computation_ratio'] = 0
        metricValues['scenarios_failed_percent'] = 0
        metricValues['distance_ratio'] = 0
        metricValues['ple_ratio'] = 0
        metricValues['simulation_time_ratio'] = 0
        metricValues['entropy_result'] = 0
        # check to make sure there is a point to running these computations
        if (results == None) and ((float(weights['coverage']) != 0) or
                                  (float(weights['ple']) != 0) or
                                  (float(weights['distance']) != 0) or
                                  (float(weights['simulationTime']) != 0)) :
            results = self.RunStats(ai_params, options=options)
        if float(weights['coverage']) != 0:
            metricValues['scenarios_failed_percent'] = self.inverseCoverageMetric(ai_params, results, options=options)
        if float(weights['distance']) != 0:
            metricValues['distance_ratio'] = self.distanceMetric(ai_params, results, options=options)
        if float(weights['ple']) != 0:
            metricValues['ple_ratio'] = self.pleMetric(ai_params, results, options=options)
        if float(weights['simulationTime']) != 0:
            metricValues['simulation_time_ratio'] = self.simulationTimeMetric(ai_params, results, options=options)
        if float(weights['computation']) != 0:
            metricValues['computation_ratio'] = self.performanceMetric(ai_params, results)
        
        # if its weight is 0 then this value does not matter, otherwise it will be updated
        if results != None and len(results) > 0:
            print "The length of the results is: " + str(len(results))
        
        
        
        if self._options.verbose :
            print weights
            print metricValues

        # Write out a bunch of information to a data file
        # I don't want to use a loop to add dict data incase order changes
        # Weights first
        
        dataFile.write(str(float(weights['coverage'])) + "," +
                       str(float(weights['distance'])) + "," +
                       str(float(weights['computation'])) + "," +
                       str(float(weights['ple'])) + "," +
                       str(float(weights['simulationTime'])) + "," +
                       str(float(weights['entropy'])) + ",")
        # then metric values
        dataFile.write(str(metricValues['scenarios_failed_percent']) + "," +
                       str(metricValues['distance_ratio']) + "," +
                       str(metricValues['computation_ratio']) + "," +
                       str(metricValues['ple_ratio']) + "," +
                       str(metricValues['simulation_time_ratio']) + "," +
                       str(metricValues['entropy_result']))
        
        for param in ai_params:
            dataFile.write("," + str(param))

        dataFile.write("\n")        
        fun_value = (((metricValues['scenarios_failed_percent'])* float(weights['coverage'])) + 
                     (metricValues['distance_ratio'] * float(weights['distance'])) +
                     (metricValues['computation_ratio'] * float(weights['computation'])) +
                     (metricValues['ple_ratio'] * float(weights['ple'])) + 
                     (metricValues['simulation_time_ratio'] * float(weights['simulationTime'])) + 
                     (metricValues['entropy_result'] * float(weights['entropy'])) )
        print "*** evaluation result for varOpt " + str(fun_value)
        return fun_value
    
    
        
    def setParamNameDict(self, names):
        self._ai_param_names = []
        for name in names:
            self._ai_param_names.append(name)
            
    def getParamNameDict(self):
        return self._ai_param_names
            
    def RandomSample(self, options=None):
        
        # Example command
        # nice -4 python SteerStats.py --ai=sfAI --checkAgentInteraction --numScenarios=10 --scenarioSetInitId=0 --scenarioDIR=data/scenarios/representativeSet_Intersections/ --dataDir=data/ --paramFile xml/config/sf-param-config.xml --statsOnly --processes=1 --benchmark compositePLE --metricWeights=simulationTime=0.2,ple=0.2,coverage=0.2,distance=0.2,computation=0.2,entropy=0 --maxFrames=2000 --randomExperiment --randSamples=5 -c
        # nice -4 python SteerStats.py --ai=pprAI --checkAgentInteraction --numScenarios=10 --scenarioSetInitId=0 --scenarioDIR=data/scenarios/representativeSet_Intersections/ --dataDir=data/ --paramFile xml/config/ppr-param-config.xml --statsOnly --processes=1 --benchmark compositePLE --metricWeights=simulationTime=0.2,ple=0.2,coverage=0.2,distance=0.2,computation=0.2,entropy=0 --maxFrames=2000 --randomExperiment --randSamples=5 -c --cmaLogFileDir=data/TestCMA
        
        # time nice -4 python SteerStats.py --ai=pprAI --checkAgentInteraction --numScenarios=2500 --scenarioSetInitId=0 --scenarioDIR=data/scenarios/representativeSet_Intersections/ --dataDir=/Volumes/Raidy1/SteerFit2/PPR/Random/ --paramFile xml/config/ppr-param-config.xml --statsOnly --processes=7 --benchmark compositePLE --metricWeights=simulationTime=0.2,ple=0.2,coverage=0.2,distance=0.2,computation=0.2,entropy=0 --maxFrames=2000 --randomExperiment --randSamples=1000 -c --cmaLogFileDir=data/optimization/ppr/random/CMAES
        # time nice -4 python SteerStats.py --ai=rvo2dAI --checkAgentInteraction --numScenarios=2500 --scenarioSetInitId=0 --scenarioDIR=data/scenarios/representativeSet_Intersections/ --dataDir=/Volumes/Block0/SteerFit/ORCA/Random/ --paramFile xml/config/rvo2d-param-config.xml --statsOnly --processes=7 --benchmark compositePLE --metricWeights=simulationTime=0.2,ple=0.2,coverage=0.2,distance=0.2,computation=0.2,entropy=0 --maxFrames=2000 --randomExperiment --randSamples=1000 -c --cmaLogFileDir=data/optimization/rvo2d/random/CMAES
        # time nice -4 python SteerStats.py --ai=sfAI --checkAgentInteraction --numScenarios=2500 --scenarioSetInitId=0 --scenarioDIR=data/scenarios/representativeSet_Intersections/ --dataDir=/Volumes/Block1/SteerSuite/SteerFit/SF/Random/ --paramFile xml/config/sf-param-config.xml --statsOnly --processes=7 --benchmark compositePLE --metricWeights=simulationTime=0.2,ple=0.2,coverage=0.2,distance=0.2,computation=0.2,entropy=0 --maxFrames=2000 --randomExperiment --randSamples=1000 -c --cmaLogFileDir=data/optimization/sf/random/CMAES
        
        print "Randomly sampling " + str(self._options.randomSamples) + " experiments of varopt"
        param_xml_file = open(options.paramFile)
        param_config_data = param_xml_file.read()
        
        OptimExper = OptimizationExperiment(param_config_data)
        optimizingParameters = OptimExper.parseExperimentXML()
    
        ai_param_names_list = []
        results_list = []
        
        for ai_param in optimizingParameters:
            ai_param_names_list.append(ai_param.get_name())
       
        # print "Algorithm param names: " + str(ai_param_names_list)
        steerStats.setParamNameDict(ai_param_names_list)
        
        metricWeights = InsertStatistics.parseArg(options.metricWeights)
            
        s=0
        while s < int(self._options.randomSamples):
            aiparams=[]
            for ai_param in optimizingParameters:
                aiparams.append(random.uniform(ai_param.get_min(), ai_param.get_max()))
                
            results_list.append(steerStats.varOpt(aiparams, weights=metricWeights))
            s=s+1
        return results_list


if __name__ == "__main__":
    
    # clear; clear; time python SteerStats.py --ai pprAI --checkAgentInteraction --numScenarios 5000 --benchmark compositeGraph -c --scenarioSetInitId=0 --scenarioDIR data/scenarios/representativeSet  --statsOnly -p 8
    
    # 1.0,0.735411992542,14.7482148491,1.43407888685,3.11733329883,0.641553319987,0.5,0.433678858006,0.1,3.46612196415,1.26346831703,1.52452305194,0.380577221897,1.32644994809,2.52576241535,10.4924819505,0.815368287563,0.999,-0.78,-0.891397024683,0.23,7.69790841754,0.829129709001,1.55988738125,2.43304357918,1.9389505901,0.280198174079,0.08,0.2,0.333462710766,0.999,0.170603363741,5.01803334241,0.78,0.9,0.43,20.0490330832,56.06084719,21.5103065311
    
    # clear; clear; time nice -18 python SteerStats.py --ai pprAI --checkAgentInteraction --numScenarios 10 --benchmark compositePLE --scenarioSetInitId 0 --scenarioDIR data/scenarios/representativeSet_Intersections/ --skipInsert --aiParams=1.0,14.7482148491,1.43407888685,3.11733329883,0.641553319987,0.5,0.433678858006,0.1,3.46612196415,1.26346831703,1.52452305194,0.380577221897,1.32644994809,2.52576241535,10.4924819505,0.815368287563,0.999,-0.78,-0.891397024683,0.23,7.69790841754,0.829129709001,1.55988738125,2.43304357918,1.9389505901,0.280198174079,0.08,0.2,0.333462710766,0.999,0.170603363741,5.01803334241,0.78,0.9,0.43,20.0490330832,56.06084719,21.5103065311 --metric=varopt --metricWeights simulationTime=0.2,ple=0.2,coverage=0.2,distance=0.2,computation=0.2 --paramFile xml/config/ppr-param-config.xml -c
    import numpy as np
    
    options = getOptions()
    steerStats = SteerStats(options)
    if options.aiParams == "":
        
        if options.randomExperiment:
            steerStats.RandomSample()
        else:
            results_list = steerStats.RunStats()
            # print results_list
            
            # print results_list
            cov_results_list = steerStats.coverageMetric((), results_list)
            cov_results_list = steerStats.completedMetric((), results_list)
            cov_results_list = steerStats.completedAvgMetric((), results_list)
            # print "agent paths: " + str(results_list[0].get_agent_path_lengths())
            # print "agent times: " + str(results_list[0].get_agent_time_enableds())
            # print "agent distances: " + str(results_list[0].get_agent_distance_traveleds())
            # print "agent ples: " + str(results_list[0].get_agent_ple_energys())
            # print "agent completes: " + str(results_list[0].get_agent_completes())
            # print "agent successes: " + str(results_list[0].get_agent_successes())
            cov_results_list = steerStats.findInterestingCases((), results_list)
            print str(options.benchmark) + ": " + str(results_list[0].get_score())
    else:
        aiparams = [float(i) for i in options.aiParams.split(",")] # will help convert scientific notation to regular for C++ code (7.43725747e+02)
        param_xml_file = open(options.paramFile)
        param_config_data = param_xml_file.read()
        
        OptimExper = OptimizationExperiment(param_config_data)
        optimizingParameters = OptimExper.parseExperimentXML()
    
        ai_param_names_list = []
        
        
        for ai_param in optimizingParameters:
            ai_param_names_list.append(ai_param.get_name())
       
        # print "Algorithm param names: " + str(ai_param_names_list)
        steerStats.setParamNameDict(ai_param_names_list)
        
        if options.metric == "pleMetric":
            results_list = steerStats.pleMetric(aiparams)    
        elif options.metric == "performanceMetric":
            results_list = steerStats.performanceMetric(aiparams)
        elif options.metric == "distanceMetricGlobal":
            results_list = steerStats.distanceMetricGlobal(aiparams)
        elif options.metric == "timeMetricGlobal":
            results_list = steerStats.timeMetricGlobal(aiparams)
        elif options.metric == "pleMetricGlobal":
            results_list = steerStats.pleMetricGlobal(aiparams)
        elif options.metric == "coverageMetric":
            print "Doing coverage Metric"
            results_list = steerStats.coverageMetric(aiparams)
        elif options.metric == "entropyMetric":
            results_list = steerStats.entropyMetric(aiparams)
        elif options.metric == "collisions":
            results_list = steerStats.collisionsMetric(aiparams)
        elif options.metric == "varopt":
            # nice -4 python SteerStats.py --ai=pprAI --checkAgentInteraction --numScenarios=50 -c --scenarioSetInitId=0 --scenarioDIR=data/scenarios/representativeSet_Intersections/ --dataDir=data/ --paramFile xml/config/ppr-param-config.xml --statsOnly --processes=2 --benchmark compositePLE --metric=varopt --metricWeights=simulationTime=0.2,ple=0.2,coverage=0.2,distance=0.2,computation=0.2,entropy=0.0 --aiParams=2.6,14,1.7,1.31,1.15,1,0.77,0.5,1.9,0.16,0.55,0.26,0.95,1.5,10,0.94,0.99,-0.95,-0.95,0.55,8,0.8,4,5,1.1,0.3,0.3,0.05,0.15,0.9,0.1,3,0.4,0.5,0.2,20,50,20 --maxFrames=2000
            # nice -4 python SteerStats.py --ai=pprAI --checkAgentInteraction --numScenarios=5000 -c --scenarioSetInitId=0 --scenarioDIR=data/scenarios/representativeSet_Intersections/ --dataDir=data/ --paramFile xml/config/ppr-param-config.xml --statsOnly --processes=8 --benchmark compositePLE --metric=varopt --metricWeights=simulationTime=0.2,ple=0.2,coverage=0.2,distance=0.2,computation=0.2,entropy=0 --aiParams=1.078218e+00,6.681752e+00,2.679555e+00,1.314296e+00,1.757858e+00,1.411529e+00,1.816518e-01,7.536427e-01,3.76,1.222902e+00,1.511366e-01,8.142793e-02,9.740078e-01,2.209680e+00,1.023104e+01,7.902996e-01,9.122150e-01,-9.856751e-01,-8.570862e-01,7.271606e-01,1.228135e+01,9.758515e-01,1.257766e+00,6.284803e+00,3.551021e-01,2.879028e-01,6.090096e-01,1.907494e-01,7.004630e-02,8.242989e-01,2.060292e-01,4.472402e+00,1.652789e-01,8.830139e-01,3.553611e-01,35,63,14 --maxFrames=2000
            # nice -4 python SteerStats.py --ai=footstepAI --checkAgentInteraction --numScenarios=50 --scenarioSetInitId=0 --scenarioDIR=data/scenarios/representativeSet_Intersections/ --dataDir=data/ --paramFile xml/config/footstep-param-config.xml --statsOnly --processes=2 --benchmark compositePLE --metric=varopt --metricWeights=simulationTime=0.2,ple=0.2,coverage=0.2,distance=0.2,computation=0.2,entropy=0.0 --aiParams=5.00000000e-01,1.10000000e+00,3.00000000e-01,9.50000000e-01,2.50000000e-02,1.19571295e+00,7.00000000e-01,2.50000000e-02,2.92439748e-01,0.00000000e+00,1.00000000e+00,1.88978421e+01,3.10876701e+01,2.03516413e+01,9.88106561e+00,1.65167575e+01,6.31621291e-01,1.05000000e+00,7.43725747e+02 --maxFrames=2000 -c
            # nice -4 python SteerStats.py --ai=footstepAI --checkAgentInteraction --numScenarios=26 --scenarioSetInitId=0 --scenarioDIR=data/scenarios/representativeSet_Intersections/ --dataDir=data/simData/ --paramFile xml/config/footstep-param-config.xml --statsOnly --processes=1 --benchmark compositePLE --metric=varopt --metricWeights=simulationTime=0.2,ple=0.2,coverage=0.2,distance=0.2,computation=0,entropy=0.0 --aiParams=5.00000000e-01,1.10000000e+00,3.00000000e-01,9.50000000e-01,2.50000000e-02,1.19571295e+00,7.00000000e-01,2.50000000e-02,2.92439748e-01,0.00000000e+00,1.00000000e+00,1.88978421e+01,3.10876701e+01,2.03516413e+01,9.88106561e+00,1.65167575e+01,6.31621291e-01,1.05000000e+00,7.43725747e+02 --maxFrames=2000 -v -c
            
            # nice -4 python SteerStats.py --ai=rvo2dAI --checkAgentInteraction --numScenarios=5000 -c --scenarioSetInitId=0 --scenarioDIR=data/scenarios/representativeSet_Intersections/ --dataDir=data/ --paramFile xml/config/rvo2d-param-config.xml --statsOnly --processes=2 --benchmark compositePLE --metric=varopt --metricWeights=simulationTime=0,ple=0,coverage=0,distance=0,computation=0,entropy=1 --aiParams=3.282935,1.503676,2.176532,1.941628,12 --maxFrames=2000 --RealDataName=data/RealWorldData/bot-300-050-050_combined_MB.txt
            # For Authoring framework
            # nice -4 python SteerStats.py --ai=hybridAI --checkAgentInteraction --numScenarios=1 -c --scenarioSetInitId=0 --scenarioDIR=data/scenarios/AuthorTest/ --dataDir=data/ --paramFile xml/config/ppr-param-config.xml --statsOnly --processes=1 --benchmark compositePLE --metric=varopt --metricWeights=simulationTime=0.2,ple=0.2,coverage=0.2,distance=0.2,computation=0.0,entropy=0.0 --aiParams=2.6,14,1.7,1.31,1.15,1,0.77,0.5,1.9,0.16,0.55,0.26,0.95,1.5,10,0.94,0.99,-0.95,-0.95,0.55,8,0.8,4,5,1.1,0.3,0.3,0.05,0.15,0.9,0.1,3,0.4,0.5,0.2,20,50,20 --maxFrames=2000
            # For recording demo videos
            # nice -4 python SteerStats.py --ai=rvo2dAI --checkAgentInteraction --numScenarios=1 --scenarioSetInitId=0 --scenarioDIR=data/scenarios/customSet/circle-500/ --dataDir=data/ --paramFile xml/config/rvo2d-param-config.xml --statsOnly --processes=1 --benchmark compositePLE --metric=timeMetricGlobal --metricWeights=simulationTime=0.2,ple=0.2,coverage=0.2,distance=0.2,computation=0.2,entropy=0 --aiParams=2.000000,15.000000,10.000000,7.000000,10.000000 --maxFrames=2500 --recordSimulation
            metricWeights = InsertStatistics.parseArg(options.metricWeights)
            results_list = steerStats.varOpt(aiparams, weights=metricWeights, options=options)
        elif options.metric == "varopt2":
            metricWeights = InsertStatistics.parseArg(options.metricWeights)
            results_list = steerStats.varOpt2(aiparams, weights=metricWeights)
            
        else:
            print "Metric " + options.metric + " is not supported."
            sys.exit(5)
    
    
    
    # print results
 
     
