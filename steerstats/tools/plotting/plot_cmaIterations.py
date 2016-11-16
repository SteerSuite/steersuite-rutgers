
"""

    Run like: python plot_cmaIterations.py ../../../../crowd-analysis/steerstats/data/optimization/footstep/overDist/outcmaxrecentbest.dat
"""

import sys
import matplotlib.pyplot as plt

sys.path.append('../../')
from util import readCSVToMatrix
file_path = sys.argv[1]
file = open(file_path)

print file

data = readCSVToMatrix(file, delimiter=' ')

print data[:,0]

colour = 'b'
_markerSize=9
_lineWidth=2

p_0, = plt.plot(data[:,0], data[:,4], color = colour, marker='o', markersize=_markerSize, linewidth=_lineWidth)
plt.show()

for param in range(5,len(data[0])):
    # print param
    # print data[0:param]
    # print data[0:param]
    p_0, = plt.plot(data[:,0], data[:,param], color = colour, marker='o', markersize=_markerSize, linewidth=_lineWidth)
    
    
plt.show()

