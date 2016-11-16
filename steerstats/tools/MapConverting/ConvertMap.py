import sys
import xml.etree.ElementTree as ET
sys.path.append("../../")

from steersuite.TestCase import TestCase


# argv[1] the file to convert
# argv[2] the output SteerSuite testcase file.

# Example:
# python ConvertMap.py ../../../testcases/star_craft/map_files/BigGameHunters.map ../../../testcases/star_craft/BigGameHunters.xml

# This script will take a map file from the movingAI project and convert it
# into a scenario file for use in Steersuite. It only converts the
# obstalces for the scenario file. A Map file contains not information
# about the agents


def convertMapData(map_data, scenario_object, map_meta_data):
    # scenario_object.addObstacle(0, 1, 2, 3, 4, 5)
    # scenario_object.addObstacle(0, 1, 2, 3, 4, 5)
    print "******* " + str(len(map_data))
    xmin = map_meta_data['xmin']
    zmin = map_meta_data['zmin']
    x = float(xmin)
    z = float(zmin)
    obstacle_count=0
    found_obstacleflag=False
    
    for data_line in map_data:
        for char in data_line:
            if (char == "T" or char == "@" or char == "0" or char == "W"): # and obstacle_count < 20000:
                if found_obstacleflag == False:
                    _xmin=x
                    found_obstacleflag=True
                # print "Found a Tree at: " + str(x) + " " + str(z)
                # scenario_object.addObstacle(x, x+1-0.01, 0, 1, z+0.01, z+1-0.01)
                
            else:
                if found_obstacleflag == True:
                    found_obstacleflag=False
                    # scenario_object.addObstacle(_xmin, x-0.01, 0, 1, z+0.01, z+1-0.01)
                    scenario_object.addObstacle(_xmin, x, 0, 1, z, z+1)
                    obstacle_count = obstacle_count + 1
            x = x + 1
        x = xmin
        if found_obstacleflag == True:
            found_obstacleflag=False
            # scenario_object.addObstacle(_xmin, x-1.01, 0, 1, z+0.01, z+1-0.01)
            scenario_object.addObstacle(_xmin, x, 0, 1, z, z+1)
            obstacle_count = obstacle_count + 1
        found_obstacleflag=False
        z = z + 1
    return obstacle_count
    
def parseMapMetaData(map_file):
    data = {}
    
    data['format']=map_file.readline().rstrip('\n').replace("type ", "")
    data['height']=map_file.readline().rstrip('\n').replace("height ", "")
    data['width']=map_file.readline().rstrip('\n').replace("width ", "")
    data['name']=map_file.readline().rstrip('\n')
    
    return data

def convertMap(map_file):
    print map_file
    
    map_meta_data = parseMapMetaData(map_file)
    map_data = map_file.readlines()
    scenario = TestCase()
    # print map_meta_data
    
    xmax = float(map_meta_data['width'])/2
    zmax = float(map_meta_data['height'])/2
    version = 1.0
    scenario.addHeader(map_meta_data['name'], version, -xmax, xmax, 0, 0, -zmax, zmax)
    
    map_meta_data['xmin'] = -xmax
    map_meta_data['zmin'] = -zmax
    
    obs_count = convertMapData(map_data, scenario, map_meta_data=map_meta_data)
    # print scenario.getText()
    print "The number of obstacle is: " + str(obs_count)
    return scenario
    
    
    
    
    
    
if __name__ == '__main__':
    
    print "Input Map file is: " + sys.argv[1]
    print "Output scenario file is: " + sys.argv[2]
    map_file = open(sys.argv[1], "r")
    scenario_file = open(sys.argv[2], "w")
    scenario = convertMap(map_file)
    map_file.close()
    scenario_file.write(scenario.getText())

