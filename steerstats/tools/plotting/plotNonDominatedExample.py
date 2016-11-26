

from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
from matplotlib import cm
import matplotlib
import numpy as np



points = [[0.85,0.05]]
points.append([0.55, 0.11])
points.append([0.4, 0.2])
points.append([0.28, 0.34])
points.append([0.20, 0.5])
points.append([0.125, 0.7])
points.append([0.05, 0.95])

points = np.array(points)

fig = plt.figure()
ax1 = fig.add_subplot(111)
ax1.set_xlim([0.0, 1.1])
ax1.set_ylim([0.0, 1.1])
for point in points:
    ax1.add_patch(matplotlib.patches.Rectangle((1,1),point[0]-1,point[1]-1, color='#0099FF', alpha=0.5))
    ax1.scatter(point[0], point[1])
# Nadir point
ax1.scatter(1.0, 1.0, color='red')
# contribution area
ax1.scatter(points[4,0], points[4,1], color='green', s=120.0)
ax1.add_patch(matplotlib.patches.Rectangle(points[4],points[3,0]-points[4,0],points[5,1]-points[4,1], color='#009900', alpha=0.6))
# POF
ax1.plot(points[:,0], points[:,1])

ax1.set_xlabel('objective 1', fontsize=18)
ax1.set_ylabel('objective 2', fontsize=18)
plt.show()
