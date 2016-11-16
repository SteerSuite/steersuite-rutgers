#!/bin/bash


#mkdir data/ICRAExperiment2/$1

#for k in $(seq $2 $3)
#do
#    file_path=xml/config/subspaces/diff-initial-agents
#    mkdir data/ICRAExperiment2/$1/$1-NEW-$k
#    j=0
#
#    for f in $file_path/*
#    do
#        mkdir data/ICRAExperiment2/$1/$1-NEW-$k/$j
#        clear; clear;
#        time nice -18 python OptimizeAlgorithm.py --ai $1AI --numScenarios 1 --benchmark compositePLE --statsOnly --scenarioSetInitId 0 --scenarioDIR data/scenarios/subspace/ --subspace=$f --#optimizer=CMA-ES --paramFile=xml/config/subspaces/hallway-one-way-$k'pillar.xml' --cmaDumpDir=data/ICRAExperiment2/$1/$1-NEW-$k/$j/ --numFrames=2000 -c --cma-p=12
#        ((j++))
#    done
#done

# clear; clear;
# time nice -18 python OptimizeAlgorithm.py --config configs/icra-configs/config_orca_funnel_p1.xml
# mv -R data/simData/0/scenarioFrames/scenario0/video.mp4 data/CASA-Experiments/ORCA/p1/one-way-funnel/orca_funnel_1p.mp4
# mv data/simData/0 data/CASA-Experiments/ORCA/p1/one-way-funnel/0

clear; clear;
time nice -18 python OptimizeAlgorithm.py --config configs/icra-configs/hallway-twoway-sideEgressGoal/config_orca_twowaySideEgress_p1.xml
cp -R data/simData/0 data/CASA-Experiments/ORCA/p1/two-way-topEgressGoal/0

clear; clear;
time nice -18 python OptimizeAlgorithm.py --config configs/icra-configs/hallway-twoway-sideEgressGoal/config_orca_twowaySideEgress_p2.xml
cp -R data/simData/0 data/CASA-Experiments/ORCA/p2/two-way-topEgressGoal/0

clear; clear;
time nice -18 python OptimizeAlgorithm.py --config configs/icra-configs/hallway-twoway-sideEgressGoal/config_orca_twowaySideEgress_p3.xml
cp -R data/simData/0 data/CASA-Experiments/ORCA/p3/two-way-topEgressGoal/0

clear; clear;
time nice -18 python OptimizeAlgorithm.py --config configs/icra-configs/hallway-twoway-sideEgressGoal/config_orca_twowaySideEgress_p4.xml
cp -R data/simData/0 data/CASA-Experiments/ORCA/p4/two-way-topEgressGoal/0

clear; clear;
time nice -18 python OptimizeAlgorithm.py --config configs/icra-configs/hallway-twoway-sideEgressGoal/config_sf_twowaySideEgress_p1.xml
cp -R data/simData/0 data/CASA-Experiments/SF/p1/two-way-topEgressGoal/0

clear; clear;
time nice -18 python OptimizeAlgorithm.py --config configs/icra-configs/hallway-twoway-sideEgressGoal/config_sf_twowaySideEgress_p2.xml
cp -R data/simData/0 data/CASA-Experiments/SF/p2/two-way-topEgressGoal/0

clear; clear;
time nice -18 python OptimizeAlgorithm.py --config configs/icra-configs/hallway-twoway-sideEgressGoal/config_sf_twowaySideEgress_p3.xml
cp -R data/simData/0 data/CASA-Experiments/SF/p3/two-way-topEgressGoal/0

clear; clear;
time nice -18 python OptimizeAlgorithm.py --config configs/icra-configs/hallway-twoway-sideEgressGoal/config_sf_twowaySideEgress_p4.xml
cp -R data/simData/0 data/CASA-Experiments/SF/p4/two-way-topEgressGoal/0