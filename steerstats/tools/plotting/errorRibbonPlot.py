

import sys
import matplotlib.pyplot as plt
import numpy as np

sys.path.append('../../')
from util import readCSVToMatrix
file_path = sys.argv[1]
file = open(file_path)

print file

data = readCSVToMatrix(file, delimiter=',')

print data


fig, ax = plt.subplots(1)
ax.fill_between(data[:,0], data[:,1]+data[:,2], data[:,1]-data[:,2], facecolor='blue', alpha=0.5, label='$\pm \sigma$')
ax.plot(data[:,0], data[:,1], alpha=0.7, label='$\mu$')
ax.set_title('Scenario Space convergence for PPR')
ax.legend(loc='upper right')
ax.set_xlabel('number of scenario samples')
ax.set_ylabel('standard deviation $\mu$ $\pm \sigma$')
plt.show()