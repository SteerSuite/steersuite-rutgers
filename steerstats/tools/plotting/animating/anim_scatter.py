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
from mpl_toolkits.mplot3d import Axes3D
import matplotlib
import random
import sys
import csv

filename = sys.argv[1]
data = []
for i in range(1, int(sys.argv[2])):
    tmp_filename = filename + str(i) + ".csv"
    csvfile = open(tmp_filename, 'r')
    spamreader = csv.reader(csvfile, delimiter=',')
    tmp_data = []
    for row in spamreader:
        tmp_data.append([float(row[0]), float(row[1]), float(row[2])])
        # tmp_data.append([float(row[0]), float(row[1]), float(row[2])])
    data.append(tmp_data)

data = np.array(data)
up = 2
low = 0

# First set up the figure, the axis, and the plot element we want to animate
fig = plt.figure()
ax = fig.add_subplot(231)
ax.set_xlabel('Efficency Metric')
ax.set_ylabel('PLE Metric')
ax.set_xlim([0.9,1])
ax.set_ylim([0.1,0.9])
ax2 = fig.add_subplot(232)
ax2.set_xlabel('PLE Metric')
ax2.set_ylabel('Entropy Metric')
ax2.set_ylim([1,4])
ax2.set_xlim([0,1])
ax3 = fig.add_subplot(233)
ax3.set_xlabel('Efficency Metric')
ax3.set_ylabel('Entropy Metric')
ax3.set_ylim([1.5,3])
ax3.set_xlim([0.9,1])

ax4 = fig.add_subplot(212, projection='3d')
ax4.set_xlabel('Efficency Metric')
ax4.set_ylabel('PLE Metric')
ax4.set_zlabel('Entropy Metric')
ax4.set_xlim([0.9,1])
ax4.set_ylim([0.1,0.8])
ax4.set_zlim([1.0,4])

# ax = plt.axes(xlim=(low, up), ylim=(low, up))
# ax = plt.axes(xlim=(0.9, 1.0), ylim=(0, 1))
scat1 = ax.scatter([3], [4], c="b")
scat2 = ax2.scatter([3], [4], c="b")
scat3 = ax3.scatter([3], [4], c="b")
scat4 = ax4.scatter([3, 4], [4, 5], [5, 6], c="b")


# initialization function: plot the background of each frame
def init():
    print "paths"
    # print scat.get_paths()
    # sys.exit()
    # scat.set_paths(matplotlib.path.Path([[2, 3]]))
    return scat1, scat2, scat3, scat4

# animation function.  This is called sequentially
def animate(i):
    tmp_data=data[i]
    # print tmp_data[:, 1:3]
    scat1.set_offsets(tmp_data[:, :2])
    scat2.set_offsets(tmp_data[:, 1:3])
    scat3.set_offsets(tmp_data[:, [0, 2]])
    # scat4.set_offsets(tmp_data)
    print scat4._offsets3d
    
    # scat4._offsets3d = (np.ma.ravel(tmp_data[:, 0]), np.ma.ravel(tmp_data[:, 1]), np.ma.ravel(tmp_data[:, 2]))
    scat4._offsets3d = (tuple(tmp_data[:, 0]), tuple(tmp_data[:, 1]), (tmp_data[:, 2]))
    # scat4._offsets3d = tmp_data
    # scat4.set_offsets(tmp_data[:,:2])
    # scat4.set_3d_properties(tmp_data[:,2],'z')
    plt.draw()
    return  scat1, scat2, scat3, scat4

# call the animator.  blit=True means only re-draw the parts that have changed.
anim = animation.FuncAnimation(fig, animate, init_func=init,
                               frames=200, interval=100, blit=True)

# save the animation as an mp4.  This requires ffmpeg or mencoder to be
# installed.  The extra_args ensure that the x264 codec is used, so that
# the video can be embedded in html5.  You may need to adjust this for
# your system: for more information, see
# http://matplotlib.sourceforge.net/api/animation_api.html
# anim.save('basic_animation.mp4', fps=30, extra_args=['-vcodec', 'libx264'])

plt.show()