

import sys
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.backends.backend_pdf import PdfPages

sys.path.append('../../')
from util import readCSVToMatrix
file_path = sys.argv[1]
file = open(file_path)


def plotErrorRibbon(x, mu, std, metric, pp):
	fig, ax = plt.subplots(1)
	ax.fill_between(x, mu+std, mu-std, facecolor='blue', alpha=0.5, label='$\pm \sigma$')
	ax.plot(data[:,0], mu, alpha=0.7, label='$\mu$', linewidth=2.0)
	# ax.set_title(metric + ' convergence', fontsize=22)
	ax.legend(loc='upper right')
	ax.set_xlabel('number of scenario samples', fontsize=20)
	ax.set_ylabel('standard deviation $\mu$ $\pm \sigma$', fontsize=20)
	ax.tick_params(axis='both', which='major', labelsize=16)
	ax.tick_params(axis='both', which='minor', labelsize=14)
	plt.axis("tight")
	plt.savefig('convergence_plot_' + metric + '.svg', format='svg', dpi=1000, bbox_inches='tight')
	pp.savefig(fig, bbox_inches='tight')
	# plt.show()
	
	
if __name__ == '__main__':
	
	print file

	data = readCSVToMatrix(file, delimiter=',')
	
	print data
	
	pp = PdfPages("convergence" + '_plots.pdf')
	
	plotErrorRibbon(data[:,0], data[:,1], data[:,5], 'coverage', pp)
	plotErrorRibbon(data[:,0], data[:,2], data[:,6], 'time', pp)
	plotErrorRibbon(data[:,0], data[:,3], data[:,7], 'distance', pp)
	plotErrorRibbon(data[:,0], data[:,4], data[:,8], 'effort', pp)
	
	# plotData(pp, filename, parameter)
	pp.close()
    