from numpy import *
from matplotlib.mlab   import csv2rec
import matplotlib.pyplot as plt
import csv
import sys
from mpl_toolkits.mplot3d import Axes3D

filename = sys.argv[1]
csvfile = open(filename, 'r')
spamreader = csv.reader(csvfile, delimiter=',')
xs = []
ys = []
zs = []
for row in spamreader:
    xs.append(float(row[0]))
    ys.append(float(row[1]))
    zs.append(float(row[2]))
print "xs = " + str(xs)
print "ys = " + str(ys)
print "zs = " + str(zs)

fig = plt.figure(1)
ax = fig.add_subplot(111)
ax.scatter(xs, ys, zs, c=zs)
ax.set_title('data points : z=f(x,y)')
ax.set_xlabel('x')
ax.set_ylabel('y')
ax.grid()


 # interpolation spline with scipy
from scipy import interpolate

tck0 = interpolate.bisplrep(xs, ys, zs)
xnew,ynew = mgrid[-1:1:70j,-1:1:70j]
znew = interpolate.bisplev(xnew[:,0],ynew[0,:],tck0)
fig = plt.figure()
ax = fig.gca(projection='3d')
ax.scatter(xnew,ynew,znew, c="b")
# colorbar()
ax.set_title("Interpolated z=f(xnew,ynew)")
plt.show()