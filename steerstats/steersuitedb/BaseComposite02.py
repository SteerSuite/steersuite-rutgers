from steersuitedb.Composite2Benchmark import Composite2Benchmark
from steersuitedb.BaseAI import BaseAI

class BaseComposite02(BaseAI, Composite2Benchmark):
    
    
    def __init__(self):
        super(BaseComposite02, self).__init__()
        # multiple inheritance is a pain, this needs to be called so that it is instantiated
        # CompositePLEBenchmark.__init__(self)
        
    
    def setMetricValuesFromDict(self, valuesDict):
        """Had to rename this method so that parent class does not call this
        causing never ending recursive calls."""
        super(BaseComposite02, self).setBenchmarkValuesFromDict(valuesDict)
        super(BaseComposite02, self).setAIValuesFromDict(valuesDict)