import sys
from ConvertMap import convertMap
sys.path.append("../../")
import copy
import re

from steersuite.TestCase import TestCase

# Had to make this immune to unordered buckets
def constructScenarioData(scenario_data):
    # scenario_object.addObstacle(0, 1, 2, 3, 4, 5)
    # scenario_object.addObstacle(0, 1, 2, 3, 4, 5)
    scenarios={}
    # print "world xmin: " + scenario_map.getWorldXmin()
    for data_line in scenario_data:
        data = parseScenarioDataLine(data_line)
        if not data == None:
            
            if not (data['bucket'] in scenarios):
                scenarios[data['bucket']] = []
                
            scenarios[data['bucket']].append(data)
            """else :
                scenarios.append(scenario)
                scenario=[]
                scenario_num = data['bucket']
                scenario.append(data)
               """ 
            # print "data: " + str(data)
    
    # de-dict the situation
    scenariosList=[]
    for key, value in scenarios.iteritems():
        scenariosList.append(value)
        #temp = [key,value]
        #dictlist.append(temp)
    return scenariosList

def constructScenario(scenario_map, scen_data):
    
    
    for data in scen_data:
        # add 0.5 so the agent starts in the middle of the grid square
        scenario_map.addAgent(agentPos=[data['start x-coordinate']-
                                                (data['map width']/2)+0.5,0,
                     data['start y-coordinate']-
                     (data['map height']/2)+0.5],
                     targetLocationV=[data['goal x-coordinate']-
                                      (data['map width']/2)+0.5,0,
                                      data['goal y-coordinate']-
                                      (data['map height']/2)+0.5])
        
    return scenario_map
        
# scenario must a TestCase class
def constructScenarios(scenario_map, scenario_file_data):
    
    version = parseScenarioMetaData(scenario_file_data)
    print "version is: " + str(version)
    scenario_data = scenario_file_data.readlines()
    
    scen_datas = constructScenarioData(scenario_data)  
    # print scen_datas
    scenarios=[]
    for scenario in scen_datas:
        scenarios.append(constructScenario(scenario_map, scenario))
        scenario_map.removeAgents()

    return scenarios

def writeConstructedScenarios(scenario_map, scenario_file_data, file_name, n):
    
    version = parseScenarioMetaData(scenario_file_data)
    print "version is: " + str(version)
    scenario_data = scenario_file_data.readlines()
    
    scen_datas = constructScenarioData(scenario_data)  
    # print scen_datas
    scenarios=[]

    if  (((scenario_map.getWorldXmax() - scenario_map.getWorldXmin()) <= 512) and
        ((scenario_map.getWorldZmax() - scenario_map.getWorldZmin()) <= 512)):
        for scenario in scen_datas:
            scen = constructScenario(scenario_map, scenario)
            out_file = open(file_name+str(n)+".xml", "w")
            out_file.write(scen.getText())
            if True:
                print "World xmin: " + str(scenario_map.getWorldXmin())
                print "World xmax: " + str(scenario_map.getWorldXmax())
                print "World zmin: " + str(scenario_map.getWorldZmin())
                print "World zmax: " + str(scenario_map.getWorldZmax())
                print "the number of agents is: " + str(scenario_map.getNumAgents())
            scenario_map.removeAgents()
            out_file.close()
            n=n+1

    return n
    

def parseScenarioDataLine(data_line):
    
    
    #Example:
    """
    Bucket    map    map width    map height    start x-coordinate    start y-coordinate    goal x-coordinate    goal y-coordinate    optimal length
    0    maps/dao/arena.map    49    49    1    11    1    12    1
    0    maps/dao/arena.map    49    49    1    13    4    12    3.41421
    """
    
    data = re.split('\t+| +', data_line.rstrip('\n'))
    # print "data: " + str(data)
    # because in some of the files there is an extra line blank line at the end of the file
    if data[0] == '':
        return None
    data_dict = {}
    data_dict['bucket'] = int(data[0])
    data_dict['map'] = data[1]
    data_dict['map width'] = int(data[2])
    data_dict['map height'] = int(data[3])
    data_dict['start x-coordinate'] = int(data[4])
    data_dict['start y-coordinate'] = int(data[5])
    data_dict['goal x-coordinate'] = int(data[6])
    data_dict['goal y-coordinate'] = int(data[7])
    data_dict['optimal length'] = float(data[8])
    # print data_dict
    return data_dict
    
def parseScenarioMetaData(scenario_file_data):
    data = {}
    data['version']=scenario_file_data.readline().rstrip('\n').replace("version ", "")
    
    return data

def constructScenarios2(map_file_name, scen_file_name, out_dir, n):
    print "staring with testcase number: " + str(n)
    map_file = open(map_file_name, "r")
    scenario_file = open(scen_file_name, "r")
    dumpdir = out_dir
    scenario = convertMap(map_file)
    # print scenario.getText()
    n = writeConstructedScenarios(scenario, scenario_file, 
                                              dumpdir+"testcase-", n)
    map_file.close()
    scenario_file.close()
    """
    for scen in scenarios_out:
        out_file = open(dumpdir+"testcase-"+str(n)+".xml", "w")
        out_file.write(scen.getText())
        out_file.close()
        n=n+1""" 
    return n

if __name__ == '__main__':
    
    if not (len(sys.argv) == 5):
        print "What's wrong with you? " + str(len(sys.argv)) + " arguments?"
        sys.exit() 
    print "Input Map file is: " + sys.argv[1]
    print "Input scenario file is: " + sys.argv[2]
    print "Output scenario dir is: " + sys.argv[3]
    n=int(sys.argv[4])
    print "staring with testcase number: " + str(n)
    map_file = sys.argv[1]
    scenario_file = sys.argv[2]
    dumpdir = sys.argv[3]
    constructScenarios2(map_file, scenario_file, dumpdir, n)
