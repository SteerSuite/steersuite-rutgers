"""
Matplotlib Animation Example

author: Jake Vanderplas
email: vanderplas@astro.washington.edu
website: http://jakevdp.github.com
license: BSD
Please feel free to use and modify this, but keep the above information. Thanks!
"""

import numpy as np
from matplotlib import pyplot as plt
from matplotlib import animation
import matplotlib
import random
import sys
import csv
sys.path.append('../../../')
from util import getParetoFront

filename = sys.argv[1]
data = []
for i in range(1, int(sys.argv[2])):
    tmp_filename = filename + str(i) + ".csv"
    csvfile = open(tmp_filename, 'r')
    spamreader = csv.reader(csvfile, delimiter=',')
    tmp_data = []
    for row in spamreader:
        tmp_data.append(row)
        # tmp_data.append([float(row[0]), float(row[1]), float(row[2])])
    data.append(tmp_data)

data = np.array(data)
up = 2
low = 0

# First set up the figure, the axis, and the plot element we want to animate
fig = plt.figure()
ax = fig.add_subplot(111)
ax.set_ylim([0.4,0.8])
ax.set_xlim([0.65,1])
ax.set_xlabel('Efficiency')
ax.set_ylabel('Effort')
ax.set_title('Pareto Optimal Front Estimation with ORCA')

# ax = plt.axes(xlim=(low, up), ylim=(low, up))
# ax = plt.axes(xlim=(0.9, 1.0), ylim=(0, 1))
scat1 = ax.scatter([3], [4], c="b")
line, = ax.plot([], [], lw=2, c='g')

# initialization function: plot the background of each frame
def init():
    print "paths"
    # print scat.get_paths()
    # sys.exit()
    # scat.set_paths(matplotlib.path.Path([[2, 3]]))
    return scat1, line

# animation function.  This is called sequentially
def animate(i):
    # scat1.set_paths(np.array(tmp_data))
    # scat.set_paths([x, y])
    # print scat.get_offsets()
    tmp_data=np.array(data[i], dtype='d')
    # print tmp_data[:, 1:3]
    # scat1.set_paths(tmp_data)
    scat1.set_offsets(tmp_data)
    fitnesses = tmp_data[:,0:2]
    parameters = tmp_data[:,2:]
    # print tmp_data
    # print fitnesses
    # print parameters
    front = getParetoFront(fitnesses, parameters)
    line.set_data(front[:,0], front[:,1])
    return  scat1, line

# call the animator.  blit=True means only re-draw the parts that have changed.
anim = animation.FuncAnimation(fig, animate, init_func=init,
                               frames=200, interval=100, blit=True)

# save the animation as an mp4.  This requires ffmpeg or mencoder to be
# installed.  The extra_args ensure that the x264 codec is used, so that
# the video can be embedded in html5.  You may need to adjust this for
# your system: for more information, see
# http://matplotlib.sourceforge.net/api/animation_api.html

anim.save('basic_animation.mp4', fps=10, extra_args=['-vcodec', 'libx264'])

plt.show()