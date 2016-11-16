

import csv
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
from matplotlib import cm
import matplotlib
import sys
import scipy
from scipy.interpolate import bisplrep
from scipy.interpolate import bisplev
import numpy as np
import math
from mpl_toolkits.mplot3d import proj3d
sys.path.append('../../')
from util import barycentric
from util import closestPoints
from util import convertToBarycentric
from util import closestVertices
from util import readCSVDictToMutliObjData
from util import getParetoFront
from util import saveMatrixToCVSDict
from util import normalizedInverseDistance

# filename = '../../data/optimization/sf/multiObjective/SteerStatsOpt2.csv'

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
    x_i=np.array([1, 1, 1])
    y_i=np.array([1, 1, 1])
    z_i=np.array([1, 1, 1])
    
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
    x_i=np.array([0, 0, 0])
    y_i=np.array([0, 0, 0])
    z_i=np.array([0, 0, 0])
    
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
    print "min **** " + str(_min)
    print "max **** " + str(_max)
    out = (data - _min) / (_max - _min) 
    return out
    
def normalizeBarycentricInCartesian(T, data):
    p0 = T[0]
    p1 = T[1]
    p2 = T[2]
    
    return out
    
filename = sys.argv[1]
xs = []
ys = []
zs = []
parameters = []
if len(sys.argv) == 2:
    dataFile = open(filename, "r")
    weighties = dataFile.readline()[:-1].split(',')
    dataFile.close()
    dataFile = open(filename, "r")
    fitnesses, parameters = readCSVDictToMutliObjData(dataFile, 3, weighties) 
    dataFile.close()
    
    front = getParetoFront(fitnesses, parameters)
    print "front " + str(len(front))
    print front
    xs = fitnesses[:,0]
    ys = fitnesses[:,1]
    zs = fitnesses[:,2]
        	

            
# print "xs = " + str(xs)
# print "ys = " + str(ys)
# print "zs = " + str(zs)

bary_coords = np.array([[1, 0, 0],
                        [0, 1, 0],
                        [0, 0, 1]])
fig = plt.figure()

# a background axis to draw lines on
ax0 = plt.axes([0.,0.,1.,1.])
ax0.set_xlim(0,1)
ax0.set_ylim(0,1)


# use these to know how to transform the screen coords
dpi = ax0.figure.get_dpi()
height = ax0.figure.get_figheight() * dpi
width = ax0.figure.get_figwidth() * dpi

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
# print points
# print points.max(axis=0)
# print np.amax(points, axis=0)

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
# print T

ax1 = plt.axes([0.05,0.05,0.425,0.9], projection='3d')
# ax = fig.gca(projection='3d')
# ax.plot_wireframe(xs, ys, zs, rstride=1, cstride=1)
# print new_xs + new_ys + new_zs
ax1.plot_trisurf(new_xs, new_ys, new_zs, cmap=cm.jet, linewidth=0.2, alpha=0.3)
ax1.scatter(new_xs, new_ys, new_zs, alpha=0.3, s=20.)
ax1.view_init(elev=45., azim=45.)
# ax1.grid(False)
ax1.set_xlim(0,1)
ax1.set_ylim(0,1)
ax1.set_zlim(0,1)

# ax1.get_xaxis().set_ticks([])
# ax1.get_yaxis().set_ticks([])
# ax1.get_zaxis().set_ticks([])
# ax1.set_axis_off()
# ax1.text(T[0,0], T[0,1], T[0,2], 'Efficency Min', (0,0,1))
# ax1.text(T[1,0], T[1,1], T[1,2], 'PLE Min', (0,0,1))
# ax1.text(T[2,0], T[2,1], T[2,2], 'Entropy Min', (0,0,1))
# ax.plot_trisurf(tri[:,0], tri[:,1], tri[:,2], linewidth=0.2)
# ax1.set_xlim([0.0, 1.0])
# ax1.set_ylim([0.0, 1.0])
# ax1.set_zlim([0.0, 1.0])
# ax1.set_xlabel('Efficency Value')
# ax1.set_ylabel('PLE Value')
# ax1.set_zlabel('Entropy Value')
# ax1.set_title("Pareto Optimal Front")
# plt.axis("tight")
print "len " + str(len(new_xs))

    
    
# ax2 = fig.add_subplot(122, projection='3d')
ax2 = plt.axes([0.475,0.05,0.475,0.9])
# ax2.scatter(new_xs_b, new_ys_b, new_zs_b)
pointsBary = barycentricRelative(T, pointsEuclid)
# pointsBary = normalize(pointsBary)
points = np.array([new_xs, new_ys, new_zs])
# ax2.scatter(pointsBary[:,0], pointsBary[:,1], pointsBary[:,2], s=20.)

pointsBary = barycentricProjection(pointsBary)
pointsBary = 1-pointsBary
spans = getSpans2(pointsBary)

trueBary = barycentric(getSpans2(pointsBary), pointsBary)
# trueBary = pointsBary
spanies = getSpans2(trueBary)

# ax2.scatter(spans[:,0], spans[:,1], spans[:,2], s=50., c='r')
# ax2.plot_trisurf(spanies[:,0], spanies[:,1], spanies[:,2], cmap=cm.jet, linewidth=0.2, alpha=0.3)
# ax2.scatter(trueBary[:,0], trueBary[:,1], trueBary[:,2], s=20.)
poly = matplotlib.patches.Polygon([[0,1],[1,0],[0,0]],edgecolor='none', color='#0000FF', alpha=0.5)
ax2.add_patch(poly)
ax2.set_xlim(0,1)
ax2.set_ylim(0,1)

ax2.scatter(trueBary[:,0], trueBary[:,1], s=20.)
# ax2.text(spanies[0,0], spanies[0,1], spanies[0,2], 'Efficency Min', (0,0,1))
# ax2.text(spanies[1,0], spanies[1,1], spanies[1,2], 'PLE Min', (0,0,1))
# ax2.text(spanies[2,0], spanies[2,1], spanies[2,2], 'Entropy Min', (0,0,1))
# ax2.set_xlabel('Efficency Weight')
# ax2.set_ylabel('PLE Weight')
# ax2.set_zlabel('Entropy Weight')
# ax2.set_axis_off()
# ax2.plot_trisurf(bary_coords[:,0], bary_coords[:,1], bary_coords[:,2], cmap=cm.jet, linewidth=0.2, alpha=0.3)

# ax2.view_init(elev=45., azim=45.)
# ax2.set_title("Projection on to Barycentric Simplex")


# I think I need some kind of intermediate transformation.
"""
    First transformation repositions the points so that the points with weight
    1 are positioned at the barycentric coordinates.
    The second transformation will need to scale the points to the the realtive 
    distances befor the transformations are preserved.
"""

x1s = []
y1s = []
x2s = []
y2s = []

baryPoints = []
for optPoint, baryPoint in zip(pointsEuclid, trueBary):
    # one point of interest
    x1, y1, _ = proj3d.proj_transform(optPoint[0], optPoint[1], optPoint[2], ax1.get_proj())
    [x1,y1] = ax1.transData.transform((x1, y1))  # convert 2d space to screen space
    # put them in screen space relative to ax0
    x1s.append(x1/width)
    y1s.append(y1/height)
    
    
    # another point of interest
    # x2, y2, _ = proj3d.proj_transform(baryPoint[0], baryPoint[1], baryPoint[2], ax2.get_proj())
    x2, y2 = baryPoint[0], baryPoint[1]
    [x2,y2] = ax2.transData.transform((x2, y2))  # convert 2d space to screen space
    x2s.append(x2/width)
    y2s.append(y2/height)

# set all these guys to invisible (needed?, smartest way?)
for item in [fig, ax1, ax2]:
    item.patch.set_visible(False)

transFigure = fig.transFigure.inverted()

lines = ()
for x1, y1, x2, y2 in zip(x1s, y1s, x2s, y2s):
    coord1 = transFigure.transform(ax0.transData.transform([x1,y1]))
    coord2 = transFigure.transform(ax0.transData.transform([x2,y2]))
    
    line = matplotlib.lines.Line2D((coord1[0],coord2[0]),(coord1[1],coord2[1]),
                                   transform=fig.transFigure, alpha=0.3, c='b')
    lines = lines + (line,)

fig.lines = lines

# pointsBary are really still in Euclidien space
trueBary = barycentric(getSpans2(pointsBary), pointsBary)
spanies = getSpans2(pointsBary)


# print getSpans2(pointsBary)
# print trueBary
# print np.sum(trueBary, axis=1)

weight = [0.01, 0.45, 0.54]
# weight = [0.2, 0.3, 0.5]
# weight = [1.3, 1.3, 1.4]
# ax2.scatter(weight[0], weight[1], weight[2], s=40., c='r')

closest = closestPoints(trueBary, weight)

# ax2.scatter(closest[:,0], closest[:,1], closest[:,2], s=150.0, c='y', marker='D')

verts = closestVertices(trueBary, weight)
print "verticies " + str(verts)
print "points " + str(closest)
print "points "
for vert in verts:
    print parameters[vert]
    
blendWeight = normalizedInverseDistance(closest, weight)
print "blendWeights " + str(blendWeight)  + " = " + str(np.sum(blendWeight))


bary_points = []
bary_points.append([0.547199,0.767135,0.0])
bary_points.append([0.700501,-0.686652,0.0])
bary_points.append([-0.76148,-0.195796,0.0])
bary_points = np.array(bary_points)
weight2 = np.array([0.5,0.5,0.0])

# print "blendWeights " + str(convertToBarycentric(bary_points, weight2))
         
# for tup in hull2:
  #  print trueBary[:,:-1][tup[0]], trueBary[:,:-1][tup[1]]
  
weightzes = []
for i in range(3):
    weightzes.append('w'+str(i))

weightzes.extend(weighties)


file = open('test.csv', 'w')

finalData = np.append(trueBary, fitnesses, axis=1)
finalData = np.append(finalData, parameters, axis=1)
saveMatrixToCVSDict(finalData, file, weightzes)

file.close()
# plt.tight_layout()
plt.show()

