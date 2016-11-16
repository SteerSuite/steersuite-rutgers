from cma.cma import CMADataLogger
from cma.cma import DEAPCMADataLogger
import sys

# sudo apt-get install python-matplotlib

# cma_data_file_name = "../data/optimization/ppr_over_coverage/outcmaes"
# cma_data_file_name = "../data/optimization/ppr_over_PLE2/outcmaes"
# cma_data_file_name = "../data/optimization/ppr/overCoverage/CMAES"
# cma_data_file_name = "../data/optimization/ppr/overPerformance/overPerformance"
# cma_data_file_name = "../data/optimization/ppr/overDistance/CMAES"
# cma_data_file_name = "../data/optimization/ppr/overCoverage2/CMAES"
# cma_data_file_name = "../data/optimization/ppr/overTime2/CMAES"
# cma_data_file_name = "../data/optimization/ppr/overPLE/CMAES"
# cma_data_file_name = "../data/optimization/rvo2d/overComb3/CMAES"
# cma_data_file_name = "../data/optimization/sf/overDist/CMAES"
cma_data_file_name = "../data/optimization/architecture/2pillar-hallway/pprAI/outcma"
cma_data_file_name = sys.argv[1]


data2 = DEAPCMADataLogger(cma_data_file_name).load()
# data2 = CMADataLogger(cma_data_file_name).load()
data2.plot()
data2.disp()