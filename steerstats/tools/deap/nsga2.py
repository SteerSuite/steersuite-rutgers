#    This file is part of DEAP.
#
#    DEAP is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as
#    published by the Free Software Foundation, either version 3 of
#    the License, or (at your option) any later version.
#
#    DEAP is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
#    GNU Lesser General Public License for more details.
#
#    You should have received a copy of the GNU Lesser General Public
#    License along with DEAP. If not, see <http://www.gnu.org/licenses/>.

import array
import random
import json
import sys
import multiprocessing
import time
import os
import math

sys.path.append('../')
import numpy

from math import sqrt

from deap import algorithms
from deap import base
from deap import benchmarks
from deap.benchmarks.tools import diversity, convergence
from deap import creator
from deap import tools

# from pathos.multiprocessing import ProcessingPool as Pool

creator.create("FitnessMin", base.Fitness, weights=(-1.0, -1.0))
creator.create("Individual", array.array, typecode='d', fitness=creator.FitnessMin)

toolbox = base.Toolbox()

# Functions zdt1, zdt2, zdt3 have 30 dimensions, zdt4 and zdt6 have 10
NDIM = 30

# Problem definition
# Functions zdt1, zdt2, zdt3, zdt6 have bounds [0, 1]
upper_bound = []
BOUND_LOW, BOUND_UP = -1.0, 1.0
# BOUND_UP = [1.0 for i in range(NDIM)]
# BOUND_LOW = [0.0 for i in range(NDIM)]

# Functions zdt4 has bounds x1 = [0, 1], xn = [-5, 5], with n = 2, ..., 10
# BOUND_LOW, BOUND_UP = [0.0] + [-5.0]*9, [1.0] + [5.0]*9
print "Lower bound: " + str(BOUND_LOW)
print "Lower bound: " + str(BOUND_UP)

def saveMatrixToCVS(data, file):
    for row in data:
        for item in row:
            file.write(str(item) + str(','))
            
        file.write('\n')
        
def checkBoundsPop(min, max):
    def decorator(func):
        def wrapper(*args, **kargs):
            offspring = func(*args, **kargs)
            for child in offspring:
                for i in xrange(len(child)):
                    if child[i] > max:
                        child[i] = max
                    elif child[i] < min:
                        child[i] = min
            return offspring
        return wrapper
    return decorator

def checkBounds(min, max):
    def decorator(func):
        def wrapper(*args, **kargs):
            offspring = func(*args, **kargs)
            for i in xrange(len(offspring)):
                    if offspring[i] > max:
                        offspring[i] = max
                    elif offspring[i] < min:
                        offspring[i] = min
            return offspring
        return wrapper
    return decorator
"""
# http://deap.gel.ulaval.ca/doc/0.9/tutorials/next_step.html
toolbox.register("mate", tools.cxBlend, alpha=0.2)
toolbox.register("mutate", tools.mutGaussian, mu=0, sigma=2)

toolbox.decorate("mate", checkBounds(MIN, MAX))
toolbox.decorate("mutate", checkBounds(MIN, MAX))
"""

def optimFunc(individual):
    
    # for i in range(1000):
    f3 = benchmarks.rastrigin(individual)
    f1, f2 = benchmarks.zdt1(individual)
    out = (f1, f2) + f3
    # print os.getpid()
    return out


def optimFunc2(individual):
    
    f3 = benchmarks.rosenbrock(individual)
    f1, f2 = benchmarks.fonseca(individual)
    out = (f1, f2) + f3
    return out

def optimFunc3(individual):
    
    f3 = math.tan(individual[0])
    f1 = math.cos(individual[0])
    f2 = math.sin(individual[0])
    out = (f1, f2, f3)
    return out

def optimFunc4(individual):
    x1 = individual[0]
    x2 = individual[1]
    f1 = 0.5*(x1**2+x2**2) + math.sin(x1**2+x2**2)
    f2 = (((3.0*x1)-(2.0*x2)+(4.0))**2/8.0) + ((x1-x2+1.0)**2/27.0) + 15.0
    f3 = ((1.0)/((x1**2+x2**2)+1.0)) - ((1.1)*(math.exp((-(x1**2))-x2**2)))
    out = (f1, f2, f3)
    return out

def optimConvex(individual):
    x1 = individual[0]
    x2 = individual[1]
    f1 = (x1**2) + (x2-1.0)**2
    f2 = -(x1**2) + ((x2+1.0)**2) + 1.0
    f3 = ((x1-1.0)**2) + (x2**2) + 2.0
    out = (f1, f2, f3)
    return out

def zdt_1(individual):
    """ZDT1 multiobjective function.
    
    :math:`g(\\mathbf{x}) = 1 + \\frac{9}{n-1}\\sum_{i=2}^n x_i`
    
    :math:`f_{\\text{ZDT1}1}(\\mathbf{x}) = x_1`
    
    :math:`f_{\\text{ZDT1}2}(\\mathbf{x}) = g(\\mathbf{x})\\left[1 - \\sqrt{\\frac{x_1}{g(\\mathbf{x})}}\\right]`
    """
    g  = 1.0 + 9.0*sum(individual[1:])/(len(individual)-1)
    f1 = individual[0]
    f2 = g * (1 - sqrt(f1/g))
    f3 = individual[2]**3
    return f1, f2, f3

def uniform(low, up, size=None):
    try:
        return [random.uniform(a, b) for a, b in zip(low, up)]
    except TypeError:
        return [random.uniform(a, b) for a, b in zip([low] * size, [up] * size)]
paretoFront = tools.ParetoFront()    
toolbox.register("attr_float", uniform, BOUND_LOW, BOUND_UP, NDIM)
toolbox.decorate("attr_float", checkBounds(0.0, 1.0))
toolbox.register("individual", tools.initIterate, creator.Individual, toolbox.attr_float)
toolbox.register("population", tools.initRepeat, list, toolbox.individual)

# toolbox.register("evaluate", benchmarks.zdt1)
toolbox.register("evaluate", benchmarks.zdt1)

toolbox.register("mate", tools.cxSimulatedBinaryBounded, low=BOUND_LOW, up=BOUND_UP, eta=20.0)
toolbox.register("mutate", tools.mutPolynomialBounded, low=BOUND_LOW, up=BOUND_UP, eta=20.0, indpb=1.0/NDIM)
toolbox.register("select", tools.selNSGA2)
toolbox.decorate("mate", checkBoundsPop(0.0, 1.0))
toolbox.decorate("mutate", checkBoundsPop(0.0, 1.0))
# toolbox.decorate("mate", paretoFront.decorator)
# toolbox.decorate("mutate", paretoFront.decorator) 

# pool = multiprocessing.Pool(4)
# pool = Pool(4)
# toolbox.register("map", pool.map)


def main(seed=None):
    random.seed(seed)

    NGEN = 10
    MU = 40
    CXPB = 0.9
    stats = tools.Statistics(lambda ind: ind.fitness.values)
    stats.register("avg", numpy.mean, axis=0)
    stats.register("std", numpy.std, axis=0)
    stats.register("min", numpy.min, axis=0)
    stats.register("max", numpy.max, axis=0)
    
    logbook = tools.Logbook()
    logbook.header = "gen", "evals", "std", "min", "avg", "max"
    
    pop = toolbox.population(n=MU)

    # Evaluate the individuals with an invalid fitness
    invalid_ind = [ind for ind in pop if not ind.fitness.valid]
    fitnesses = toolbox.map(toolbox.evaluate, invalid_ind)
    for ind, fit in zip(invalid_ind, fitnesses):
        ind.fitness.values = fit

    # This is just to assign the crowding distance to the individuals
    # no actual selection is done
    pop = toolbox.select(pop, len(pop))
    paretoFront.update(pop)
    
    record = stats.compile(pop)
    logbook.record(gen=0, evals=len(invalid_ind), **record)
    print(logbook.stream)

    # Begin the generational process
    for gen in range(1, NGEN):
        # Vary the population
        offspring = tools.selTournamentDCD(pop, len(pop))
        offspring = [toolbox.clone(ind) for ind in offspring]
        
        for ind1, ind2 in zip(offspring[::2], offspring[1::2]):
            if random.random() <= CXPB:
                toolbox.mate(ind1, ind2)
            
            toolbox.mutate(ind1)
            toolbox.mutate(ind2)
            del ind1.fitness.values, ind2.fitness.values
        
        # Evaluate the individuals with an invalid fitness
        invalid_ind = [ind for ind in offspring if not ind.fitness.valid]
        fitnesses = toolbox.map(toolbox.evaluate, invalid_ind)
        for ind, fit in zip(invalid_ind, fitnesses):
            ind.fitness.values = fit

        # Select the next generation population
        pop = toolbox.select(pop + offspring, MU)
        record = stats.compile(pop)
        logbook.record(gen=gen, evals=len(invalid_ind), **record)
        print(logbook.stream)
    return pop, logbook
        
if __name__ == "__main__":
    optimal_front = json.load(open("zdt1_front.json"))
    # Use 500 of the 1000 points in the json file
    optimal_front = sorted(optimal_front[i] for i in range(0, len(optimal_front), 2))
    
    pop, stats = main()
    pop.sort(key=lambda x: x.fitness.values)
    paretoFront.update(pop)
    # print "Outputting the stats"
    print(stats)
    # print "************************* len of pop " + str(len(pop))
    # print "************************* len of pop " + str(len(pop[0])) 
    # print pop
    print("Convergence: ", convergence(pop, optimal_front))
    print("Diversity: ", diversity(pop, optimal_front[0], optimal_front[-1]))
    
    import matplotlib.pyplot as plt
    from mpl_toolkits.mplot3d import Axes3D
    import numpy
    # 
    front = numpy.array([ind.fitness.values for ind in pop])
    # print "************** "
    print paretoFront
    pFront = numpy.array([ind.fitness.values for ind in paretoFront])
    print pFront
    print front
    xs = front[:,0]
    ys = front[:,1]
    zs = 0
    # zs = front[:,2]
    fig = plt.figure()
    ax = fig.add_subplot(111)
    optimal_front = numpy.array(optimal_front)
    ax.scatter(optimal_front[:,0], optimal_front[:,1], c="r")
    
    
    
    ax.plot(xs, ys,c="b")
    ax.scatter(pFront[:,0], pFront[:,1], c="g")
    ax.plot(pFront[:,0], pFront[:,1], c="g")
    # ax.scatter(xs, ys, zs, c="b")
    opt_log = open("SteerStatsOpt_.log", "w")
    saveMatrixToCVS(numpy.append(front, pop, axis=1), opt_log)
    opt_log.close()
    # ax.plot_wireframe(xs, ys, zs, rstride=1, cstride=1)

    ax.set_xlabel('f1')
    ax.set_ylabel('f2')
    
    plt.show()
