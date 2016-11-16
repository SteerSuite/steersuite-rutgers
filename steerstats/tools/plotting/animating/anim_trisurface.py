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
from matplotlib import cm
import random
import sys
import csv

filename = sys.argv[1]
frames = int(sys.argv[2])
data = []
for i in range(1, frames):
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


ax = fig.add_subplot(111, projection='3d')
ax.set_xlabel('Efficency Metric')
ax.set_ylabel('PLE Metric')
ax.set_zlabel('Entropy Metric')
ax.set_xlim([0.9,1])
ax.set_ylim([0.1,0.8])
ax.set_zlim([1.0,4])

# ax = plt.axes(xlim=(low, up), ylim=(low, up))
# ax = plt.axes(xlim=(0.9, 1.0), ylim=(0, 1))
surface = ax.plot_trisurf(data[0][:,0], data[0][:,1], data[0][:,2], cmap=cm.jet, linewidth=0.2)


# initialization function: plot the background of each frame
def init():
    print "paths"
    # print scat.get_paths()
    # sys.exit()
    # scat.set_paths(matplotlib.path.Path([[2, 3]]))
    return surface, 

# animation function.  This is called sequentially
def animate(i):
    tmp_data=data[i]
    print i
    # print tmp_data[:, 1:3]
    # surface.set_offsets(tmp_data)
    # print surface.get_offsets()
    # surface.set_data(tmp_data)
    # print scat4._offsets3d
    
    # scat4._offsets3d = (np.ma.ravel(tmp_data[:, 0]), np.ma.ravel(tmp_data[:, 1]), np.ma.ravel(tmp_data[:, 2]))
    # surface._offsets3d = (tuple(tmp_data[:, 0]), tuple(tmp_data[:, 1]), (tmp_data[:, 2]))
    # scat4._offsets3d = tmp_data
    # scat4.set_offsets(tmp_data[:,:2])
    # scat4.set_3d_properties(tmp_data[:,2],'z')
    ax.clear()
    surface = ax.plot_trisurf(data[i][:,0], data[i][:,1], data[i][:,2], cmap=cm.jet, linewidth=0.2)
    plt.draw()
    return  surface,

# call the animator.  blit=True means only re-draw the parts that have changed.
anim = animation.FuncAnimation(fig, animate, init_func=init,
                               frames=frames, interval=100, blit=True)

# save the animation as an mp4.  This requires ffmpeg or mencoder to be
# installed.  The extra_args ensure that the x264 codec is used, so that
# the video can be embedded in html5.  You may need to adjust this for
# your system: for more information, see
# http://matplotlib.sourceforge.net/api/animation_api.html
anim.save('basic_animation.mp4', fps=10, extra_args=['-vcodec', 'libx264'])

plt.show()