
import sys
import random
sys.path.append("../")
from util import *
import numpy

__all__ = ['overlapPenalty']


def chunks(l, n):
    return [l[i:i+n] for i in xrange(0, len(l), n)]

def overlapPenalty(args, f):
    """
        Returns the sum of overlap penalties between objects
    """
    # divy up chunks
    # args = numpy.append(args, args)
    # args = numpy.append(args, args)
    # print "args " + str(args)
    chunkies = chunks(args, 2)
    print "chunkies " + str(chunkies) + " chunk[0] " + str(chunkies[0])
    penalty=0
    for obj1 in chunkies:
        for obj2 in chunkies:
            penalty = penalty + overlapPenaltyPair(obj1, obj2, f)
        penalty = penalty - overlapPenaltyPair(obj1, obj1, f)
    return penalty

def overlapPenaltyPair(obj1, obj2, f):
    """
        Computes and overlap penalty between two objects.
        objects = <x, z, r> 
    """
    x1 = obj1[0]
    y1 = obj1[1]
    
    x2 = obj2[0]
    y2 = obj2[1]
    overlap = overlaps(x1, y1, 0.65, x2, y2, 0.65) 
    if overlap > 0:
        # print str(obj1) + " overlaps " + str(obj2)
        return ((overlap + 1) * (f+1))**2
    else:
        return 0 
    