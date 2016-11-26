

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

# filename = '../../data/optimization/sf/multiObjective/SteerStatsOpt2.csv'
filename = sys.argv[1]
xs = []
ys = []
zs = []
if len(sys.argv) == 2:
    csvfile = open(filename, 'r')
    spamreader = csv.reader(csvfile, delimiter=',')
    xs = []
    ys = []
    for row in spamreader:
        xs.append(float(row[0]))
        ys.append(float(row[1]))
        zs.append(float(row[2]))
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
                        [0, 0, 0]])
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

points = np.array([new_xs])
points = np.append(points, [new_ys], 0)
points = np.append(points, [new_zs], 0)

points = points.transpose()
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

ax1 = plt.axes([0.05,0.05,0.425,0.9], projection='3d')
# ax = fig.gca(projection='3d')
# ax.plot_wireframe(xs, ys, zs, rstride=1, cstride=1)
# print new_xs + new_ys + new_zs
ax1.plot_trisurf(new_xs, new_ys, new_zs, cmap=cm.jet, linewidth=0.2, alpha=0.3)
ax1.view_init(elev=60., azim=0)
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
ax1.set_title("Multi-Objective Optimization")
# plt.axis("tight")
print "len " + str(len(new_xs))
new_xs_b = []
new_ys_b = []
new_zs_b = []
for i in range(len(new_xs)):
    length =  ( new_xs[i] + new_ys[i] + new_zs[i] )
    new_xs_b.append(new_xs[i] / length)
    new_ys_b.append(new_ys[i] / length)
    new_zs_b.append(new_zs[i] / length)

new_xs_b = np.array(new_xs_b)
new_ys_b = np.array(new_ys_b)
new_zs_b = np.array(new_zs_b)
    
# print new_xs_b + new_ys_b + new_zs_b
# ax1.scatter(new_xs_b, new_ys_b, new_zs_b)

for i in range(len(new_xs)):
    xdata = [new_xs[i], new_xs_b[i]]
    ydata = [new_ys[i], new_ys_b[i]]
    zdata = [new_zs[i], new_zs_b[i]]
    # ax1.plot(xdata, ydata, zs=zdata, linestyle='-')
    
# ax2 = fig.add_subplot(122, projection='3d')
ax2 = plt.axes([0.475,0.05,0.475,0.9], projection='3d')

points = np.array([new_xs, new_ys, new_zs])
ax2.scatter(new_xs_b, new_ys_b, 0, s=20.)
ax2.plot_trisurf(bary_coords[:,0], bary_coords[:,1], bary_coords[:,2], cmap=cm.jet, linewidth=0.2, alpha=0.3)

ax2.view_init(elev=90., azim=0)
ax2.text(bary_coords[0,0], bary_coords[0,1], bary_coords[0,2], 'Efficency Metric', (0,0,1))
ax2.text(bary_coords[1,0], bary_coords[1,1], bary_coords[1,2], 'PLE Metric', (0,0,1))
ax2.text(bary_coords[2,0], bary_coords[2,1], bary_coords[2,2], 'Entropy Metric', (0,0,1))
ax2.set_title("Projection on to Barycentric Simplex")


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
for optPoint in T:
    # one point of interest
    x1, y1, _ = proj3d.proj_transform(optPoint[0], optPoint[1], optPoint[2], ax1.get_proj())
    [x1,y1] = ax1.transData.transform((x1, y1))  # convert 2d space to screen space
    # put them in screen space relative to ax0
    x1s.append(x1/width)
    y1s.append(y1/height)
    
    length =  ( optPoint[0] + optPoint[1] + optPoint[2] )
    baryPoint = [optPoint[0] / length, optPoint[1] / length, 0]
    baryPoints.append(baryPoint)
    
    # another point of interest
    x2, y2, _ = proj3d.proj_transform(baryPoint[0], baryPoint[1], baryPoint[2], ax2.get_proj())
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
                                   transform=fig.transFigure)
    lines = lines + (line,)

fig.lines = lines
baryPoints = np.array(baryPoints)
print "barry points " + str(baryPoints)
baryPoint = baryPoints[1]
# converting to barycentric coordinates now
# baryPoint[0] = 
ax2.scatter(baryPoint[0], baryPoint[1], 0, s=20., c='r')


plt.show()

