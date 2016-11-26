from SteerStats import SteerStats
from steersuite.SteerStatsOptions import getOptions
from tools.cma.cma import fmin
from tools.cma.cma import fcts
import math
from steersuite.OptimizationExperiment import OptimizationExperiment
import sys
from optimization import *
import random
import array
import numpy 
import copy
from math import sqrt
from tools.deap import algorithms
from tools.deap import base
from tools.deap import benchmarks
from tools.deap.benchmarks.tools import diversity, convergence
from tools.deap import creator
from tools.deap import tools
from util import saveMatrixToCVS
from util import saveMatrixToCVSDict
from util import readCSVToMutliObjData
from util import readCSVDictToMutliObjData

from multiprocessing import Semaphore

# for post processing
from tools.SteerStatsPostProcessEvent import PostProcessEvent_TarData
from tools.SteerStatsPostProcessEvent import PostProcessEvent_DumpOptResults
from tools.SteerStatsPostProcessEvent import PostProcessEvent_CreateHeatMap
from tools.SteerStatsPostProcessEvent import PostProcessEvent_MakeVideo
# import multiprocessing
# from pathos.multiprocessing import ProcessingPool as Pool
# from tools.plotting.PlotParameterData import parameter



# clear; clear; time python OptimizeAlgorithm.py --ai pprAI --checkAgentInteraction --numScenarios 200 --benchmark compositeGraph -c --scenarioSetInitId=0 --scenarioDIR data/scenarios/representativeSet --paramFile xml/config/ppr-param-config.xml --statsOnly -p 2

# clear; clear; time nice -4 python OptimizeAlgorithm.py --ai rvo2dAI --checkAgentInteraction --numScenarios 5000 -c --scenarioSetInitId=0 --scenarioDIR data/scenarios/representativeSet --paramFile xml/config/rvo2d-param-config.xml --statsOnly -p 16

# clear; clear; time nice -4 python OptimizeAlgorithm.py --ai pprAI --checkAgentInteraction --numScenarios 50 -c --scenarioSetInitId=0 --scenarioDIR data/scenarios/representativeSet --paramFile xml/config/ppr_optimized_configs/param_config_ppr_combination.xml --statsOnly -p 2 --cma data

# clear; clear; time nice -4 python OptimizeAlgorithm.py --ai rvo2dAI --checkAgentInteraction --numScenarios 500 -c --scenarioSetInitId=0 --scenarioDIR data/scenarios/representativeSet_Intersections/ --paramFile xml/config/rvo2d-param-config.xml --statsOnly -p 2 --cma data --benchmark compositePLE

# clear; clear; time nice -4 python OptimizeAlgorithm.py --ai pprAI --checkAgentInteraction --numScenarios 5000 -c --scenarioSetInitId=0 --scenarioDIR data/scenarios/representativeSet_Intersections/ --paramFile xml/config/ppr-param-config.xml --statsOnly -p 10 --cma data/optimization/ppr_over_PLE/ --benchmark compositePLE --dataDir /Volumes/Block0/SteerFit/PPR/PLE/

# clear; clear; time nice -4 python OptimizeAlgorithm.py --ai footstepAI --checkAgentInteraction --numScenarios 200 -c --scenarioSetInitId=0 --scenarioDIR data/scenarios/representativeSet_Intersections/ --paramFile xml/config/footstep-param-config.xml --statsOnly -p 2 --benchmark compositePLE

# Optimize a subspace
# clear; clear; time nice -18 python OptimizeAlgorithm.py --ai sfAI --numScenarios 10 --benchmark compositePLE --statsOnly --scenarioSetInitId 0 --scenarioDIR data/scenarios/subspace/ --subspace=../subspaces/hallway-one-way.xml --optimizer=CMA-ES --paramFile=xml/config/subspaces/hallway-one-way-2pillar.xml --cmaDumpDir=data/ --numFrames=2000
# clear; clear; time nice -18 python OptimizeAlgorithm.py --ai sfAI --numScenarios 10 --benchmark compositePLE --statsOnly --scenarioSetInitId 0 --scenarioDIR data/scenarios/subspace/ --subspace=../subspaces/hallway-one-way.xml --optimizer=CMA-ES --paramFile=xml/config/subspaces/hallway-4-pillar.xml --cmaDumpDir=data/ --numFrames=2000

# To optimize 
# clear; clear; time nice -18 python OptimizeAlgorithm.py --ai sfAI --numScenarios 1 --benchmark compositePLE --statsOnly --scenarioSetInitId 0 --scenarioDIR data/scenarios/subspace/ --subspace=../subspaces/hallway-one-way.xml --optimizer=CMA-ES --paramFile=xml/config/subspaces/hallway-4-pillar.xml --cmaDumpDir=data/ --numFrames=2000 -c

def nothing(*args):
    return 0

class OptimizeAlgorithm(object):
    
    def __init__(self, f=nothing, g=nothing, options=None):
        import os
        from datetime import datetime
        self._result_list = []
        self._experiements = []
        self._penalty_func = []
        self.set_metric_func(f)
        self.set_penatly_funct(g)
        self._evals=0
        self._options=options
        self.__eval_processes=1
        self.__evalSem=Semaphore(self.__eval_processes)
        if os.path.exists(self._options.dataPath) and (not self._options.noSimulation):
            if self._options.dataPath[-1] == '/': 
                newDir = self._options.dataPath[:-1]+"_"+str(datetime.now().strftime('%Y-%m-%d-%M-%S.%f'))
            else:
                newDir = self._options.dataPath+"_"+str(datetime.now().strftime('%Y-%m-%d-%M-%S.%f'))
            print "Moving old simulation data dir: " + str(self._options.dataPath) + " to " + newDir
            os.rename( self._options.dataPath, newDir )
        self._post_process_events=[]
        # Used to normalize results (make it possible for functions to be converted to minimizations)
        self._control_result=None
        
    def set_metric_func(self, f):
        self._metric_func=f
        
    def set_penatly_funct(self, g):
        self._penalty_func.append(g);
        
    def set_steerstats(self, steerStats):
        self._steerStats=steerStats
    def get_steerstats(self):
        return self._steerStats
        
    def add_post_process_event(self, event):
		self._post_process_events.append(event)
        
    def post_process(self):
    	for event in self._post_process_events:
    		event.process()
    		
    def get_options(self):
    	return self._options
    
    def eval(self, parameters):
        """
            Used to combine the metric and penalty functions. This also allows
            the penalty function to be a function of the metric function.
        """

        dirName="eval"
        self.__evalSem.acquire()
        options = copy.deepcopy(self._options)
        options.dataPath = options.dataPath+dirName+str(self._evals)+"/"
        print "Evals thus far: " + str(self._evals) + " datapath: "+ str(options.dataPath)
        self._evals += 1
        self.__evalSem.release()
        f = self._metric_func(parameters,options=options)
        # print "parameters in eval: " + str(parameters)

        g = float(0)
        for penalty in self._penalty_func:
            g = g + penalty(parameters, f)
        if self._control_result is not None:
            print "Control result is: " + str(self._control_result)
            print "original f is: " + str(f)
            f = self._control_result - f

        result = f + g
        print "**** f: " + str(f) + " ***** g:" + str(g) + " **** result: " + str(result) + " params: " + str(parameters)
        if options.logMetrics:
            # Will this work properly? Will the instances be different?
            # will it handle multi-threading??
            data = [result, f, g]
            data.extend(parameters)
            print "Writing metric file: " + str(data)
            self._steerStats.log(data)
            self._steerStats.close_data_log()
        return result

    def evalMulti(self, parameters):
        out = ()
        results = self._steerStats.RunStats(parameters)
        for metric in self._metric_func:
            out = out + (metric(parameters, results),)
        
        return out
    
    def evalMultiWithEntropyLast(self, parameters):
        out = ()
        results = self._steerStats.RunStats(parameters)
        for metric in self._metric_func[:-1]:
            out = out + (metric(parameters, results),)
        entropyOut = self._metric_func[2](parameters)
        out = out + (entropyOut,)
        return out
    
    def log_result(self, result):
        # This is called whenever foo_pool(i) returns a result.
        # result_list is modified only by the main process, not the pool workers.
        self._result_list.append(result)
    def get_results(self):
    	return self._result_list
    def set_experiements(self, exps):
    	self._experiements = exps
    def get_experiements(self):
    	return self._experiements

def OptimizeParamsCMA(function_to_min, initial_param_values, std_value, 
                      ai_bounds, CMAavailableProcesses,
                      cmaFilePrefix="", step_scaling=None, 
                       maxfevals_=1000):
    return fmin(function_to_min, initial_param_values, sigma0=std_value, 
                bounds=ai_bounds,
                verb_filenameprefix=cmaFilePrefix+'outcma',
                scaling_of_variables=step_scaling,
                maxfevals=maxfevals_,
                availableProceses=CMAavailableProcesses)
    
def OptimizeParamsCMAProfiling(function_to_min, initial_param_values, goal_value, 
                      ai_bounds, cmaFilePrefix=""):
    return fmin(function_to_min, initial_param_values, sigma0=goal_value, 
                bounds=ai_bounds,
                verb_filenameprefix=cmaFilePrefix+'outcma',
                maxiter=3)

def prepareOptimization(filename, options):
    
    param_xml_file = open(filename)
    param_config_data = param_xml_file.read()
    
    OptimExper = OptimizationExperiment(param_config_data)
    optimizingParameters = OptimExper.parseExperimentXML()

    stddev_for_parameters=1
    
    scaling_factor=float(0.3)
        
    ai_params_list = []
    ai_param_names_list = []
    ai_lower_bounds = []
    ai_upper_bounds = []
    ai_step_scaling = []
    
    
    for ai_param in optimizingParameters:
        ai_params_list.append(ai_param.get_original())
        ai_param_names_list.append(ai_param.get_name())
        ai_lower_bounds.append(ai_param.get_min())
        ai_upper_bounds.append(ai_param.get_max())
        """ 
        This is magic stuff here
        Scaling stddev for all the parameters acording the min/max values for the search
        apparently this should be 0.3 of the search area if your search area is perfectly round 
        """
        ai_step_scaling.append( (ai_param.get_max()-ai_param.get_min())*scaling_factor )
   
    ai_bounds = [ai_lower_bounds, ai_upper_bounds]
    
    return [ai_bounds, ai_params_list, ai_param_names_list,
     stddev_for_parameters, ai_step_scaling]

def multiOptimizeNSGA2(restart_=False):
    """
    # for multi objective optimization
    # clear; clear; time nice -4 python OptimizeAlgorithm.py --ai sfAI --checkAgentInteraction --numScenarios 1 --scenarioSetInitId=0 --scenarioDIR data/scenarios/representativeSet_Intersections/ --paramFile xml/config/sf-param-config-multi.xml --statsOnly -p 1 --benchmark compositePLE --optimizer="NSGA2" -c
    # 3d multi opjective optimization
    # clear; clear; time nice -4 optirun python OptimizeAlgorithm.py --ai sfAI --checkAgentInteraction --numScenarios 1 --scenarioSetInitId=0 --scenarioDIR data/scenarios/customSet/bottleneck-hallway/ --paramFile xml/config/sf-param-config-multi.xml --statsOnly -p 1 --benchmark compositePLE --optimizer="NSGA2" --numFrames 2500 --RealDataDir=data/RealWorldData/b140_combined/ -v -c
    # to restart
    # clear; clear; time nice -4 optirun python OptimizeAlgorithm.py --ai sfAI --checkAgentInteraction --numScenarios 1 --scenarioSetInitId=0 --scenarioDIR data/scenarios/customSet/bottleneck-hallway/ --paramFile xml/config/sf-param-config-multi.xml --statsOnly -p 6 --benchmark compositePLE --optimizer="NSGA2" --numFrames 2500 --RealDataDir=data/RealWorldData/b140_combined/ -c --multiObjRestartFile=SteerStatsOpt_1.log
    # clear; clear; time nice -4 optirun python OptimizeAlgorithm.py --ai sfAI --checkAgentInteraction --numScenarios 1 --scenarioSetInitId=0 --scenarioDIR data/scenarios/customSet/bottleneck-hallway/ --paramFile xml/config/sf-param-config-multi.xml --statsOnly -p 6 --benchmark compositePLE --optimizer="NSGA2" --numFrames 2500 --RealDataDir=data/RealWorldData/b140_combined/ -c --multiObjRestartFile=SteerStatsOpt_2.log
    """
    
    options_ = getOptions()
    availableProcesses_=int(options_.processes)
    options_.processes=1
    
    [ai_bounds_, ai_params_list, ai_param_names_list,
     stddev_for_parameters, ai_step_scaling]  = prepareOptimization(options.paramFile, options)
 
    steerStats_ = SteerStats(options_)
    steerStats_.setParamNameDict(ai_param_names_list)
    cmaFilePrefix=options_.cmaFilePrefix
    
    print "Algorithm param names: " + str(ai_param_names_list)
    print ai_bounds_
    print ai_params_list
    
    NDIM_ = len(ai_param_names_list)
    
    # f_ = (steerStats_.performanceMetric, steerStats_.pleMetricGlobal, steerStats_.entropyMetric)
    f_ = (steerStats_.performanceMetric, steerStats_.pleMetricGlobal)
    
    return multiOptimizeWithNSGA2(NDIM=NDIM_, NGEN=3, MU=8, f=f_, 
                                  options=options_, ai_bounds=ai_bounds_, 
                                  availableProcesses=availableProcesses_, 
                                  steerStats=steerStats_, restart=restart_)
    
    
def multiOptimizeWithNSGA2(options, availableProcesses, ai_bounds, steerStats,
                           restart, NDIM=5, NGEN=3, MU=6, f=()):
    
    BOUND_LOW, BOUND_UP = 0, 10000.0
    weights_ = (-1.0,)*len(f)
    weightNames = ['o'+str(i) for i in range(len(weights_))]
    
    creator.create("FitnessMin", base.Fitness, weights=weights_)
    creator.create("Individual", array.array, typecode='d', fitness=creator.FitnessMin)
    
    toolbox = base.Toolbox()
    
    op = OptimizeAlgorithm()
    op.set_penatly_funct(overlapPenalty)
    op.set_penatly_funct(agentNotFInishedPenalty)
    op.set_steerstats(steerStats)

    
    def checkBoundsPop(min, max):
        def decorator(func):
            def wrapper(*args, **kargs):
                offspring = func(*args, **kargs)
                for child in offspring:
                    for i in xrange(len(child)):
                        # print "************************ checking range"
                        if child[i] > max[i]:
                            child[i] = max[i]
                        elif child[i] < min[i]:
                            child[i] = min[i]
                return offspring
            return wrapper
        return decorator
    
    def checkBounds(min, max):
        def decorator(func):
            def wrapper(*args, **kargs):
                offspring = func(*args, **kargs)
                for i in xrange(len(offspring)):
                    # print offspring[i]
                    if offspring[i] > max[i]:
                        offspring[i] = max[i]
                    elif offspring[i] < min[i]:
                        offspring[i] = min[i]
                return offspring
            return wrapper
        return decorator
    
    
    def uniform(low, up, size=None):
        return [random.uniform(a, b) for a, b in zip(low, up)]


    if availableProcesses > 1:
        # pool = multiprocessing.Pool(2)
        # I will assume the user knows what they are doing here.
        pool = Pool(availableProcesses)
        toolbox.register("map", pool.map)
        
    toolbox.register("attr_float", uniform, ai_bounds[0], ai_bounds[1], NDIM)
    toolbox.register("individual", tools.initIterate, creator.Individual, toolbox.attr_float)
    toolbox.register("population", tools.initRepeat, list, toolbox.individual)

    op.set_metric_func(f)
    
    toolbox.register("evaluate", op.evalMulti)
    toolbox.register("mate", tools.cxSimulatedBinaryBounded, low=BOUND_LOW, up=BOUND_UP, eta=20.0)
    toolbox.register("mutate", tools.mutPolynomialBounded, low=BOUND_LOW, up=BOUND_UP, eta=20.0, indpb=1.0/NDIM)
    toolbox.register("select", tools.selNSGA2)
    toolbox.decorate("mate", checkBoundsPop(ai_bounds[0], ai_bounds[1]))
    toolbox.decorate("mutate", checkBoundsPop(ai_bounds[0], ai_bounds[1]))

    CXPB = 0.9

    stats = tools.Statistics(lambda ind: ind.fitness.values)
    stats.register("avg", numpy.mean, axis=0)
    stats.register("std", numpy.std, axis=0)
    stats.register("min", numpy.min, axis=0)
    stats.register("max", numpy.max, axis=0)
    
    logbook = tools.Logbook()
    logbook.header = "gen", "evals", "std", "min", "avg", "max"
    
    weighties = []
    weighties.extend(weightNames)
    weighties.extend(steerStats.getParamNameDict())
    
    if not restart:
        pop = toolbox.population(n=MU)
        print "population: "
        print pop
    
        # Evaluate the individuals with an invalid fitness
        invalid_ind = [ind for ind in pop if not ind.fitness.valid]
        fitnesses = toolbox.map(toolbox.evaluate, invalid_ind)
        for ind, fit in zip(invalid_ind, fitnesses):
            ind.fitness.values = fit
    
        print "Post fitness"
        print pop
        print "ind"
        print ind
        print "invalid_ind"
        print invalid_ind
        print "fit"
        print fit
        print "fitnesses"
        print fitnesses
    else:
        print "restarting from previous data"
        pop = toolbox.population(n=MU)
        dataFile = open(options.multiObjRestartFile, "r")
        fitnesses, invalid_ind = readCSVDictToMutliObjData(dataFile, len(f), weighties) 
        dataFile.close()
        for tmp_p, ind, fit in zip(invalid_ind, pop, fitnesses):
            fit = tuple(fit)
            print "fit: " + str(fit)
            print "tmp_p: " + str(tmp_p)
            print "ind " + str(ind)
            ind.fitness.values = fit
            for i in range(len(ind)):
                ind[i] = float(tmp_p[i])
    # This is just to assign the crowding distance to the individuals
    # no actual selection is done
    pop = toolbox.select(pop, len(pop))
    record = stats.compile(pop)
    logbook.record(gen=0, evals=len(invalid_ind), **record)
    print(logbook.stream)

    # Begin the generational process
    for gen in range(1, NGEN):
        # Vary the population
        offspring = tools.selTournamentDCD(pop, len(pop))
        offspring = [toolbox.clone(ind) for ind in offspring]
        # print offspring
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
        tmp_pop = copy.deepcopy(pop)
        tmp_pop.sort(key=lambda x: x.fitness.values)
        front = numpy.array([ind.fitness.values for ind in tmp_pop])
        
        log_filename = (options.steeringAlgorithm + '_GEN_' + str(gen) + 
            '_POP_' + str(MU) + '.csv') 
        opt_log = open(log_filename, "w")
        saveMatrixToCVSDict(numpy.append(front, pop, axis=1), opt_log, weighties)
        opt_log.close()

    return pop, logbook

    
    
def OptimizeWithCMA():
  
    # clear; clear; time python SteerStats.py --ai pprAI --checkAgentInteraction --numScenarios 5000 --benchmark compositeGraph -c --scenarioSetInitId=0 --scenarioDIR data/scenarios/representativeSet  --statsOnly -p 8
    
    options = getOptions()
    
    [ai_bounds, ai_params_defaults_list, ai_param_names_list,
     stddev_for_parameters, ai_step_scaling]  = prepareOptimization(options.paramFile, options)
 
    steerStats = SteerStats(options)
    steerStats.setParamNameDict(ai_param_names_list)
    cmaFilePrefix=options.cmaFilePrefix
    
    print "Algorithm param names: " + str(ai_param_names_list)
    print ai_bounds
    print ai_params_defaults_list
    
    # sys.exit()
    # print results
    
    # JUst coverage metric
    #result = OptimizeParamsCMA(steerStats.coverageMetric, ai_params_list, 
    #                       stddev_for_parameters, ai_bounds, 
    #                       cmaFilePrefix)
    
     # JUst coverage metric
    #result = OptimizeParamsCMA(steerStats.performanceMetric, ai_params_list, 
    #                        stddev_for_parameters, ai_bounds, cmaFilePrefix)
    # 
    # JUst coverage metric
    # result = OptimizeParamsCMA(steerStats.distanceMetric, ai_params_list, 
    #                       stddev_for_parameters, ai_bounds, cmaFilePrefix)
    
    # Coverage plus frames metric
    #result = OptimizeParamsCMA(steerStats.coveragePlusFramesMetric, ai_params_list, 
     #                       stddev_for_parameters, ai_bounds, cmaFilePrefix)
    
    # coverage plus distance metric
    # result = OptimizeParamsCMA(steerStats.coveragePlusDistanceMetric, ai_params_list, 
    #                      stddev_for_parameters, ai_bounds, cmaFilePrefix)
    
    # coverage + computational_time
    #result = OptimizeParamsCMA(steerStats.coveragePlusPerformanceMetric,
    #                            ai_params_list, 
    #1                       stddev_for_parameters, ai_bounds, cmaFilePrefix)
    
    # computation time + converage + quality_distance
    # result = OptimizeParamsCMA(steerStats.coveragePlusDistancePlusComputationMetric,
    #                            ai_params_list, 
    #                       stddev_for_parameters, ai_bounds,
    #                        cmaFilePrefix)
    
        # computation time + converage + quality_distance
    op = OptimizeAlgorithm(options=options)
    op.set_steerstats(steerStats)
    ppe = PostProcessEvent_TarData(op)
    pped = PostProcessEvent_DumpOptResults(op)
    ppev = PostProcessEvent_MakeVideo(op)
    # op.set_metric_func(steerStats.timeMetricGlobal)
    if options.objectiveFunction != "":
        metric = steerStats.getBoundObjective(options.objectiveFunction)
        if metric is None:
        	print '***** objecive function ' + str(options.objectiveFunction) + ' not found *******'
        	sys.exit(1)
        print "objective Function: " + str(options.objectiveFunction) + ", found: " + str(metric)
        op.set_metric_func(steerStats.getBoundObjective(options.objectiveFunction))
        
    else:
        op.set_metric_func(steerStats.agentFlowMetricGlobal)
        print 'blah'
	
	if options.penaltyFunction != "overlapPenalty":
   		penalty = steerStats.getBoundPenaltyFunc(options.objectiveFunction)
   		op.set_penatly_funct(penalty)
   	else:
   		op.set_penatly_funct(overlapPenalty)
    
    # Does not acctually place an obstacle in the scenario because it comes from options.subspaceParams
    #control_result = steerStats.RunStats((0,0), options=options) # hard coded stuff uggh...
    #control_result = op._metric_func((0,0),results=control_result ,options=options)
    # control_result = steerStats.RunStats(ai_params_defaults_list, options=options)
    # print "control result: " + str(control_result)
    # print "op: " + str(op)
    # control_result = op._metric_func(ai_params_defaults_list,results=control_result ,options=options)
    # op._control_result = control_result
    
    result = OptimizeParamsCMA(op.eval,
                                ai_params_defaults_list, 
                           stddev_for_parameters, ai_bounds,
                           options.cmaProcesses,
                            cmaFilePrefix,
                            ai_step_scaling,
                            maxfevals_=options.cmaMaxEvals)
    
    
    print "Control result: " + str(op._control_result)
    opt_log = open(cmaFilePrefix+"SteerStatsOptResult.txt", "w")
    writeCMAResults(opt_log, result)
    opt_log.close()
    
    # write all of the result to a file.
    op.add_post_process_event(pped)
    # create a heatmap from the results
    # op.add_post_process_event(ppeh)
    # make video
    ppev.set_ai_params(result[0])
    # Record a video of the result
    #if options.config.getProcessingEvents()['PostProcessEvent_MakeVideo'] == 'true':
    #    op.add_post_process_event(ppev)
    # this post processing step should be added last (compressing everything)
    op.add_post_process_event(ppe)
    
    op.post_process()
    print "Done"
    
def writeCMAResults(file, result):
    file.write("best evaluated set of parameters, X: " + str(result[0]) + '\n')
    file.write('best evaluated f for parameters, X: ' + str(result[1]) + '\n')
    file.write('evaluations till best X: ' + str(result[2]) + '\n')
    file.write('geno of x: ' + str(result[3]) + '\n')
    file.write('total evaluations: ' + str(result[4]) + '\n')
    file.write('total iterations f: ' + str(result[5]) + '\n')
    file.write('best mean solution for normal distribution: ' + str(result[6]) + '\n')
    file.write('std deviation of parameters: ' + str(result[7]) + '\n')
    file.write('reason for stopping optimization: ' + str(result[8]) + '\n')
    
def OptimizeWithCMA_ES_MixedInt():
    print "blah"
    options = getOptions()
    
    cmaFilePrefix=options.cmaFilePrefix
    
    result = "blah"
    
    param_xml_file = open(options.paramFile)
    param_config_data = param_xml_file.read()
    
    OptimExper = OptimizationExperiment(param_config_data)
    optimizingParameters = OptimExper.parseExperimentXML()

    stddev_for_parameters=1
    
    scaling_factor=float(0.13)
        
    ai_params_list = []
    ai_param_names_list = []
    ai_lower_bounds = []
    ai_upper_bounds = []
    ai_step_scaling = []
    
    
    for ai_param in optimizingParameters:
        ai_params_list.append(ai_param.get_original())
        ai_param_names_list.append(ai_param.get_name())
        ai_lower_bounds.append(ai_param.get_min())
        ai_upper_bounds.append(ai_param.get_max())
        """ 
        This is magic stuff here
        Scaling stddev for all the parameters acording the min/max values for the search
        apparently this should be 0.3 of the search area if your search area is perfectly round 
        """
        ai_step_scaling.append( (ai_param.get_max()-ai_param.get_min())*scaling_factor )
   
    print "Algorithm param names: " + str(ai_param_names_list)
    ai_bounds = [ai_lower_bounds, ai_upper_bounds]
    
    steerStats = SteerStats(options)
    steerStats.setParamNameDict(ai_param_names_list)
    
    print ai_lower_bounds
    print ai_params_list
    print ai_upper_bounds

    steerStats.pleMetric(ai_params_list, None)
    
def OptimizeWithMIDACO():
    print "Not supported"
    sys.exit(-1)
    
def OptimizeWithBruteForce():
    # Only works for 2D for now
    # clear; clear; time nice -18 python OptimizeAlgorithm.py --ai sfAI --checkAgentInteraction --numScenarios 1 --benchmark compositePLE --statsOnly --scenarioSetInitId 0 --subspace=../subspaces/icra-subspaces/hallway-one-way-100-agents-funnel.xml --dataDir=data/ --numFrames=2000 --optimizeWith=bruteForce --paramFile=xml/config/subspaces/icra-subspaces/hallway-one-way-1pillar-smallOptimizationRegion.xml -c -p 4 --logMetrics
    import time # aint Python crazy like this
    # from multiprocessing import Pool as Pool
    from multiprocessing.pool import ThreadPool
        
    import itertools
    options = getOptions()
    # options.noReturn=True
    availableProcesses=int(options.processes)
    options.processes=int(1)
    steerStats = SteerStats(options)
    # steerStats.set_insert_func(InsertStatistics.InsertOptimizationConfig)
    
    [ai_bounds, ai_params_list, ai_param_names_list,
    stddev_for_parameters, ai_step_scaling]  = prepareOptimization(options.paramFile, options)
 
    # steerStats = SteerStats(options)
    steerStats.setParamNameDict(ai_param_names_list)
    cmaFilePrefix=options.cmaFilePrefix

    param_xml_file = open(options.paramFile)
    param_config_data = param_xml_file.read()
    
    default_params={}
    for param_name,t_param in zip(ai_param_names_list,ai_params_list):
        default_params[param_name]=t_param
        
    print default_params
    
    OptimExper = OptimizationExperiment(param_config_data)
    optimizingParameters = OptimExper.parseExperimentXML()

    op = OptimizeAlgorithm(options=options)
    ppe = PostProcessEvent_TarData(op)
    pped = PostProcessEvent_DumpOptResults(op)
    ppeh = PostProcessEvent_CreateHeatMap(op)
    
    # op._options=options
    # op.set_metric_func(steerStats.timeMetricGlobal)
    op.set_steerstats(steerStats)
    # op.set_metric_func(steerStats.simulationTimeMetricGlobal)
    if options.objectiveFunction != "":
        op.set_metric_func(steerStats.getBoundObjective(options.objectiveFunction))
    else:
        # op.set_metric_func(steerStats.agentFlowMetricGlobal)
        print 'blah'
    
    op.set_penatly_funct(overlapPenalty)
    
    # result = OptimizeParamsCMA(op.eval,
    #                             ai_params_list, 
    #                       stddev_for_parameters, ai_bounds,
    #                       options.cmaProcesses,
    #                        cmaFilePrefix,
    #                        ai_step_scaling)
    
    #opt_log = open(cmaFilePrefix+"SteerStatsOptResult.txt", "w")
    #writeCMAResults(opt_log, result)
    #opt_log.close()
    
    # print "Algorithm param names: " + str(ai_param_names_list)
    # print optimizingParameters
    experiements = []
    experiements_param = []
    aiParams = []
    for param in optimizingParameters:
    
        # this will create a lot of directories but I won't have to worry about
        # syncroniztion and reuse of directories
        for n in range(int(param.get_descetization())):
            # aiParams_tmp = default_params
            aiParams_tmp = {}
            """
            for param_ in optimizingParameters:
                if param_.get_type() == "float":
                    aiParams_tmp[param_.get_name()] = str(float(param_.get_original()))
                elif param_.get_type() == "integer":
                    aiParams_tmp[param_.get_name()] = str(int(param_.get_original()))
            """
            # calculate param value for this iteration
            if param.get_type() == "float":
                paramValue = param.get_min() + (((param.get_max()-param.get_min())/param.get_descetization())*n)
            elif param.get_type() == "integer":
                paramValue = int(param.get_min() + (((param.get_max()-param.get_min())/param.get_descetization())*n))
            aiParams_tmp[param.get_name()] = paramValue
            tmp_params = []
            tmp_param_names = []        
            for key, value in aiParams_tmp.items() :
                tmp_params.append(value)
                tmp_param_names.append(key)
            # print tmp_params
            # print tmp_param_names
            experiements_param.append(copy.deepcopy(paramValue))
            aiParams.append(copy.deepcopy(tmp_param_names))
        
        experiements.append(experiements_param)
        experiements_param = []  
            #TODO stuff here.
    
    
    print "Number of experiments in-line: " + str(len(experiements))
    print "Size of process pool: " + str(availableProcesses)
    #print "experiements: " + str(experiements)
    # print ""
    experiements = list(itertools.product(experiements[0],experiements[1]))
    # print "Cross product: " + str(experiements)
    
    # sys.exit()
    #try: 
    processes_pool = ThreadPool(availableProcesses)
    # results = processes_pool.apply(op.eval, experiements)
    for item in experiements:
        # this ensures the results come out in the same order the the experiemtns are in this list.
        processes_pool.apply_async(op.eval, args = (item, ), callback = op.log_result)
    processes_pool.close()
    processes_pool.join()
    # print op._result_list 
    # Does not acctually place an obstacle in the scenario because it comes from options.subspaceParams
    control_result = steerStats.RunStats((0,0), options=options)
    control_result = op._metric_func((0,0),results=control_result ,options=options)
   
    # print "Control result: " + str(control_result)
    op._result_list = control_result - numpy.array(op._result_list)
    # print "Corrected results" + str( op._result_list) 
    # processes_pool = Pool(availableProcesses)
    # results = processes_pool.map(op.eval, experiements)
    # except :
    # print "Multi-processing failed: "
    op.set_experiements(experiements)
    # results = map(op.eval, experiements)
    print "Waited for subprocess"
    # print "Results: " + str(zip(results,experiements))
	# write all of the result to a file.
    op.add_post_process_event(pped)
    # create a heatmap from the results
    op.add_post_process_event(ppeh)
    # this post processing step should be added last
    op.add_post_process_event(ppe)
    
    op.post_process()
    print "Done"
    
if __name__ == "__main__":
    
    options = getOptions()
    if options.optimizationAlgorithm == "CMA-ES":
        OptimizeWithCMA()
    elif options.optimizationAlgorithm == "midaco":
        OptimizeWithMIDACO()
    elif options.optimizationAlgorithm == "bruteForce":
        OptimizeWithBruteForce()
    elif options.optimizationAlgorithm == "CMA-ES-MixInt":
        OptimizeWithCMA_ES_MixedInt()
    elif options.optimizationAlgorithm == "NSGA2":
        if options.multiObjRestartFile == "":
            pop, stats = multiOptimizeNSGA2()
        else:
            pop, stats = multiOptimizeNSGA2(restart_=True)
        print(stats)
        pop.sort(key=lambda x: x.fitness.values)
        import matplotlib.pyplot as plt
        # 
        front = numpy.array([ind.fitness.values for ind in pop])
        opt_log = open("SteerStatsOpt.log", "w")
    
        saveMatrixToCVS(numpy.append(front, pop, axis=1), opt_log)
        plt.scatter(front[:,0], front[:,1], c="b")
        plt.axis("tight")
        plt.show()
        
    else :
        print "Usage: python OptimizeAlgorithm --optimizationAlgorithm CMA-ES"
