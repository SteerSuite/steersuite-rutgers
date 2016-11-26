
import matplotlib
from pylab import *
import csv
from matplotlib.backends.backend_pdf import PdfPages
import sys

def cvsToDict(reader):
    d = {}
    rows=[]
    keys=[]
    for row in reader:
       rows.append(row)
       
    for item in rows[0]:
        d[item]=[]
        keys.append(item)
        
    for row in rows[1:]:
        i=0
        for item in row:
            d[keys[i]].append(item)
            i=i+1
    
    return d


def createFigure(data, parameter, metric):
    fig = figure()
    ax = fig.add_subplot(111)
    ax.plot(data[parameter][:20], data[metric][:20], linewidth=6.0, color='red', marker='.', 
            markersize=30.0)
    ax.set_xlabel(parameter, fontweight='bold', fontsize=24)
    ax.set_ylabel(metric, fontweight='bold', fontsize=30)
    # ax.set_title(parameter + ' vs ' + metric, fontweight='bold', fontsize=20)
    # ax.get_title().set_visible(False)
    # creates a grid in the background
    ax.tick_params(axis='both', which='major', labelsize=18)
    ax.tick_params(axis='both', which='minor', labelsize=16)
    ax.yaxis.grid(color='gray', linestyle='dashed')
    ax.xaxis.grid(color='gray', linestyle='dashed')
    # Puts the grid behind the line being ploted.
    ax.set_axisbelow(3)
    # plt.axis("tight")
    return fig

def plotData(pp, filename, parameter):   
    reader = csv.reader(open(filename, 'r'))
    data = cvsToDict(reader)
    for key, value in data.items():
        print key
        fig = createFigure(data, parameter, key)
        pp.savefig(fig, bbox_inches='tight')
        
     
if __name__ == '__main__':
    if len(sys.argv) != 3:
        print "Usage:"
        print "python PlotParameterData.py <file_name> <parameter>"
        print "Example:"
        print "python PlotParameterData.py ../../data/optimization/rvo2d/Uni-Variet/rvo2d_max_neighbors.csv rvo_max_neighbors"
        print ""
        sys.exit(0)
    filename=sys.argv[1]
    parameter=sys.argv[2]
    pp = PdfPages(parameter + '_plots.pdf')
    plotData(pp, filename, parameter)
    pp.close()
    