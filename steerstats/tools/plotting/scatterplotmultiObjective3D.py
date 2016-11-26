

import csv
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
from matplotlib import cm
import sys
import scipy
from scipy.interpolate import bisplrep
from scipy.interpolate import bisplev
import numpy as np

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
            
print "xs = " + str(xs)
print "ys = " + str(ys)
print "zs = " + str(zs)
fig = plt.figure()

ax = fig.add_subplot(111, projection='3d')
ax.scatter(xs, ys, zs, c="b")
ax.set_xlabel('Efficency Metric')
ax.set_ylabel('PLE Metric')
ax.set_zlabel('Entropy Metric')
ax.set_title("Multi-Objective Optimization")

plt.axis("tight")

plt.show()