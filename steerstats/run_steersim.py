import sys
from steersuite.LogParser import LogParser
from steersuite import init_steerSim


if __name__ == "__main__":
    
    """
    Example run
    python run_steersim.py -module scenario,scenarioAI=pprAI,useBenchmark,benchmarkTechnique=compositePLE,checkAgentValid,reducedGoals,fixedSpeed,checkAgentRelevant,minAgents=3,maxFrames=1000,checkAgentInteraction,egocentric,scenarioSetInitId=0,numScenarios=20,skipInsert=True -config configs/pprAI-config.xml -commandLine
    """
    data = init_steerSim(len(sys.argv), sys.argv)
    
    print "Done in python"
    # print data
    i=0
    for key, value in data.iteritems():
        print str(key) + ": " + str(value)
        i=i+1
        print i
    
