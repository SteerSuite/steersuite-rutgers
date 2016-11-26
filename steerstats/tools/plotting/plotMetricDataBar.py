#!/usr/bin/env python
# a bar plot with errorbars
import numpy as np
import matplotlib.pyplot as plt

N = 6
scale = 100.0
ppr = np.array([[0.389, 0.486531845513, 0.563139309706, 0.525082089958, 0.956529670642, 0.584492],
                [0.0888, 0.201719007817, 0.0650382409885, 0.28306446735, 0.893289636035, 0.337310]])
ppr = np.divide(ppr[0] - ppr[1], ppr[0]) * scale
orca = np.array([[0.5610000000000000542, 0.610615939137, 0.555998, 0.665191365668, 0.746871, 0.619335],
                 [0.471600, 0.559310569063, 0.302385, 0.626107818416, 0.671885, 0.551048]]) * scale
orca = np.divide(orca[0] - orca[1], orca[0]) * scale
sf = np.array([[0.2606, 0.409060995479, 0.496210651886, 0.451420123905, 0.870840975302, 0.497626641698],
               [0.044000, 0.197546, 0.294940, 0.233776, 0.777800, 0.318901]])
sf = np.divide((sf[0] - sf[1]), sf[0] )*scale
# menStd =   (2, 3, 4, 1, 2)

ind = np.arange(N)  # the x locations for the groups
width = 0.25       # the width of the bars

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