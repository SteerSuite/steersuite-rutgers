from optparse import OptionParser
from steersuite.Configuration import Configuration
import sys

def getOptions(_args=None):
    parser = OptionParser()
    parser.add_option("-f", "--file", dest="filename",
                      help="write report to FILE", metavar="FILE")
    parser.add_option("-v", "--verbose",
                      action="store_true", dest="verbose", default=False,
                      help="print status messages to stdout")
    
    parser.add_option("--recordDIR", "--recordingDirectory",
                      action="store", dest="recordingDirectory", default="scenarioRecordings",
                      metavar="Directory",
                      help="Specify the directory that recording files will be stored")
    
    parser.add_option("--frameDIR", "--frameDirectory",
                      action="store", dest="frameDirectory", default="scenarioFrames",
                      metavar="Directory",
                      help="Specify the directory that frame image files will be stored")
    
    parser.add_option("--testcaseDIR", "--testcaseDirectory",
                      action="store", dest="testcaseDirectory", default="testCases",
                      metavar="Directory",
                      help="Specify if the system should write the generated testcases to disk")
    
    parser.add_option("--dumpTestcases", 
                      action="store_true", dest="dumpTestcases", default=False,
                      metavar="BOOL",
                      help="Specify the directory that scenarios files will be stored")
    
    parser.add_option("--scenarioDIR", "--scenarioDirectory",
                      action="store", dest="scenarioDirectory", default="data/scenarios/representativeSet_Intersections",
                      metavar="Directory",
                      help="Specify the directory that testcase files will be read from")
    
    parser.add_option("--benchFile", "--benchmarkFileName",
                      action="store", dest="benchmarkFileName", default="test",
                      metavar="FILE",
                      help="The name of the benchmark file.")
    
    parser.add_option("--numScenarios", "--numTests",
                      action="store", dest="numScenarios", default=1,
                      metavar="INTEGER",
                      help="The number of scenarios to be executed")
    
    parser.add_option("--scenarioSetInitId", "--scenarioSetStartId",
                      action="store", dest="scenarioSetStartId", default=0,
                      metavar="Integer",
                      help="The scenario ID to start with")
    
    parser.add_option("--ai", "--steeringAlgorithm",
                      action="store", dest="steeringAlgorithm", default="SimpleAI",
                      metavar="module", help="Specify the directory that  files will be stored")
    
    parser.add_option("--dataDir", "--datastoreDirectory", "--dataPath",
                  action="store", dest="dataPath", default="data/simData/",
                  metavar="Directory", help="Specify the directory that files will be stored")
    
    parser.add_option("-p", "--processes", "--availableProcesses",
              action="store", dest="processes", default=1,
              metavar="INTEGER", help="The number of processes the SteerStats script can use")
    
    parser.add_option("--cma-p", "--CMAprocesses", "--CMAavailableProcesses",
              action="store", dest="cmaProcesses", default=1,
              metavar="Integer", help="""The number of processes the CMA Algorithm can use script can use. It 
              is unknown what will happen when you start to combine this with -p""")
    
    parser.add_option("-b", "--benchmark", "--benchmarkTechnique",
          action="store", dest="benchmark", default="compositePLE",
          metavar="String", help="Specify the directory that files will be stored")
    
    parser.add_option("--frameSize", 
          action="store", dest="frameSize", default="640x480",
          metavar="IntegerxInteger", help="The pixel width and height, example 640x480")
    
    parser.add_option("-c", "--commandline", "--commandLine", 
          action="store_true", dest="commandLine", default=False,
          help="TO specify if an openGL window should be created")

    parser.add_option("--paramFile", "--parameterOptimizationConfig", "--pf", 
          action="store", dest="paramFile", default="",
          metavar="FILE", help="The file to be used for the optimization procedure")
    
    parser.add_option("--checkAgentInteraction", "--checkInteraction", 
          action="store_true", dest="checkAgentInteraction", default=False,
          help="Parameter to tell the system to work to force agent interaction when generating scenarios")
    
    parser.add_option("--checkAgentInteractionRadius", "--interactionRadius", 
          action="store", dest="checkAgentInteractionRadius", default=-1.0,
          metavar="FLOAT", help="The radius to be used to evaluate expected interaction.")
    
    parser.add_option("--numFrames", "--frames", "--maxFrames", 
          action="store", dest="numFrames", default=1000,
          metavar="INTEGER", help="The max number of frames to be executed in a scenario")
    
    parser.add_option("--db", "--database", "--DataBaseName", 
          action="store", dest="dbName", default="steersuitedb",
          metavar="STRING", help="The name of the database to be used to store data.")
    
    parser.add_option("--recordSimulation", "--record", "-r", 
          action="store_true", dest="recordSimulation", default=False,
          help="Tells steersuite to dump a record file of the simulation")
    
    parser.add_option("--statsOnly", "--noInserts", "--skipInsert",
          action="store_true", dest="skipInsert", default=False,
          help="Skips inserting the metrics generated into the database.")
    
    parser.add_option("--passAllParams", 
          action="store_true", dest="passAllParams", default=False,
          help="Makes sure to pass all parameters to .")

    parser.add_option("--saveFrames", 
            action="store_true", dest="saveFrames", default=False,
          help="""Flag to tell the scenario module to dump frame files. The openGL
          window must be rendered for this to work""")
    
    parser.add_option("--makeVideo", "--compileVideo", "--renderVideo", 
            action="store_true", dest="makeVideo", default=False,
          help="""Flag to tell the scenario module to render a video from the dumped frames.
           The openGL window must be rendered for this to work""")
    
    parser.add_option("--scenarioFilePrefix",  
          action="store", dest="scenarioFilePrefix", default="",
          metavar="STRING", help="Prefix used before the number of testcase files.")
    
    parser.add_option("--cmaLogFileDir", "--cmaDumpDir", "--cma",  
          action="store", dest="cmaFilePrefix", default="",
          metavar="STRING", help="Prefix used before the cma Log files.")

    parser.add_option("--checkStaticPathInteractions", "--checkStaticInteractions",
          "--chSI", action="store_true", dest="checkStaticPathInteractions", 
          default=False, help="Flag checking for static path interactions.")
    
    parser.add_option("--calculateMaxFrames", "--calcMaxFrame",
          "--CalcFrames", action="store_true", dest="calculateMaxFrames", 
          default=False, help="""Flag to tell scenario module to calculate the 
          maximum number of frames for a simulation from the loaded testcase file.""")
    
    parser.add_option("--forceConfig", 
          action="store", dest="forceConfig", default="",
          metavar="STRING", help="Used to force the simulation to use the specified config file.")
    
    parser.add_option("--scenarioTimeThreshold", "--timeOut", "-t",
          action="store", dest="scenarioTimeThreshold", default="",
          metavar="STRING", help="Used to limit the max time a simulation can run in seconds.")
    
    parser.add_option("--maxNodesToExpandForSearch", 
          action="store", dest="maxNodesToExpandForSearch", default="",
          metavar="STRING", help="Used to change the number of nodes expanded in A-star search.")
    
    parser.add_option("--optimizeWith", "--optimizer", "--op", "--optimizationAlgorithm", 
      action="store", dest="optimizationAlgorithm", default="CMA-ES",
      metavar="STRING", help="Algorithm to use for parameter optimization.")
    
    parser.add_option("--aiParams", 
      action="store", dest="aiParams", default="",
      metavar="STRING", help="Can be used to pass ai parameters into the simultion in a list")
    
    parser.add_option("--metric", 
      action="store", dest="metric", default="",
      metavar="STRING", help="Used to identify a desired metric.")
    
    parser.add_option("--metricWeights", 
      action="store", dest="metricWeights", default="",
      metavar="STRING", help="Used to identify the weights to be used for each metric.")
    
    parser.add_option("--RealDataName", 
      action="store", dest="realDataFileName", default="",
      metavar="STRING", help="Used to identify the file to use that contains real world data.")
    
    parser.add_option("--RealDataDir", 
      action="store", dest="realDataDirName", default="",
      metavar="STRING", help="Used to identify the directory to use that contains real world data.")
    
    
    
    parser.add_option("--validate", "--validation",
          "--validateOnTestSet", action="store_true", dest="validation", 
          default=False, help="""Flag to tell SteerStats that it should perform
          validation run on scenarios.""")
    
    parser.add_option("--randomExperiment", "--randomExp",
          "--randExp", action="store_true", dest="randomExperiment", 
          default=False, help="""Performs a random sampling of the steering algorithms
          parameters for equally weighted varopt""")
    
    parser.add_option("--randSamples", "--samples",  
          action="store", dest="randomSamples", default=1000,
          metavar="INTEGER", help="The number of random samples")
    
    parser.add_option("--noTweakBar", "--noTweak",
          action="store_true", dest="noTweakBar", 
          default=False, help="""TODO no tweakBar""")
    
    parser.add_option("--startPaused", 
           action="store_true", dest="startPaused", 
          default=False, help="""Start Simulation Paused TODO""")
    
    parser.add_option("--subspace", "--scenariosubspace",  
          action="store", dest="subspace", default="",
          metavar="STRING", help="A string to indicate the subspace to be used")
    
    parser.add_option("--subspaceParams", "--scenariosubspaceParams",  
          action="store", dest="subspaceParams", default="",
          metavar="STRING", help="A string to define the parameters to the subspace")
    parser.add_option("--subspaceObsType", "--scenariosubspaceObsType",  
          action="store", dest="subspaceObsType", default="circle",
          metavar="STRING", help="A string to define the type od obstacle to generate")
    
    parser.add_option("--multiObjRestartFile",   
          action="store", dest="multiObjRestartFile", default="",
          metavar="STRING", help="A string to a file that a multi Objective optimization can continue")
    
    parser.add_option("--noReturn", 
           action="store_true", dest="noReturn", 
          default=False, help="""Whether or not to return the data collected from the
          simulation. Saves memory sometimes""")
    
    parser.add_option("--logMetrics", 
           action="store_true", dest="logMetrics", 
          default=False, help="""Whether or not to dump log files of all the metric and objective values""")
    
    parser.add_option("--objective", "--objectiveFunction", 
           action="store", dest="objectiveFunction", 
          default="", help="""The objective function to evaluate with.""")
    parser.add_option("--penalty", "--penaltyFunction", 
           action="store", dest="penaltyFunction", 
          default="overlapPenalty", help="""The penalty function to evaluate with.""")
    
    parser.add_option("--noSimulation", 
           action="store_true", dest="noSimulation", 
          default=False, help="""Whether or not to run a steersuite simulation. The idea is to instead read the data from previous simulation files""")
    
    parser.add_option("--config", 
           action="store", metavar="STRING", dest="configFile", default="",
          help="""The xml config file that many of the config settings can be parsed from""")
    
    parser.add_option("--cmaMaxEvals", "--CMAMaxEvals",
           action="store", metavar="INTEGER", dest="cmaMaxEvals", default=1000,
          help="""The number of evaluations the CMA algorithm will execute before halting""")
    
    parser.add_option("--processingEvents", 
           action="store", metavar="STRING/XML", dest="processingEvents", default="",
          help="""XML data describing what processing events should be executed after the simulation is done.
          Used mostly by OptimizeAlgorithm.py""")
    
    parser.add_option("--randomScenarios", 
           action="store_true", dest="randScenarios", 
          default=False, help="""Use to simulate randomly generated scenario, from the representative Set
          (see ScenarioSpace)\n
          This can not be multi-processed well.""")
    
    parser.add_option("--randomSeed", 
           action="store", dest="randomSeed", metavar="INTEGER",
          default=10, help="""randomSeed that will be used for random scenario generation.""")
    
    if _args is None:
		(options, args) = parser.parse_args()
    else:
	    (options, args) = parser.parse_args(_args)
		
    if options.configFile != "":
        config = Configuration(open(options.configFile).read())
        # print config.getParameterString().split(" ")
        args = sys.argv[1:]+config.getParameterString().split(" ")
        # print args
        (options, args) = parser.parse_args(args)
        options.config = config
    return options

# print getOptions()
