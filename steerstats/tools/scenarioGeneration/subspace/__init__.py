from numpy  import *
import sys
import random
sys.path.append("../../")
from steersuite.TestCase import TestCase
from steersuite.AxisAlignedBox import AxisAlignedBox
from util import *

sys.path.append("../")
from tools.gameobjects.vector3 import Vector3 as vector


__all__ = ['hallway_one_way', 'numpy_array_to_params']

def hallway_one_way(args, samples=1):
    
    pillars = chunks(args, 2)
    # print "args to hallway " + str(args)
    # print "pillars " + str(pillars)
    numAgents = 50
    vec1 = vector(1,0,0)
    axis = vector(0,1,0)

    box = AxisAlignedBox(0, 30, 0, 1, -5, 5)    
    # this is almost more like radius
    # scale = float(30)
    scenarios=[]
    seed=10
    random.seed(seed)
    for i in range(samples):
        scenario = TestCase()
            # print map_meta_data
        
        
        xmax = float(100)/2
        zmax = float(100)/2
        version = 1.0
        scenario.addHeader("hallway one way", 1.0, -xmax, xmax, 0, 0, -zmax, zmax)
        add_noise=True
        seed = sample_agents(scenario, box, numAgents)
        # for vec in positions:
            # print vec
            
        scenario.addObstacle(box.get_xmin()-20, box.get_xmax()+20, 
                             0, 1, box.get_zmin()-5, box.get_zmin())
        
        scenario.addObstacle(box.get_xmin()-20, box.get_xmax()+20, 
                             0, 1, box.get_zmax(), box.get_zmax()+5)
    
        for pillar in pillars:
            # print "pillar is " + str(pillar)
            scenario.addCircleObstacle(pillar[0], 0, pillar[1], 0.65, 1)
            
        scenarios.append(scenario)
    
    # print scenario.getAgents()
    return scenarios

def sample_agents(scenario, box, numAgents, seed=10):
    radius=0.5
    target = vector(-15,0,0)
    for i in range(numAgents):
        vectmp = scenario.randPositionInWithoutColisions(box.get_xmin(), box.get_xmax(),
                                                         box.get_zmin(), box.get_zmax(), 
                                                         radius) 
        scenario.addAgent(name="agent"+str(i), radius=radius, agentPos=vectmp, directionV=[1, 0, 0],
                      speed=0, targetLocationV=target, desiredSpeed=1.33, timeDuration=1000.0)

def numpy_array_to_params(data):
    if len(data) == 0:
        return '()'
    out='('
    for x in data[:-1]:
        out = out + str(x)+';'
    out = out + str(data[-1])+')'
    return out
    
    
    
