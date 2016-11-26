from numpy  import *
import sys
import random
sys.path.append("../../")
from steersuite.TestCase import TestCase
sys.path.append("../")
from tools.gameobjects.vector3 import Vector3 as vector

# sudo apt-get install python-visual
# http://vpython.org/contents/docs/vector.html

agents = 240
vec1 = vector(1,0,0)
axis = vector(0,1,0)
thetaPart = float(360) / agents
# this is almost more like radius
# scale = float(30)
extra_space = 1.1
noise_amount=0.05
scale = (agents*extra_space)/(2*pi) # calculates circumference and gives %15 extra space

scenario = TestCase()
    # print map_meta_data
    
xmax = float(100)/2
zmax = float(100)/2
version = 1.0
scenario.addHeader("concentric circles", 1.0, -xmax, xmax, 0, 0, -zmax, zmax)
add_noise=True
    
positions=[]
for i in range(agents):
    theta = radians(i*thetaPart)
    vectmp = rotate(vector=vec1, angle=theta, axis=axis)*scale
    if add_noise:
       vectmp = vectmp * (1 +  random.uniform(0, noise_amount)) 
    positions.append(vectmp)
    scenario.addAgent(name="blah"+str(i), radius=0.5, agentPos=vectmp, directionV=[1, 0, 0],
                  speed=0, targetLocationV=vectmp*-1, desiredSpeed=1.25, timeDuration=1000.0)

# for vec in positions:
    # print vec
    
# print scenario.getText()
scenarioFile = open("concentric-circles.xml","w")
scenarioFile.write(scenario.getTextPretty())