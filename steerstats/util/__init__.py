
import numpy as np
import sys
import csv
import random
import array
# OSX does not support this easily.
import scipy
sys.path.append("../")
from tools.gameobjects.vector3 import Vector3 as vector

from tools.deap import algorithms
from tools.deap import base
from tools.deap import benchmarks
from tools.deap.benchmarks.tools import diversity, convergence
from tools.deap import creator
from tools.deap import tools


__all__ = ['overlaps', 'chunks']

def distance(x1, y1, x2, y2):
    """
        Returns the distance between the twoo points
    
    """
    p1 = vector(x1,0,y1)
    p2 = vector(x2, 0, y2)
    
    return (p1 - p2).length()


def overlaps(x1, y1, r1, x2, y2, r2):
    """
        Returns the amount the two circles overlap
    """
    # print "do " + str(agent1) + " and " + str(agent2) + " overlap?"
    p1 = vector(x1,0,y1)
    p2 = vector(x2, 0, y2)
    distance = (p1 - p2).get_length()
    distanceThreshold = (r1 + r2)
    # print "distanceTreshold: " + str(distanceThreshold) + " distance: " + str(distance)
    overlaps = distanceThreshold - distance
    if overlaps > 0:
        return overlaps
    else:
        return 0
    
def chunks(l, n):
    return [l[i:i+n] for i in xrange(0, len(l), n)]

def saveMatrixToCVS(data, file):
    
    """
        writes the data out to a file
    """
    # The first n-1 rows
    for row in data[:-1]:
        # the first m-1 items in the row
        for item in row[:-1]:
            file.write(str(item) + str(','))
        # -1 for last element in array
        file.write(str(row[-1]))
            
        file.write('\n')
    # The m-1 items in the last row
    for item in data[-1][:-1]:
        file.write(str(item) + str(','))
        # -1 for last element in array
    # item n,m
    file.write(str(data[-1][-1]))
    
def saveMatrixToCVSDict(data, csvfile, fieldNames):
    
    """
        writes the data out to a file
    """
    csvWriter = csv.DictWriter(csvfile, fieldNames, dialect='excel')
    
    headers = dict( (n,n) for n in fieldNames )
    csvWriter.writerow(headers)
    for row in data:
        d = dict((str(k), str(v)) for k, v in zip(fieldNames, row))
        # print d
        csvWriter.writerow(d)

def writeEntropyMetricData(data, file):
    
    """
        This will append a default 0 in the z data
        
        writes the data out to a entropy data file
        <agent_#> <frame_#> <x> <y> <z>\n
        
        agents and frames should be in order. Data should be in centmeters
    """
    a=1
    f=1
    for agent in data:
        for frame in agent:
            file.write("{0} {1} {2} {3} {4}\n".format(a, f, frame[0], frame[1], 0))
            f=f+1
        a=a+1
        f=1
            
        
def readCSVToMatrix(csvfile, delimiter=','):
    """
        cvsfile should be an open readable file
    """
    spamreader = csv.reader(csvfile, delimiter=delimiter)
    data=[]
    try:
        for row in spamreader:
            tmp_data=[]
            for item in row:
                tmp_data.append(float(item))
            data.append(tmp_data)
        # print data
        return np.array(data, dtype='d')
    except ValueError as err:
        print "item can not be converted: " + str(item)
        print data
        raise err
        
        
def readCSVToMutliObjData(file, fNum):
    """
        fNum is the number of objective functions in the data
    """
    data = readCSVToMatrix(file)
    print data
    data = np.array(data, dtype='d')
    return data[:,:fNum], data[:,fNum:]

def readCSVDictToMutliObjData(file, fNum, fieldNames=None):
    """
        fNum is the number of objective functions in the data
    """
    data = readCSVToDictList(file, fieldNames)
    # print data
    data = np.array(data, dtype='d')
    return data[:,:fNum], data[:,fNum:]

def readCSVToDict(items_file):
    for row in csv.DictReader(items_file, dialect='excel', delimiter=','):
        for key in row:
            print 'key=%s, value=%s' % (key, row[key])
        print ' '
        
def readCSVToDictList(items_file, fieldNameOrder):
    out = {}
    data = []
    for row in csv.DictReader(items_file, dialect='excel', delimiter=','):
        data.append(row)
        
    for key in data[0]:
        # print 'key=%s, value=%s' % (key, data[0][key])
        out[key] = []
        
    for row in data:
        for key in row:
            out[key].append(row[key])
            
    # print out
    
    out2 = []
    
    for key in fieldNameOrder:
        out2.append(out[key])
    # print ''
    # print out2
    # print ''
    out2 = np.array(out2).transpose()
    # print out2
    return out2
        
def uniform(low, up, size=None):
        return [random.uniform(a, b) for a, b in zip(low, up)]
    
def getParetoFront(fitnesses, invalid_ind):
    weights_ = (-1.0,)*len(fitnesses[0])
    NDIM=len(invalid_ind[0])
    ai_bounds = []
    ai_bounds.append([-1.0]*NDIM)
    ai_bounds.append([1.0]*NDIM)
    creator.create("FitnessMin", base.Fitness, weights=weights_)
    creator.create("Individual", array.array, typecode='d', fitness=creator.FitnessMin)
    
    toolbox = base.Toolbox()
    
    toolbox.register("attr_float", uniform, ai_bounds[0], ai_bounds[1], NDIM)
    toolbox.register("individual", tools.initIterate, creator.Individual, toolbox.attr_float)
    toolbox.register("population", tools.initRepeat, list, toolbox.individual)
    
    toolbox.register("select", tools.selNSGA2)
    
    pop = toolbox.population(n=len(fitnesses))
    for tmp_p, ind, fit in zip(invalid_ind, pop, fitnesses):
            fit = tuple(fit)
            # print "fit: " + str(fit)
            # print "tmp_p: " + str(tmp_p)
            # print "ind " + str(ind)
            ind.fitness.values = fit
            for i in range(len(ind)):
                ind[i] = float(tmp_p[i])
    # This is just to assign the crowding distance to the individuals
    # no actual selection is done
    pop = toolbox.select(pop, len(pop))
    paretoFront = tools.ParetoFront()
    paretoFront.update(pop)
    pFront = np.array([ind.fitness.values for ind in paretoFront])
    # print pFront
    return pFront

def convertToBarycentric(triangle, input):
    v0 = triangle[0] - triangle[2]
    v1 = triangle[1] - triangle[2]
    v2 = input - triangle[2]
    d00 = dot(v0, v0)
    d01 = dot(v0, v1)
    d11 = dot(v1, v1)
    d20 = dot(v2, v0)
    d21 = dot(v2, v1)
    denom = (d00 * d11) - (d01 * d01)
    
    a = (d11 * d20 - d01 * d21) / denom
    b = (d00 * d21 - d01 * d20) / denom
    weights = np.array([a, b, 1 - a - b]) 
    return weights

def dot(left, right):
     return (left[0] * right[0]) + (left[1] * right[1])
    
def barycentric(T, data):
    
    out = [] 
    for row in data:
        out.append(convertToBarycentric(T, row))
    
    return np.array(out)

def _closestPoints(data, point):
    """
        Finds the closest 2*len(point) points
    """
    print "looking for the points closest to " + str(point)
    above = [1000000.0]*len(point)
    above = [above] * len(point)
    below = [-1000000.0]*len(point)
    below = [below] * len(point)
    
    # print "above " + str(above)
    # print "below " + str(below)
    for tmp_p in data:
        for ind in range(len(point)):
            if (tmp_p[ind] > point[ind]) and (tmp_p[ind] - point[ind]) < (above[ind][ind] - point[ind]):
                # print "above " + str(tmp_p)
                above[ind] = tmp_p
            if (tmp_p[ind] < point[ind]) and (point[ind] - tmp_p[ind]) < (point[ind] - below[ind][ind]):
                # print "below " + str(tmp_p)
                below[ind] = tmp_p
    
    return np.append(above, below, axis=0)

def closestPoints(data, point):
    # do Delaunay
    delaunay = scipy.spatial.Delaunay(data[:,:-1])
    # find simplex
    simplex = delaunay.find_simplex(xi=point[:-1])
    # get points from simplex
    points = []
    for tup in delaunay.vertices[simplex]:
        points.append(data[tup])
        
    return np.array(points)

def closestVertices(data, point):
    # do Delaunay
    delaunay = scipy.spatial.Delaunay(data[:,:-1])
    # find simplex
    simplex = delaunay.find_simplex(xi=point[:-1])
    # get points from simplex
    verts = []
    for tup in delaunay.vertices[simplex]:
        verts.append(tup)
        
    return np.array(verts)
            
def inverseDistance(T, point):
    distance = 0.0
    for row in T:
        distance = distance + np.linalg.norm(point-row)
    weights = []
    for row in T:
        weights.append( (distance - np.linalg.norm(point-row))/distance )
        
    return np.array(weights)

def normalizedInverseDistance(T, point):
    
    weights = inverseDistance(T, point)
    norm = np.sum(weights)
        
    return weights/norm

def barycentricProjection(data):
    new_xs_b = []
    new_ys_b = []
    new_zs_b = []
    for i in range(len(data)):
        length =  ( data[i][0] + data[i][1] + data[i][2] )
        new_xs_b.append(data[i][0] / length)
        new_ys_b.append(data[i][1] / length)
        new_zs_b.append(data[i][2] / length)
    
    new_xs_b = np.array(new_xs_b)
    new_ys_b = np.array(new_ys_b)
    new_zs_b = np.array(new_zs_b)
    return np.array([new_xs_b, new_ys_b, new_zs_b]).transpose()
    

def barycentricRelative(T, data):
    p0 = T[0]
    p1 = T[1]
    p2 = T[2]
    
    out = [] 
    for row in data:
        x = row[0]
        y = row[1]
        z = row[2]
        tmp_out = []
        tmp_out.append(np.linalg.norm(p0-row))
        tmp_out.append(np.linalg.norm(p1-row))
        tmp_out.append(np.linalg.norm(p2-row))
        
        out.append(tmp_out)
    
    return np.array(out)
        

if __name__ == '__main__':
    file = open(sys.argv[1])
    fieldNamesOrder = ['o0', 'o1', 'hidac_acceleration', 'hidac_personal_space_threshold', 
                       'hidac_agent_repulsion_importance', 'hidac_body_force', 
                       'hidac_agent_body_force', 'hidac_sliding_friction_force', 
                       'hidac_agent_b', 'hidac_agent_a', 'hidac_wall_b', 'hidac_wall_a']
    readCSVToDictList(file, fieldNameOrder=fieldNamesOrder)
    file.close()
