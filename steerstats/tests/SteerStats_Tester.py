from optparse import OptionParser
import inspect
import sys
sys.path.append("../")
import SteerStats as SteerStats

ss = SteerStats.SteerStats(None)

methods = inspect.getmembers(ss, predicate=inspect.ismethod)
# print methods
objective = 'agentFlowMetricGlobal'
for method in methods:
    print method[0]
    if method[0] == objective:
        print method


print ss.getBoundObjective(objective) 
