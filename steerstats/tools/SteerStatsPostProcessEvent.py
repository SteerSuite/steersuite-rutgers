from abc import ABCMeta, abstractmethod
import sys
sys.path.append('../')
# from OptimizeAlgorithm import OptimizeAlgorithm

"""
	The idea is to setup the processing before the simulation starts. 
	This will ensure that the program will not fail when process() is called
	This step should be done in the class initilization.

"""

class SteerStatsPostProcessEvent(object):
	__metaclass__ = ABCMeta
    
	@abstractmethod
	def process(self):	
		pass
	
	def set_OptimizationAlgorithm(self, op_):
		self._op = op_

	def get_OptimizationAlgorithm(self):
		return self._op

class PostProcessEvent_TarData(SteerStatsPostProcessEvent):
	"""
		compresses all of the data from the simulation that was stored in dataDir
	"""
	def __init__(self, op):
		from datetime import datetime
		options = op.get_options()
		self.set_OptimizationAlgorithm(op)
		timeNow = str(datetime.now().strftime('%Y-%m-%d-%H-%M-%S.%f'))
		self._files_to_add = []
		self._output_file = options.steeringAlgorithm+'_'+timeNow+".tar.gz"
		self._files_to_add.append(options.dataPath)
		self._files_to_add.append(options.configFile)
		if options.subspace != "":
			self._files_to_add.append(options.subspace)
		self._message = "Compressing simulation data"
		
    
	def process(self):	
		import tarfile
		import os
		print self._message
		with tarfile.open(self._output_file, mode="w:gz") as tar:
			for file in self._files_to_add:
				tar.add(file, recursive=True) # add data folder
			
			tar.close()
		print "Done: " + str(self._message)
		
class PostProcessEvent_CreateHeatMap(SteerStatsPostProcessEvent):
	"""
		Plots a heatmap the data [results, points/parameters]
	"""
	def __init__(self, op):
		import os
		from plotting import color_grid
		options = op.get_options()
		self.set_OptimizationAlgorithm(op)
		self._message = "generating heatmap of results"
		if not os.path.exists(op.get_options().dataPath):
			if options.verbose :
				print "Creating directory: " + op.get_options().dataPath
			os.makedirs(os.path.abspath(op.get_options().dataPath))
		self._file = open(op.get_options().dataPath+"/"+"evalData.dat", 'w+')
		
    
	def process(self):	
		from plotting import color_grid
		print self._message
		
		color_grid.generate_heat_map(self._file, self.get_OptimizationAlgorithm().get_options().dataPath+'/'+'test.svg')
		self._file.close()
		print "Done: " + str(self._message)
		
class PostProcessEvent_DumpOptResults(SteerStatsPostProcessEvent):
	"""
		Writes a file in the dataDir that is all of the experiments data
		results, parameters
	"""
	
	def __init__(self, op):
		import os
		options = op.get_options()
		self.set_OptimizationAlgorithm(op)
		self._message = "writing optimization results to a file"
		if not os.path.exists(op.get_options().dataPath):
			if options.verbose :
				print "Creating directory: " + op.get_options().dataPath
			os.makedirs(os.path.abspath(op.get_options().dataPath))
		self._file = open(op.get_options().dataPath+"/"+"evalData.dat", 'w+')
		
    
	def process(self):	
		print self._message
		
		self.get_OptimizationAlgorithm().get_steerstats().set_log_File(self._file)
		# print 'experiments: ' + str(self.get_OptimizationAlgorithm().get_experiements())
		for res, exp in zip(self.get_OptimizationAlgorithm().get_results(),self.get_OptimizationAlgorithm().get_experiements()):
			data = [res]
			data.extend(exp)
			self.get_OptimizationAlgorithm().get_steerstats().log(data)
		
		self._file.close()
	        
		print "Done: " + str(self._message)
		
class PostProcessEvent_MakeVideo(SteerStatsPostProcessEvent):
	"""
		compresses all of the data from the simulation that was stored in dataDir
	"""
	def __init__(self, op):
		from datetime import datetime
		import copy
		# op = copy.deepcopy(op)
		self.set_OptimizationAlgorithm(op)
		self._message = "Rendering optimal simulation video"
		

	def process(self):
		import copy
			
		options = copy.deepcopy(self.get_OptimizationAlgorithm().get_options())
		options.subspaceParams = self._params
		options.makeVideo=True
		options.saveFrames=True
		options.commandLine=False
		self.get_OptimizationAlgorithm().get_steerstats().RunStats(self._params, options=options)
		print "Done: " + str(self._message)
		
	def set_ai_params(self, params):
		self._params = params