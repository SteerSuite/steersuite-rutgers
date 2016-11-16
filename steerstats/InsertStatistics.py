import psycopg2
import sys
import copy
import cProfile, pstats, io
from steersuite.SteerStatsOptions import getOptions
from multiprocessing import Semaphore

# Supported AIs
from steersuitedb.PprAI import PprAI
from steersuitedb.FootstepAI import FootstepAI
from steersuitedb.CcAI import CcAI
from steersuitedb.ReactiveAI import ReactiveAI
from steersuitedb.RVO2DAI import RVO2DAI
from steersuitedb.EgocentricAI import EgocentricAI
from steersuitedb.SocialForcesAI import SocialForcesAI

from steersuitedb.Test import Test
from steersuitedb.Benchmark import Benchmark
from steersuitedb.Video import Video
from steersuitedb.Recording import Recording
from steersuitedb.Algorithm import Algorithm
from steersuitedb.AlgorithmData import AlgorithmData
from steersuitedb.Composite2Benchmark import Composite2Benchmark
from steersuitedb.CompositePLEBenchmark import CompositePLEBenchmark
from steersuitedb.CompositeGraphBenchmark import CompositeGraphBenchmark
from steersuitedb.Composite1Benchmark import Composite1Benchmark
from steersuitedb.Scenario import Scenario
from steersuitedb.Config import Config
from steersuitedb.ConnectionManager import ConnectionManager
from steersuite.Configuration import Configuration
from steersuite.TestCase import TestCase
from steersuitedb.PPRConfig import PPRConfig

from steersuitedb.BaseCompositePLE import BaseCompositePLE
from steersuitedb.BaseCompositeEntropy import BaseCompositeEntropy
from steersuitedb.BaseComposite02 import BaseComposite02

from steersuite.LogParser import LogParser
from steersuite.OptimizationExperiment import OptimizationExperiment
from steersuitedb.RVO2DConfig import RVO2DConfig
from steersuitedb.SFConfig import SFConfig

# Used to ensure items are inserted into the database in order
# more for querry efficiency than anything else.
__db_processes=1
__dbSem=Semaphore(__db_processes)

def parseArg(arg):
    outargs = []
    args = arg.split(",")
    for _arg in args:
        _args = _arg.split("=")
        if len(_args) == 1:
            _args.append("") # In the case of no second value
        outargs.append(_args)
    return {key: value for (key, value) in outargs}

def parseLogData(data, args):
    
    algorithmName=args['scenarioAI'] # "pprAI" or "footstapAI"
	
	 # add steering algorithms as needed
    if algorithmName == "pprAI":
        algorithmAI = PprAI()
    elif algorithmName == "footstepAI":
        algorithmAI = FootstepAI()
    elif algorithmName == "ccAI":
        algorithmAI = CcAI()
    elif algorithmName == "rvo2dAI":
        algorithmAI = RVO2DAI()
    elif algorithmName == "reactiveAI":
        algorithmAI = ReactiveAI()
    elif algorithmName == "egocentricAI":
        algorithmAI = EgocentricAI()
    elif algorithmName == "sfAI":
        algorithmAI = SocialForcesAI()
    else:
        print "Unknown AI algorithm"
        sys.exit(0)
        
    algorithmAI.setValuesFromDict(aData)
    
    return 0

def InsertStatistics(params, configFilePath, dataQueue=None):

    # pr = cProfile.Profile()
    # pr.enable()
    __dbSem.acquire()
    options = getOptions()    
    args = parseArg(params) # the same command passed to the module
    stats = []
    cur=None
    scenario_id=0
    config_id=0
    if 'recFileFolder' in args:
        recFileNameSuffix=args['recFile'] # "rec"
        recFileNamePath=args['recFileFolder'] # "./"
    # frameBasePath=args['framePath']# "scenarioFrames/"
    benchmarkLog=args['benchmarkLog'] # "test.log"
    if 'testCasePath' in args: # check if key is in dict
        testFilePath=args['testCasePath']
        testFilePrefix="testcase-"
    elif 'subspace' in args:
        testFilePath=args['subspace']
        testFilePrefix=''
        subspaceFile = open(testFilePath, 'r')
    elif options.randScenarios:
        testFilePrefix="testcase-"
        # dumped testcases
        testFilePath=args['testCasePath']
    else:
        testFilePath=args['scenarioSetPath']
        testFilePrefix="testcase-"
        
    #if "scenarioSetInitId" in args:
    #    scenarioSetInitId = 0 # args['scenarioSetInitId']
    #else:
    #    scenarioSetInitId=0
    
    videoType="mp4"
    videoBaseName="scenario" # Need to implement in C++ (scenarioModule)
    scenario_description=str(params)
    # items that may change (different ai algorithm or benchmark)
    benchmarkName=args['benchmarkTechnique'] # "Composite02" or "Composite01"
    algorithmName=args['scenarioAI'] # "pprAI" or "footstapAI"
    algorithmLogFileName=args['ailogFileName']
    config_id=1
    
    configFile = open(configFilePath, "r")
    # print "config file path: " + configFilePath
    configFileData = configFile.read()
    config = Configuration(configFileData)
    configFile.close()
    
    test_comments="AlgorithmName: " + algorithmName
    test_comments=test_comments + ", BenchmarkName: " + benchmarkName
    # The point of all of these varaibles is to make scripting easier.
    # Naming convention will tailor to conforming to steersuite practice
    # allowing for ai names and benchmark names to match the easiest.
    

    
    try:

        
        if not 'skipInsert' in args:
            if 'dbName' in args:
                conM = ConnectionManager(dbname=args['dbName'])
                if options.verbose :
                    print "DataBase used: " + args['dbName']
            else:
                if options.verbose :
                    print "DataBase used: " + "steersuitedb"
                conM = ConnectionManager()
            cur = conM.getCursor() 
            
            
            # need the benchmark_id
            
            benchmark = Benchmark()
            benchmark_id = benchmark.getBenchmarkIndex(cur, benchmarkName)
            
            
            # also need a algorithm id
            
            algorithm = Algorithm()
            algorithm_id = algorithm.getAlgorithmIndex(cur, algorithmName)
            
            # We can load the configuration data for this
            config_data = {}
            
            config_data['name'] = "stats_config"
            config_data['fps'] = config.getFPS()
            config_data['grid_size_z'] = config.getGridSizeZ()
            config_data['grid_size_x'] = config.getGridSizeX()
            config_data['grid_cells_z'] = config.getGridCellsZ()
            config_data['grid_cells_x'] = config.getGridCellsX()
            config_data['config_xml'] = configFileData
            
            configuration = Config()
            configuration.setValuesFromDict(config_data)
            config_id = configuration.insertConfig2(cur)
            
            #then insert a scenario for this scenario
            
            scenario = Scenario(algorithm_id, benchmark_id, config_id, scenario_description)
            scenario_id = scenario.insertScenario2(cur)
            
            if options.verbose :
                print "scenario id: " + str(scenario_id)
            
            
            # add steering algorithms as needed
            if algorithmName == "pprAI":
                algorithmAI = PprAI()
            elif algorithmName == "footstepAI":
                algorithmAI = FootstepAI()
            elif algorithmName == "ccAI":
                algorithmAI = CcAI()
            elif algorithmName == "rvo2dAI":
                algorithmAI = RVO2DAI()
            elif algorithmName == "reactiveAI":
                algorithmAI = ReactiveAI()
            elif algorithmName == "egocentricAI":
                algorithmAI = EgocentricAI()
            elif algorithmName == "sfAI":
                algorithmAI = SocialForcesAI()
            else:
                print "Unknown AI algorithm"
                sys.exit(0)
        
       
        try:
            print '============' + benchmarkLog
            benchmarkLogFile = open(benchmarkLog,"r")
            rand_seed = benchmarkLogFile.readline() #save / remove random seed
            lparser = LogParser()
            logData = lparser.parseLog(benchmarkLogFile)
            
            print '============' + algorithmLogFileName
            # pr = cProfile.Profile()
            # pr.enable()
            algorithmLogFile = open(algorithmLogFileName, "r")
            algorithmData = lparser.parseLog(algorithmLogFile)
        except IOError:
            print "Are you sure you have compiled SteerSuite?"
            raise IOError("Are you sure you have compiled SteerSuite?")
        # print logData
        
        # sys.exit(0)
        
        if options.verbose :
            print "logdata length: " + str(len(logData))
            print "algorithmData length: " + str(len(algorithmData))
        
        # sys.exit(0)
        if 'skipInsert' in args:
            # pr = cProfile.Profile()
            # pr.enable()  
            for ldat,aData in zip(logData,algorithmData):
                n= ldat['scenario_id'] 
                
                if 'subspace' in args:
                    testcaseFile = open(testFilePath, 'r')
                else:
                    testcaseFile = open(testFilePath+"/"+testFilePrefix+n+".xml", "r")
                  
                
                testcase = testcaseFile.read()
                testcase_ = TestCase(testcase)
                
                test_status=ldat['agent_success']
                ldat['test_comments']=test_comments
                # ldat['benchmark_type']=benchmark_id # update benchmark_id because of naming issues
                # ldat['test_case']=testcase
                ldat['test_status']=test_status
                # ldat['scenario_group']=scenario_id
                # __test_case = TestCase(testcase)
                # // TODO
                # ldat['num_agents']=__test_case.getNumAgents()
                # ldat['num_obstacles']=__test_case.getNumObstacles()
                
                ldat['num_agents'] = testcase_.getNumAgents()
                ldat['num_obstacles'] = testcase_.getNumObstacles()
                # needed for data but not used in calculations
                ldat['algorithm_data_id']=0
                ldat['benchmark_type']=0
                ldat['test_comments']=test_comments
                ldat['test_case']=""
                ldat['test_status']=""
                ldat['scenario_group']=scenario_id
                
                if benchmarkName == "compositeEntropy":
                    baseStats = BaseCompositeEntropy()
                elif benchmarkName == "composite02":
                    baseStats = BaseComposite02()
                else:
                    baseStats = BaseCompositePLE()
                baseStats.setMetricValuesFromDict(dict(ldat.items() + aData.items()))
                stats.append(baseStats)

        else:
            for ldat,aData in zip(logData,algorithmData):
                testcase=""
                # n= str(int(ldat['scenario_id']) + int(scenarioSetInitId)) # the numbering for the file
                n= ldat['scenario_id'] 
                
                # print "Scenario Id: " + n
            
                # then insert test data
                # read dumped test xml
                if 'subspace' in args:
                    testcaseFile = open(testFilePath, 'r')
                else:
                    testcaseFile = open(testFilePath+"/"+testFilePrefix+n+".xml", "r")
                  
                testcase = testcaseFile.read()
                testcase_ = TestCase(testcase)
                # print testcase 
                
                # read the record data
                if 'recFileFolder' in args:
                    recordFile = open(recFileNamePath+"rec"+n+"."+recFileNameSuffix, "r+b")
                    recordbytes = recordFile.read()
                    recordFile.close()
                    recordArray = bytearray()
                
                    for bite in recordbytes:
                        recordArray.append(bite)
                    
                """pathToVideoFile=frameBasePath+videoBaseName+n+"."+videoType
                print "Video file: " + pathToVideoFile
                videoFile = open(pathToVideoFile, "r+b")
                videobytes = videoFile.read()
                videoFile.close()
                videoArray = bytearray()
                
                for vbite in videobytes:
                    videoArray.append(vbite)"""
                
                # now that all of the file have been read properly insert a test
                # need a way to check status
                # test_status=0
                test_status=ldat['agent_success']
                
                
                algorithmAI.setValuesFromDict(aData)
                algorithm_data_id = algorithmAI.insertAlgorithmData2(cur)
                
                ldat['algorithm_data_id']=algorithm_data_id
                ldat['test_comments']=test_comments
                ldat['benchmark_type']=benchmark_id # update benchmark_id because of nameing issues
                ldat['test_case']=testcase
                ldat['test_status']=test_status
                ldat['scenario_group']=scenario_id
                __test_case = TestCase(testcase)
                # // TODO
                # ldat['num_agents']=__test_case.getNumAgents()
                # ldat['num_obstacles']=__test_case.getNumObstacles()
                
                ldat['num_agents'] = testcase_.getNumAgents()
                ldat['num_obstacles'] = testcase_.getNumObstacles()
                #print ldat
                # then other stuff for config.
                # check is config already exists
                # save width and heigth stuff
                
                if benchmarkName == "composite01":
                    benchmark = Composite1Benchmark()
                elif benchmarkName == "composite02":
                    benchmark = Composite2Benchmark()
                elif benchmarkName == "compositeGraph":
                    benchmark = CompositeGraphBenchmark()
                elif benchmarkName == "compositePLE":
                    benchmark = CompositePLEBenchmark()
                else:
                    print "Invalid benchmark type: " + benchmarkName
                    sys.exit()
                # print ldat
                benchmark.setBenchmarkValuesFromDict(ldat)
            
                benchmark_data_id = benchmark.insertBenchmark2(cur)
                
                if benchmark_data_id % 50 == 0:
                    if options.verbose :
                        print "Test id: " + str(benchmark_data_id)
                
                """video = Video(benchmark_data_id, videoArray)
                status = video.insertVideo(cur)"""
                
                if 'recFileFolder' in args:
                    recording = Recording(benchmark_data_id, recordArray)
                    status = recording.insertRecording(cur)
                
                
                testcaseFile.close()
                if benchmarkName == "compositeEntropy":
                    baseStats = BaseCompositeEntropy()
                elif benchmarkName == "composite02":
                    baseStats = BaseComposite02()
                else:
                    baseStats = BaseCompositePLE()
                baseStats.setMetricValuesFromDict(dict(ldat.items() + aData.items()))
                stats.append(baseStats)
                
        
        # last is benchmark
        # oid = psycopg2.extensions.lobject()
        
        # close logFile
        benchmarkLogFile.close()
        if options.verbose :
            print "scenario id: " + str(scenario_id)
        out = {}
        out['scenario_id']=scenario_id
        out['config_id']=config_id
        
        __dbSem.release()
        if dataQueue:
            dataQueue.put(stats)
#        pr.disable()
#        f = open('x.prof', 'a')
#        pstats.Stats(pr, stream=f).sort_stats('time').print_stats()
#        f.close()
        return out
        # videoFileOut.close()
    
    except psycopg2.DatabaseError, e:
        print 'Error pprAI-test: %s' % e    
        
        
    finally:
        
        if cur:
            conM.dispose()
            
            
def InsertOptimizationConfig(parameters, configFilePath, aiConfigParams):
    insertData = InsertStatistics(parameters, configFilePath)
    
    
    print aiConfigParams
    print insertData
    options = getOptions()
    param_xml_file = open(options.paramFile)
    param_config_data = param_xml_file.read()
    
    OptimExper = OptimizationExperiment(param_config_data)
    optimizingParameters = OptimExper.parseExperimentXML()
    for param in optimizingParameters:
        if not param.get_name() in aiConfigParams:
            if param.get_type() == "float":
                aiConfigParams[param.get_name()] = str(float(param.get_original()))
            elif param.get_type() == "integer":
                aiConfigParams[param.get_name()] = str(int(param.get_original()))
    aiConfigParams['ppr_ai_config_id'] = insertData['config_id']
    _args = parseArg(parameters)
    if 'dbName' in _args:
        conM = ConnectionManager(dbname=_args['dbName'])
        if options.verbose :
            print "DataBase used: " + _args['dbName']
    else:
        if options.verbose :
            print "DataBase used: " + "steersuitedb"
        conM = ConnectionManager()
        
    cur = conM.getCursor()
    
    
    if _args['scenarioAI'] == "pprAI":
        config_ = PPRConfig(aiConfigParams)
        aiConfigParams['ppr_ai_config_id'] = insertData['config_id']
    elif _args['scenarioAI'] == "rvo2dAI":
        config_ = RVO2DConfig(aiConfigParams)
        aiConfigParams['rvo2d_ai_config_id'] = insertData['config_id']
    elif _args['scenarioAI'] == "sfAI":
        config_ = SFConfig(aiConfigParams)
        aiConfigParams['sf_ai_config_id'] = insertData['config_id']
    else:
        print "Configuration not supported for " + str(_args['scenarioAI'])
    
    config_.insertConfig2(cur)
    
    conM.dispose()
    # pprC2 = pprC.getConfigData(cur, 1)
    
    return insertData
 