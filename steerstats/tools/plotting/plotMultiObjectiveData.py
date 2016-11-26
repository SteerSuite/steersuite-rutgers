

import csv
import matplotlib.pyplot as plt
import sys
import numpy as np

# filename = '../../data/optimization/sf/multiObjective/SteerStatsOpt2.csv'
filename = sys.argv[1]
xs = []
ys = []
if len(sys.argv) == 2:
    csvfile = open(filename, 'r')
    spamreader = csv.reader(csvfile, delimiter=',')
    xs = []
    ys = []
    for row in spamreader:
        xs.append(float(row[0]))
        ys.append(float(row[1]))
elif len(sys.argv) == 3:
    for i in range(1, int(sys.argv[2])):
        tmp_filename = filename + str(i) + ".log"
        csvfile = open(tmp_filename, 'r')
        spamreader = csv.reader(csvfile, delimiter=',')
        for row in spamreader:
            xs.append(float(row[0]))
            ys.append(float(row[1]))
    
else:
    print "Wrong"
    sys.exit()
print "xs = " + str(xs)
print "ys = " + str(ys)
x_min = np.amin(xs)
x_max = np.amax(xs)
y_min = np.amin(ys)
y_max = np.amax(ys)
new_xs = (xs - x_min) / (x_max - x_min) 
new_ys = (ys - y_min) / (y_max - y_min) 
fig = plt.figure()
ax = fig.add_subplot(111)    
ax.plot(new_xs, new_ys, c="b")
# ax.set_xlim([np.amin(xs), np.amax(xs)])
# ax.set_ylim([np.amin(ys), np.amax(ys)])
ax.set_xlim([0.0, 1.0])
ax.set_ylim([0.0, 1.0])
ax.set_xlabel('efficency metric')
ax.set_ylabel('PLE metric')
ax.set_title("multi-objective optimization")
# plt.axis("tight")
plt.show()
