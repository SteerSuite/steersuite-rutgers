

import csv
from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
from matplotlib import cm
import sys
sys.path.append('../../')
import scipy
from scipy.interpolate import bisplrep
from scipy.interpolate import bisplev
import numpy as np
from util import getParetoFront

filename = sys.argv[1]
data = []
last_data = []
invalid_ind = []
if len(sys.argv) == 2:
    csvfile = open(filename, 'r')
    spamreader = csv.reader(csvfile, delimiter=',')
    for row in spamreader:
        last_data.append([float(j) for j in row[:2]])
        invalid_ind.append([float(j) for j in row[2:]])
elif len(sys.argv) == 3:
    for i in range(1, int(sys.argv[2])-1):
        tmp_filename = filename + str(i) + ".csv"
        csvfile = open(tmp_filename, 'r')
        spamreader = csv.reader(csvfile, delimiter=',')
        for row in spamreader:
            data.append([float(j) for j in row[:2]])
    tmp_filename = filename + str(i+1) + ".csv"
    csvfile = open(tmp_filename, 'r')
    spamreader = csv.reader(csvfile, delimiter=',')
    for row in spamreader:
        last_data.append([float(j) for j in row[:2]])
        invalid_ind.append([float(j) for j in row[2:]])
            
# print "xs = " + str(xs)
# print "ys = " + str(ys)
fig = plt.figure()

last_data = np.array(last_data)
data = np.array(data)

front = getParetoFront(last_data, invalid_ind=invalid_ind)

ax = fig.add_subplot(111)
ax.scatter(data[:,0], data[:,1], c="w", marker='o', label="samples")
ax.scatter(last_data[:,0], last_data[:,1], c='b', marker='D', label="last iteration")
# ax.plot(last_data[:,0], last_data[:,1], c="b")
ax.plot(front[:,0], front[:,1], 'g.-', label="pareto front", linewidth=2.0)

ax.set_xlabel('Efficency', fontsize=18)
ax.set_ylabel('Effort', fontsize=18)
# ax.set_title("Multi-Objective Optimization")

# ax.legend()

plt.axis("tight")

plt.show()
