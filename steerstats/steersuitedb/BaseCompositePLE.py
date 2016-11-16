from steersuitedb.CompositePLEBenchmark import CompositePLEBenchmark
from steersuitedb.BaseAI import BaseAI

class BaseCompositePLE(BaseAI, CompositePLEBenchmark):
    
    
    def __init__(self):
        super(BaseCompositePLE, self).__init__()
        # multiple inheritance is a pain, this needs to be called so that it is instantiated
        # CompositePLEBenchmark.__init__(self)
        self._something=0
        
    
    def setMetricValuesFromDict(self, valuesDict):
        """Had to rename this method so that parent class does not call this
        causing never ending recursive calls."""
        super(BaseCompositePLE, self).setBenchmarkValuesFromDict(valuesDict)
        super(BaseCompositePLE, self).setAIValuesFromDict(valuesDict)