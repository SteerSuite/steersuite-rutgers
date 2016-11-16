#!/bin/bash

mkdir -p data/density/


for subspace in 'oneway'
do
	mkdir -p data/density/hallway-$subspace

   	for ai in 'orca' 'sf' 'ppr'
   	do
   		mkdir -p data/density/hallway-$subspace'/'$ai

   		for pillar in p1 p2 p3 p4
   		do
   			mkdir -p data/density/hallway-$subspace'/'$ai'/'$pillar

			IFS=$'\n' read -d '' -r -a params < data/density/hallway-$subspace'/'$ai'/'$pillar'/params.txt'  			
			line_no=0
			# echo "${params[0]}"
			# echo "${params[4]}"
			
	   		for density in E D C B A
	   		do	   			
	   			mkdir -p data/density/hallway-$subspace'/'$ai'/'$pillar'/'$density

	   			clear; clear; time nice -18 python SteerStats.py --config configs/density/hallway-$subspace'/config_'$ai'_'$subspace'_'$pillar'_'$density'.xml' --dataDir=data/density/hallway-$subspace'/'$ai'/'$pillar'/'$density'/' --subspaceParams=${params[$line_no]} --randomSeed 2891 --numScenarios 200 --commandLine
	   			((line_no++))
		       	
		    done

		done

   	done

done

# mkdir -p data/density-depth/


# for subspace in 'oneway'
# do
# 	mkdir -p data/density-depth/hallway-$subspace

#    	for ai in 'orca' 'sf' 'ppr'
#    	do
#    		mkdir -p data/density-depth/hallway-$subspace'/'$ai

#    		for pillar in p1 p2 p3 p4
#    		do
#    			mkdir -p data/density-depth/hallway-$subspace'/'$ai'/'$pillar

# 			IFS=$'\n' read -d '' -r -a params < data/density-depth/hallway-$subspace'/'$ai'/'$pillar'/params.txt'  			
# 			line_no=0
# 			# echo "${params[0]}"
# 			# echo "${params[4]}"
			
# 	   		for density in E D C B A
# 	   		do	   			
# 	   			mkdir -p data/density-depth/hallway-$subspace'/'$ai'/'$pillar'/'$density

# 	   			for den in E D C B A
# 		   		do	   			
# 		   			mkdir -p data/density-depth/hallway-$subspace'/'$ai'/'$pillar'/'$density'/'$density$den

# 		   			clear; clear; time nice -18 python SteerStats.py --config configs/density/hallway-$subspace'/config_'$ai'_'$subspace'_'$pillar'_'$den'.xml' --dataDir=data/density-depth/hallway-$subspace'/'$ai'/'$pillar'/'$density'/'$density$den'/' --subspaceParams=${params[$line_no]} --randomSeed 2891 --numScenarios 200 --commandLine		   			
			       	
# 			    done

# 	   			((line_no++))
		       	
# 		    done

# 		done

#    	done

# done




# for subspace in 'twoway'
# do
# 	mkdir -p data/density-sims-default/hallway-$subspace

#    	for ai in 'sf'
#    	do
#    		mkdir -p data/density-sims-default/hallway-$subspace'/'$ai
   		
#    		for density in D C B A
#    		do	   			
#    			mkdir -p data/density-sims-default/hallway-$subspace'/'$ai'/'$density

#    			clear; clear; time nice -18 python SteerStats.py --config configs/density/hallway-$subspace'/config_'$ai'_'$subspace'_p1_'$density'.xml' --dataDir=data/density-sims-default/hallway-$subspace'/'$ai'/'$density'/' --randomSeed 2891 --numScenarios 1000 --commandLine

# 	    done

#    	done

# done

# for subspace in 'twoway'
# do
# 	mkdir -p data/density-sims-default/hallway-$subspace

#    	for ai in 'ppr'
#    	do
#    		mkdir -p data/density-sims-default/hallway-$subspace'/'$ai
   		
#    		for density in E D C B A
#    		do	   			
#    			mkdir -p data/density-sims-default/hallway-$subspace'/'$ai'/'$density

#    			clear; clear; time nice -18 python SteerStats.py --config configs/density/hallway-$subspace'/config_'$ai'_'$subspace'_p1_'$density'.xml' --dataDir=data/density-sims-default/hallway-$subspace'/'$ai'/'$density'/' --randomSeed 2891 --numScenarios 1000 --commandLine

# 	    done

#    	done

# done