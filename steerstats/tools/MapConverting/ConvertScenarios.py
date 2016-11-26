import glob
import sys
from ConstructScenarios import constructScenarios2
import os


map_dir = sys.argv[1]
output_dir = sys.argv[2]
maps = glob.glob(map_dir+"*.map")
print maps
maps.sort(cmp=None, key=None, reverse=False)
print maps

# print maps.index('../../../testcases/dragon_age/map_files/den012d.map')
maps2 = maps
# print maps
n=0
for map_name in maps2:
    scen_name = map_name+".scen"
    print "Working on map: " + map_name
    
    map_name_base = os.path.basename(map_name)
    
    
    print map_name_base
    map_basename = os.path.splitext(map_name_base)[0]
    temp_output_dir=output_dir+"/"+map_basename+"/"
    if not os.path.exists(temp_output_dir):
        print "Creating directory: " + temp_output_dir
        os.makedirs(os.path.abspath(temp_output_dir))
        
    n = constructScenarios2(map_name, scen_name, temp_output_dir, n)
    print "Number of scenarios created so far: " + str(n)
    
print "Total number of scenarios created: " + str(n)