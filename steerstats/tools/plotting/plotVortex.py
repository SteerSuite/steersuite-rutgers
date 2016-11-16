

import csv
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
from matplotlib import cm
import sys
import scipy
from scipy.interpolate import bisplrep
from scipy.interpolate import bisplev
import numpy as np
import math
sys.path.append('../../')
from util import writeEntropyMetricData

velocity = 1.33
def f(x):
    y=x
    return y

def f2(x):
    y = math.cos(x)/3.6
    return y

def f3(x):
    y = math.exp(x*4)
    return y

def f4(x):
    y = float(x)
    y = ((y)) / (y+0.3)
    return y

def rotateInXZPlane( dir, angle):
    s = math.sin(angle)
    c = math.cos(angle)
    return np.array((dir * c) + (dir.z * s),  (dir.y),  (dir.z * c) - (dir.x * s))

    
distance = 30.9
points = 100.0
fps = 16.0
tocm = 100.0
scale = 10
shift = scale * tocm

seconds = distance/velocity
points = math.floor(seconds*fps)
print "points: " + str(points)
x = np.array(np.arange((points)))/float(points)
x2 = np.array(np.arange(points))/float(points)

print "len x " + str(len(x))
y = np.array(map(f4, x*(math.pi/2.0)))
# y = y / np.amax(y)
# print y

fig = plt.figure()
ax = fig.add_subplot(111)
# x=x*2.0
# y=(y)+x2
# y=y -0.5
# y=y*2.0
points = np.append([x], [y], axis=0).transpose()
ax.plot(points[:,0], points[:,1], linewidth=2.0)
# ax.set_ylim([0.0,1.0])
# ax.set_xlim([-1.0,1.0])
# plt.show()
points = (points-0.5)*2.0*scale*tocm
# print points
distance = 0.0
for i in range(len(points)-1):
    distance = distance + np.linalg.norm(points[i]-points[i+1])
    
print "distance: " + str(distance)
agentData = []
agentData.append(np.array(points))
agentData.append(np.array(points+tocm))
agentData.append(np.array(points+-tocm))
tmp_points = np.array(points *-1.0, copy=True)
agentData.append(np.array(tmp_points))
agentData.append(np.array(tmp_points+tocm))
agentData.append(np.array(tmp_points+-tocm))

points[:,0] = points[:,0] * -1.0
points2= np.array(points, copy=True )
length = len(points2)-1
for i in range(len(points2)):
    # point[1] = point[1] + (((shift+point[0])-point[1]))
    points[i] = points2[length-i]
agentData.append(np.array(points))
tmp_points = np.array(points, copy=True)
tmp_points[:,0] = tmp_points[:,0] + tocm
tmp_points[:,1] = tmp_points[:,1] - tocm
agentData.append(np.array(tmp_points))
agentData.append(np.array(points + (points - tmp_points)))

points = points *-1.0
agentData.append(np.array(points))
tmp_points = np.array(points, copy=True)
tmp_points[:,0] = tmp_points[:,0] + tocm
tmp_points[:,1] = tmp_points[:,1] - tocm
agentData.append(np.array(tmp_points))
agentData.append(np.array(points + (points - tmp_points)))


for agent in agentData:
    # print agent
    ax.plot(agent[:,0], agent[:,1], linewidth=2.0)
    
file = open('entropyData.txt', 'w')
writeEntropyMetricData(agentData, file)
file.close()

plt.show()