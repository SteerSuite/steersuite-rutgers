#!/usr/bin/env python
# a bar plot with errorbars
import numpy as np
import matplotlib.pyplot as plt

N = 6
scale = 100.0

ppr = np.array([[0.3894,     0.487654575109,     0.565519108483,     0.527699992598,    0.963413513353,     0.586981050741],
                [0.098,     0.217110049354,    0.0717740961362,    0.302297290503,    0.905959925553,      0.341180998219]])
ppr = np.divide(ppr[0] - ppr[1], ppr[0]) * scale
orca = np.array([[0.5314,    0.612707635721,    0.555547145788,    0.666921900525,    0.842679710495,    0.641050415],
                 [0.5096,     0.566699756221,      0.293049849496,    0.623065539314,      0.816804559293,      0.57566110481]])
orca = np.divide(orca[0] - orca[1], orca[0]) * scale
sf = np.array([[0.271,    0.415502261389,    0.502642359067,    0.457747830345,    0.886034146571,    0.506585319474],
               [0.0508,    0.200472061167,    0.296951199671,    0.234167359864,    0.806159990216,    0.329237443197]])
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
