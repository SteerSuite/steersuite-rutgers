
from subspace import *

if __name__ == '__main__':
    scenario = hallway_one_way(-5,0)
    print scenario
    scenarioFile = open("hallway-one-way-space.xml","w")
    scenarioFile.write(scenario.getTextPretty())