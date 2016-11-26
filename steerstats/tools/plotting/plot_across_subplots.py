#! /usr/bin/env python

import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from mpl_toolkits.mplot3d import proj3d
import matplotlib

N = 50
x = np.random.rand(N)
y = np.random.rand(N)
z = np.random.rand(N)

# point's to join
p1 = 10
p2 = 20

fig = plt.figure()

# a background axis to draw lines on
ax0 = plt.axes([0.,0.,1.,1.])
ax0.set_xlim(0,1)
ax0.set_ylim(0,1)

# use these to know how to transform the screen coords
dpi = ax0.figure.get_dpi()
height = ax0.figure.get_figheight() * dpi
width = ax0.figure.get_figwidth() * dpi

# first scatter plot
ax1 = plt.axes([0.05,0.05,0.9,0.425], projection='3d')
ax1.scatter(x, y, z)

# one point of interest
ax1.scatter(x[p1], y[p1], z[p1], s=100.)
x1, y1, _ = proj3d.proj_transform(x[p1], y[p1], z[p1], ax1.get_proj())
[x1,y1] = ax1.transData.transform((x1, y1))  # convert 2d space to screen space
# put them in screen space relative to ax0
x1 = x1/width
y1 = y1/height

# second scatter plot (same data)
ax2 = plt.axes([0.05,0.475,0.9,0.425], projection='3d')
ax2.scatter(x, y, z)

# another point of interest
ax2.scatter(x[p2], y[p2], z[p2], s=100.)
x2, y2, _ = proj3d.proj_transform(x[p2], y[p2], z[p2], ax2.get_proj())
[x2,y2] = ax2.transData.transform((x2, y2))  # convert 2d space to screen space
x2 = x2/width
y2 = y2/height


# set all these guys to invisible (needed?, smartest way?)
for item in [fig, ax1, ax2]:
    item.patch.set_visible(False)

# draw a line between the transformed points
# again, needed? I know it works...

transFigure = fig.transFigure.inverted()

coord1 = transFigure.transform(ax0.transData.transform([x1,y1]))
coord2 = transFigure.transform(ax0.transData.transform([x2,y2]))

line = matplotlib.lines.Line2D((coord1[0],coord2[0]),(coord1[1],coord2[1]),
                               transform=fig.transFigure)
fig.lines = line,

plt.show()