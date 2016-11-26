from steersuitedb.CompositeEntropyBenchmark import CompositeEntropyBenchmark
from steersuitedb.BaseAI import BaseAI

class BaseCompositeEntropy(BaseAI, CompositeEntropyBenchmark):
    
    
    def __init__(self):
        super(BaseCompositeEntropy, self).__init__()
        # multiple inheritance is a pain, this needs to be called so that it is instantiated
        # CompositePLEBenchmark.__init__(self)
        self._something=0
        
    
    def setMetricValuesFromDict(self, valuesDict):
        """Had to rename this method so that parent class does not call this
        causing never ending recursive calls."""
        super(BaseCompositeEntropy, self).setBenchmarkValuesFromDict(valuesDict)
        super(BaseCompositeEntropy, self).setAIValuesFromDict(valuesDict)