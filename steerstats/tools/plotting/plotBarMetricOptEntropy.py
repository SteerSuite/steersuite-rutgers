#!/usr/bin/env python
# a bar plot with errorbars
import numpy as np
import matplotlib.pyplot as plt

N = 2
scale = 100.0

ppr = np.array([[3.41784, 3.403440],
                [1.91507, 2.271680]])
ppr = np.divide(ppr[0] - ppr[1], ppr[0]) * scale
orca = np.array([[2.117200, 2.953220],
                 [0.628748, 2.203690]])
orca = np.divide(orca[0] - orca[1], orca[0]) * scale
sf = np.array([[3.741280, 3.620520],
               [3.098120, 2.757230]])
sf = np.divide(sf[0] - sf[1], sf[0]) * scale
"""
max = np.amax(ppr)
ppr = ppr / max
orca = orca / max
sf = sf / max
"""
# menStd =   (2, 3, 4, 1, 2)

ind = np.arange(N)  # the x locations for the groups
width = 0.25       # the width of the bars
adjust=0.5

fig, ax = plt.subplots()
rects1 = ax.bar(ind, ppr, width, color='r')

# womenStd =   (3, 5, 2, 3, 3)
rects2 = ax.bar(ind+width, orca, width, color='b')

rects3 = ax.bar(ind+(width*2.0), sf, width, color='g')

# add some
# ax.set_ylabel('Scores')
# ax.set_title('Scores by group and gender')
ax.set_xticks(ind+(width*1.5))
# ax.set_xticklabels( ('d', 'q^d', 'q^t', 'q^e', 'e', 'u') )
ax.set_xticklabels( ('', '', '', '', '', '') )

ax.legend( (rects1[0], rects2[0], rects3), ('PPR', 'ORCA', 'SF', ) )

def autolabel(rects):
    # attach some text labels
    for rect in rects:
        height = rect.get_height()
        ax.text(rect.get_x()+rect.get_width()/2., 1.05*height, '%d'%int(height),
                ha='center', va='bottom')

# autolabel(rects1)
# autolabel(rects2)
# autolabel(rects3)

plt.show()