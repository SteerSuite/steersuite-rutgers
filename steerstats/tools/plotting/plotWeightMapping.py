

import csv
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
from matplotlib import cm
import matplotlib
import sys
sys.path.append('../../')
import scipy
from scipy.interpolate import bisplrep
from scipy.interpolate import bisplev
import numpy as np
import math
from mpl_toolkits.mplot3d import proj3d
from util import getParetoFront
from util import closestPoints
# filename = '../../data/optimization/sf/multiObjective/SteerStatsOpt2.csv'

"""
    First transformation repositions the points so that the points with weight
    1 are positioned at the barycentric coordinates.
    The second transformation will need to scale the points to the the realtive 
    distances befor the transformations are preserved.
"""

def barycentricProjection(data):
    new_xs_b = []
    new_ys_b = []
    new_zs_b = []
    for i in range(len(data)):
        length =  ( data[i][0] + data[i][1] + data[i][2] )
        new_xs_b.append(data[i][0] / length)
        new_ys_b.append(data[i][1] / length)
        new_zs_b.append(data[i][2] / length)
    
    new_xs_b = np.array(new_xs_b)
    new_ys_b = np.array(new_ys_b)
    new_zs_b = np.array(new_zs_b)
    return np.array([new_xs_b, new_ys_b, new_zs_b]).transpose()
    

def barycentricRelative(T, data):
    p0 = T[0]
    p1 = T[1]
    p2 = T[2]
    
    out = [] 
    for row in data:
        x = row[0]
        y = row[1]
        z = row[2]
        tmp_out = []
        tmp_out.append(np.linalg.norm(p0-row))
        tmp_out.append(np.linalg.norm(p1-row))
        tmp_out.append(np.linalg.norm(p2-row))
        
        out.append(tmp_out)
    
    return np.array(out)

def getSpans(data):
    x_i=np.array([100000, 100000, 100000])
    y_i=np.array([100000, 100000, 100000])
    z_i=np.array([100000, 100000, 100000])
    
    for row in data:
        if row[0] < x_i[0]:
            x_i = row
        if row[1] < y_i[1]:
            y_i = row
        if row[2] < z_i[2]:
            z_i = row
        
    T = []
    T.append(x_i)
    T.append(y_i)
    T.append(z_i)
    T = np.array(T)
    return T

def getSpans2(data):
    x_i=np.array([-100000, -100000, -100000])
    y_i=np.array([-100000, -100000, -100000])
    z_i=np.array([-100000, -100000, -100000])
    
    for row in data:
        if row[0] > x_i[0]:
            x_i = row
        if row[1] > y_i[1]:
            y_i = row
        if row[2] > z_i[2]:
            z_i = row
        
    T = []
    T.append(x_i)
    T.append(y_i)
    T.append(z_i)
    T = np.array(T)
    return T

def normalize(data):
    
    _min = np.amin(data)
    _max = np.amax(data)
    out = (data - _min) / (_max - _min) 
    return out
    
    
filename = sys.argv[1]
xs = []
ys = []
zs = []
invalid_ind = []
if len(sys.argv) == 2:
    csvfile = open(filename, 'r')
    spamreader = csv.reader(csvfile, delimiter=',')
    xs = []
    ys = []
    for row in spamreader:
        xs.append(float(row[0]))
        ys.append(float(row[1]))
        zs.append(float(row[2]))
        invalid_ind.append([float(j) for j in row[3:]])
        
elif len(sys.argv) == 3:
    for i in range(1, int(sys.argv[2])):
        tmp_filename = filename + str(i) + ".log"
        csvfile = open(tmp_filename, 'r')
        spamreader = csv.reader(csvfile, delimiter=',')
        for row in spamreader:
            xs.append(float(row[0]))
            ys.append(float(row[1]))
            zs.append(float(row[2]))
            

            
# print "xs = " + str(xs)
# print "ys = " + str(ys)
# print "zs = " + str(zs)

bary_coords = np.array([[1, 0, 0],
                        [0, 1, 0],
                        [0, 0, 1]])
fig = plt.figure()

# a background axis to draw lines on

pointsOriginal = np.array([xs])
pointsOriginal = np.append(pointsOriginal, [ys], 0)
pointsOriginal = np.append(pointsOriginal, [zs], 0).transpose()

front = getParetoFront(pointsOriginal, invalid_ind)
print "Pareto Front" + str(front)
# use these to know how to transform the screen coords
xs = front[:,0]
ys = front[:,1]
zs = front[:,2]
x_min = np.amin(xs)
x_max = np.amax(xs)
y_min = np.amin(ys)
y_max = np.amax(ys)
z_min = np.amin(zs)
z_max = np.amax(zs)
new_xs = (xs - x_min) / (x_max - x_min) 
new_ys = (ys - y_min) / (y_max - y_min)
new_zs = (zs - z_min) / (z_max - z_min)

"""
new_xs = np.append(new_xs, float(0.05))
new_ys = np.append(new_ys, float(0.05))
new_zs = np.append(new_zs, float(0.95))
new_xs = np.append(new_xs, float(0.95))
new_ys = np.append(new_ys, float(0.95))
new_zs = np.append(new_zs, float(0.05))
"""
pointsEuclid = []
pointsEuclid.append(new_xs)
pointsEuclid.append(new_ys)
pointsEuclid.append(new_zs)
pointsEuclid = np.array(pointsEuclid).transpose()

points = np.array([new_xs])
points = np.append(points, [new_ys], 0)
points = np.append(points, [new_zs], 0)

points = points.transpose()

front = getParetoFront(points, invalid_ind)
print points
print points.max(axis=0)
print np.amax(points, axis=0)

x_i=np.array([1, 1, 1])
y_i=np.array([1, 1, 1])
z_i=np.array([1, 1, 1])

for row in points:
    if row[0] < x_i[0]:
        x_i = row
    if row[1] < y_i[1]:
        y_i = row
    if row[2] < z_i[2]:
        z_i = row
    

T = np.array([x_i, y_i, z_i])
print T

ax1 = fig.add_subplot(111, projection='3d')
# ax = fig.gca(projection='3d')
# ax.plot_wireframe(xs, ys, zs, rstride=1, cstride=1)
# print new_xs + new_ys + new_zs
ax1.plot_trisurf(new_xs, new_ys, new_zs, cmap=cm.jet, linewidth=0.2, alpha=0.3)
ax1.scatter(new_xs, new_ys, new_zs, alpha=0.3, s=20.)
ax1.view_init(elev=45., azim=45.)
ax1.text(T[0,0], T[0,1], T[0,2], 'Efficency Min', (0,0,1))
ax1.text(T[1,0], T[1,1], T[1,2], 'PLE Min', (0,0,1))
ax1.text(T[2,0], T[2,1], T[2,2], 'Entropy Min', (0,0,1))
# ax.plot_trisurf(tri[:,0], tri[:,1], tri[:,2], linewidth=0.2)
# ax1.set_xlim([0.0, 1.0])
# ax1.set_ylim([0.0, 1.0])
# ax1.set_zlim([0.0, 1.0])
ax1.set_xlabel('Efficency Metric')
ax1.set_ylabel('PLE Metric')
ax1.set_zlabel('Entropy Metric')
ax1.set_title("Pareto Optimal front from Multi-Objective Optimization")
# plt.axis("tight")
print "len " + str(len(new_xs))

    # ax1.plot(xdata, ydata, zs=zdata, linestyle='-')
    
# ax2 = fig.add_subplot(122, projection='3d')
pointsBaryR = barycentricRelative(T, pointsEuclid)
pointsBaryR = normalize(pointsBaryR)
points = np.array([new_xs, new_ys, new_zs])
# ax1.scatter(pointsBaryR[:,0], pointsBaryR[:,1], pointsBaryR[:,2], s=20.)

pointsBary = barycentricProjection(pointsBaryR)
pointsBary = 1-pointsBary
# pointsBary = pointsBary + 1
spans = getSpans2(pointsBary)

# ax2.scatter(spans[:,0], spans[:,1], spans[:,2], s=50., c='r')
ax1.plot_trisurf(spans[:,0], spans[:,1], spans[:,2], cmap=cm.jet, linewidth=0.2, alpha=0.3)
ax1.scatter(pointsBary[:,0], pointsBary[:,1], pointsBary[:,2], s=20.)
ax1.text(spans[0,0], spans[0,1], spans[0,2], 'Efficency Min', (0,0,1))
ax1.text(spans[1,0], spans[1,1], spans[1,2], 'PLE Min', (0,0,1))
ax1.text(spans[2,0], spans[2,1], spans[2,2], 'Entropy Min', (0,0,1))

for p1, p2 in zip(pointsEuclid, pointsBary):
    ax1.plot([p1[0], p2[0]], [p1[1], p2[1]], [p1[2], p2[2]], c='b', alpha=0.5)


weight = [0.3, 0.3, 0.4]
ax1.scatter([0.3], [0.3], [0.4], s=40., c='r')

closest = closestPoints(pointsBary, weight)
print "closest " + str(closest)
ax1.scatter(closest[:,0], closest[:,1], closest[:,2], s=40., c='g', marker='+')


plt.show()

