

import csv
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
from matplotlib import cm
import sys
import scipy
from scipy.interpolate import bisplrep
from scipy.interpolate import bisplev
import numpy as np
sys.path.append('../../')
from util import readCSVDictToMutliObjData
from util import getParetoFront

# filename = '../../data/optimization/sf/multiObjective/SteerStatsOpt2.csv'
filename = sys.argv[1]
xs = []
ys = []
zs = []
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
elif len(sys.argv) == 3:
    for i in range(1, int(sys.argv[2])):
        tmp_filename = filename + str(i) + ".csv"
        csvfile = open(tmp_filename, 'r')
        spamreader = csv.reader(csvfile, delimiter=',')
        for row in spamreader:
            xs.append(float(row[0]))
            ys.append(float(row[1]))
            zs.append(float(row[2]))
            
print "xs = " + str(xs)
print "ys = " + str(ys)
print "zs = " + str(zs)
fig = plt.figure()

x_min = np.amin(xs)
x_max = np.amax(xs)
y_min = np.amin(ys)
y_max = np.amax(ys)
z_min = np.amin(zs)
z_max = np.amax(zs)
new_xs = (xs - x_min) / (x_max - x_min) 
new_ys = (ys - y_min) / (y_max - y_min)
new_zs = (zs - z_min) / (z_max - z_min)

tri = np.array([[1, 0, 0],
                [0, 1, 0],
                [0, 0, 1]])

ax = fig.add_subplot(111, projection='3d')
# ax = fig.gca(projection='3d')
# ax.plot_wireframe(xs, ys, zs, rstride=1, cstride=1)
ax.plot_trisurf(new_xs, new_ys, new_zs, cmap=cm.jet, linewidth=0.1)
# ax.plot_trisurf(tri[:,0], tri[:,1], tri[:,2], linewidth=0.2)
ax.set_xlim([0.0, 1.0])
ax.set_ylim([0.0, 1.0])
ax.set_zlim([0.0, 1.0])
ax.set_xlabel('Efficency Metric', fontsize=18)
ax.set_ylabel('PLE Metric', fontsize=18)
ax.set_zlabel('Entropy Metric', fontsize=18)
# ax.set_title("Multi-Objective Optimization")
plt.axis("tight")

plt.show()

