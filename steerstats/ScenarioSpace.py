import random
# from pathos.multiprocessing import ProcessingPool as Pool
from steersuite.SteerStatsOptions import getOptions
from SteerStats import SteerStats
from steersuite import init_steerSim
from multiprocessing import Pool
import sys
import numpy as np
# from memory_profiler import profile #  pip install -U memory_profiler; pip install psutil


dataLog = None 
dataLog_data = None
# examples on how to execute this program

# python ScenarioSpace.py --config configs/scenarioSpace/scenarioSpace-config.xml

def runSteerSuite(data):
	_numScenarios=data[0]
	_randSeed=data[1]
	_options=data[2]
	
	args = " -module scenario,scenarioAI="+_options.steeringAlgorithm+",useBenchmark,benchmarkTechnique=compositePLE,checkAgentValid,reducedGoals,fixedSpeed,checkAgentRelevant,minAgents=3,maxFrames=1000,checkAgentInteraction,egocentric,scenarioSetInitId=0,dbName=steersuitedb,skipInsert=True,numScenarios="+str(_numScenarios)+",randomSeed="+str(_randSeed)+" -config "+_options.configFile
	"""scenario,scenarioAI=pprAI,useBenchmark,benchmarkTechnique=compositePLE,checkAgentValid,reducedGoals,fixedSpeed,checkAgentRelevant,minAgents=3,ailogFileName=data/simData/3/pprAI.log,maxFrames=1000,checkAgentInteraction,egocentric,dumpTestCases,testCasePath=data/simData/3/testCases/,randomSeed=2891,numScenarios=14,scenarioSetInitId=36,dbName=steersuitedb,skipInsert=True"""
	if _options.commandLine:
		args = args + " -commandline"
	# args = " -module scenario,scenarioAI=sfAI,useBenchmark,benchmarkTechnique=compositePLE,checkAgentValid,reducedGoals,fixedSpeed,checkAgentRelevant,minAgents=3,maxFrames=1000,checkAgentInteraction,egocentric,scenarioSetInitId=0,numScenarios="+str(_numScenarios)+",randomSeed="+str(_randSeed)+",skipInsert=True,logAIStats -config configs/scenarioSpace/scenarioSpace-config.xml"
	print args
	# sys.exit(0)
	args = args.split(' ')
	result=None
	try:
		result = init_steerSim(len(args), args)
	except Exception as inst:
		print inst
	print "Done running SteerSuite in Python"
	return result

def runSteerSuite2(_numScenarios, _randSeed, _options):
	
	args = " -module scenario,scenarioAI="+_options.steeringAlgorithm+",useBenchmark,benchmarkTechnique=compositePLE,checkAgentValid,reducedGoals,fixedSpeed,checkAgentRelevant,minAgents=3,maxFrames=1000,checkAgentInteraction,egocentric,scenarioSetInitId=0,dbName=steersuitedb,skipInsert=True,numScenarios="+str(_numScenarios)+",randomSeed="+str(_randSeed)+" -config "+_options.configFile
	"""scenario,scenarioAI=pprAI,useBenchmark,benchmarkTechnique=compositePLE,checkAgentValid,reducedGoals,fixedSpeed,checkAgentRelevant,minAgents=3,maxFrames=1000,checkAgentInteraction,egocentric,testCasePath=data/simData/3/testCases/,randomSeed=2891,numScenarios=14,scenarioSetInitId=36,dbName=steersuitedb,skipInsert=True"""
	if _options.commandLine:
		args = args + " -commandline"
	# args = " -module scenario,scenarioAI=sfAI,useBenchmark,benchmarkTechnique=compositePLE,checkAgentValid,reducedGoals,fixedSpeed,checkAgentRelevant,minAgents=3,maxFrames=1000,checkAgentInteraction,egocentric,scenarioSetInitId=0,numScenarios="+str(_numScenarios)+",randomSeed="+str(_randSeed)+",skipInsert=True,logAIStats -config configs/scenarioSpace/scenarioSpace-config.xml"
	print args
	# sys.exit(0)
	args = args.split(' ')
	result=None
	try:
		result = init_steerSim(len(args), args)
		# print result
	except Exception as inst:
		print "Error running SteerSuite in Python" + str(inst)
	print "Done running SteerSuite in Python"
	# print result
	return result

def handleSteerSuite(result):
	# print result[1]
	# do nothing for now
	options = result
	# print options
	# cov = (coverage(result[0], options))
	# dataLog_data.write(str(options.numScenarios)+", " + str(cov) + "\n")
	# dataLog_data.flush()
	# metrics.append(cov)
	# print "Coverage is: " + str(cov)
	
def runScenarioSpace():
	options = getOptions()
	steerstats = SteerStats(options)
	options.dumpTestcases=True
	random.seed(10)
	totalSimulations = 0
	samples=10
	step = 50
	dataLog = open("scenarioSpace.cvs", "w+") 
	for nScenarios in range(step, int(501), step):
		for i in range(0, samples, 1):
			options.numScenarios = nScenarios
			steerstats._options.numScenarios = nScenarios
			_rand = random.randint(1, 5000)
			options.randomSeed=_rand
			results = steerstats.RunStats(None, None, options)
			
			cov = steerstats.coverageMetric(None, results, options)
			dataLog.write(str(nScenarios)+", " + str(cov) + "\n")
			print "Coverage is: " + str(cov)
			totalSimulations = totalSimulations + nScenarios
		dataLog.flush()
		# this should just save the mean and std after every group...
	
	print "totalSimulations: " + str(totalSimulations)
	

def runScenarioSpace2():
	options = getOptions()
	steerstats = SteerStats(options)
	options.dumpTestcases=True
	random.seed(10)
	totalSimulations = 0
	samples=25
	step = 500
	p = Pool(int(options.processes))
	dataLog = open("scenarioSpace.cvs", "w+") 
	dataLog_data = open("scenarioSpace_data.cvs", "w+") 
	for nScenarios in range(step, int(options.numScenarios), step):
	# for nScenarios in range(step, int(501), step):
		_rands = []
		_nScenarios = []
		for i in range(0, samples, 1):
			totalSimulations = totalSimulations + nScenarios
			
			_rands.append(random.randint(1, 5000))
			# args = " -module scenario,scenarioAI=sfAI,useBenchmark,benchmarkTechnique=compositePLE,checkAgentValid,reducedGoals,fixedSpeed,checkAgentRelevant,minAgents=3,maxFrames=1000,checkAgentInteraction,egocentric,scenarioSetInitId=0,numScenarios="+str(nScenarios)+",randomSeed="+str(_rand)+",skipInsert=True,logAIStats -config configs/scenarioSpace/scenarioSpace-config.xml -commandLine"
			# args = args.split(' ')
			_nScenarios.append(nScenarios)
		process_pairs = zip(_nScenarios, _rands, [options] * len(_rands))
		options.numScenarios = nScenarios
		# print process_pairs
		# print zip(_nScenarios, _rands)
		# sys.exit(0)
		results = p.map(runSteerSuite, process_pairs)
		metrics = []
		for result in results:
			if result is not None:
				cov = (coverage(result, options))
				dataLog_data.write(str(nScenarios)+", " + str(cov) + "\n")
				dataLog_data.flush()
				metrics.append(cov)
				print "Coverage is: " + str(cov)
		metrics = np.array(metrics)
		
		dataLog.write(str(nScenarios)+", " + str(np.mean(metrics)) + ", " + str(np.std(metrics)) + "\n")
		dataLog.flush()
		# this should just save the mean and std after every group...
	
	print "totalSimulations: " + str(totalSimulations)


# @profile
def runScenarioSpace3():
	options = getOptions()
	steerstats = SteerStats(options)
	options.dumpTestcases=True
	random.seed(10)
	totalSimulations = 0
	samples=24
	step = 500
	timelimit=600
	dataLog = open("scenarioSpace.cvs", "w+") 
	dataLog_data = open("scenarioSpace_data.cvs", "w+")
	terminate=False 
	for nScenarios in range(step, int(options.numScenarios), step):
	# for nScenarios in range(step, int(1501), step):
		processes_pool = Pool(int(options.processes))
		terminate=False
		results=[]
		for i in range(0, samples, 1):
			try: 
				totalSimulations = totalSimulations + nScenarios
				steerstats._options.numScenarios = nScenarios
				result = processes_pool.apply_async(runSteerSuite2, args = (nScenarios, random.randint(1, 5000), options, ), callback = handleSteerSuite)
				results.append(result)
			except Exception as inst:
				print "Exception processing result " + str(inst)
				# terminate=True;
				continue
			# result.get(timeout=120)
		# process_pairs = zip(_nScenarios, _rands, [options] * len(_rands))
		# processes_pool.close()
		# print process_pairs
		# print zip(_nScenarios, _rands)
		# sys.exit(0)
		# results = p.map(runSteerSuite, process_pairs)
		metrics = []
		for result in results:
			try: 
				# print "Getting Result"
				_result = result.get(timeout=timelimit)
				# print "RESULT: " + str(_result)
				cov = (coverage(_result, options))
				_time = (qualityTime(_result, options))
				_dist = (qualityPathLength(_result, options))
				_effort = (qualityEffort(_result, options))
				_result=None
				_line = str(nScenarios)+", " + str(cov) + ", " + str(_time) + ", " + str(_dist) + ", " + str(_effort) + "\n"
				print _line
				dataLog_data.write(_line)
				dataLog_data.flush()
				metrics.append([cov, _time, _dist, _effort] )
			except Exception as inst:
				print "Exception getting result " + str(inst)
				terminate=True;
				continue
		if terminate is True:
			# Using seems to leave a lot of memory behind
			processes_pool.terminate() # Will this end processes early?
		else:
			processes_pool.close() # Will this end processes early?
			# processes_pool.terminate() # Will this end processes early?
			
		processes_pool.join()
		processes_pool=None
		metrics = np.array(metrics)
		print "Metrics: " + str(metrics)
		# dataLog.write(str(nScenarios)+", " + str(np.mean(metrics, axis=0)) + ", " + str(np.std(metrics, axis=0)) + "\n")
		dataLog.write(str(nScenarios))
		for _item in np.mean(metrics, axis=0):
			dataLog.write(", " + str(_item))
		for _item in np.std(metrics, axis=0):
			dataLog.write(", " + str(_item))
		dataLog.write("\n")
		dataLog.flush()
		# this should just save the mean and std after every group...
	
	print "totalSimulations: " + str(totalSimulations)
	dataLog.close()
   	dataLog_data.close()


def coverage(results, options): 
	scenarios_passed = sum(results['agent_success'])
	print "Number of scenarios passed " + str(scenarios_passed)
	scenarios_passed_value = ((scenarios_passed))/float(options.numScenarios)
	
	print "percentage scenarios passed: " + str(scenarios_passed_value)
	
	fun_value = ((scenarios_passed_value))
	print "*** evaluation result for coverage: " + str(fun_value)
	return fun_value

def qualityPathLength(results, options): 
	fun_value = np.average(results['length_ratio'])
	print "*** avg length quality: " + str(fun_value)
	return fun_value

def qualityTime(results, options): 
	fun_value = np.average(results['time_ratio'])
	print "*** avg time quality: " + str(fun_value)
	return fun_value

def qualityEffort(results, options): 
	ple_values = (results['ple_energy_dummy'])
	print "*** avg ple quality: " + str(np.average(ple_values))
	penetration_values = results['penetration_penalty']
	# print "*** avg penetration quality: " + str(penetration_values)
	fun_value = np.average(np.array(ple_values) + np.array(penetration_values))
	print "*** avg effort quality: " + str(fun_value)
	
	
	return fun_value


if __name__ == "__main__":

   runScenarioSpace3()
   
