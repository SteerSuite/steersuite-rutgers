
import matplotlib
from pylab import *
import csv
from matplotlib.backends.backend_pdf import PdfPages
import sys
from os import listdir
from os.path import isfile, join
import os
sys.path.append('../')
from util import saveMatrixToCVSDict

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

def plotData(directory, outfile):   
    onlyfiles = [ f for f in listdir(directory) if isfile(join(directory,f)) and f.endswith(".csv") ]
    print onlyfiles
    objectives = ['u(Av)','f(Av)','eff(Av)','distance(Av)','ple(Av)','time(Av)']
    p_data = [['objectives']]
    p_data[0].extend(objectives)
    for filename in onlyfiles:
        tmp_data=[filename]
        reader = csv.reader(open(directory+filename, 'r'))
        data = cvsToDict(reader)
        best_parameters = []
        # print "In file: " + filename
        for key in objectives:
            best_parameters.append(data[key][-1])
        tmp_data.extend(best_parameters)
        p_data.append(tmp_data)
        
    p_data = sorted(p_data,key=lambda x: x[0])
    # p_data = np.array(p_data)
    _dict = {}
    keys = []
    just_params = []
    for line in p_data:
        _dict[line[0]] = line[1:]
        print _dict[line[0]]
        keys.append(line[0])
        just_params.append(_dict[line[0]])
        
    just_params = zip(*just_params)
    print "just params: " + str(just_params)
#    p_data_2 = zip(*p_data)
#    for line in p_data_2:
#        print line
    
    csvfile = open(outfile, 'w')
    saveMatrixToCVSDict(just_params, csvfile, keys)
    csvfile.close()
     
if __name__ == '__main__':
    if len(sys.argv) != 3:
        print "Usage:"
        print "python " + os.path.basename(__file__) + " <directory> <out_file>"
        print "Example:"
        print "python " + os.path.basename(__file__) + "../../data/optimization/rvo2d/Uni-Variet/ orca_uni.csv"
        print ""
        sys.exit(0)
    dir=sys.argv[1]
    plotData(dir, sys.argv[2])
    