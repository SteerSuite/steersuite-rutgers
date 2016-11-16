from cma import fmin
from cma import fcts
import math
import random
import numpy as np

print "help"

def gcos2(xs):
    sum=0
    for i in range(len(xs)):
        sum = sum + math.sin(float(xs[i])) + xs[i]
    return sum

def onethousand(x, **args):
    sum = 0
    for i in range(len(x)):
        sum = sum + math.fabs(x[i]-1000)
    return sum

def halfBad(x):
    if random.choice([0, 1]) > 0.5:
        return np.NaN
    else:
        # return np.NaN
        return onethousand(x)


print str(7 * [0.1])
print "sin(1): " + str(math.sin(-1.57))

# print fmin(fcts.elli, 7 * [0.1], 1, verb_disp=1e9)

lbound = [ 0, 0] # lower bounds
ubound = [800, 800] # upper bounds
bounds = [lbound, ubound]
options = {'bounds':bounds, 'seed':1234} # Setting options for optimization
result = fmin(onethousand, [0, 9809.0], 10.1, **options)

print "result: " + repr(result)
result[-1].plot()