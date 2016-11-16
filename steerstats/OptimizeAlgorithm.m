
%  matlab -nodesktop -nosplash -r OptimizeAlgorithm
% matlab -nodesktop -nosplash -r "options.opts='steak'; options.ai='ppr'; options.cmaLogFilenamePrefix='CMA_PPR'; OptimizeAlgorithm(options)"
% matlab -nodesktop -nosplash -r "options.opts='steak'; options.ai='ppr'; options.cmaLogFilenamePrefix='data/optimization/CMA_PPR/ppr_over_PLE'; options.metric='pleMetric'; options.processes=2; options.numScenarios=30; options.scenarioDIR='data/scenarios/representativeSet_Intersections/'; OptimizeAlgorithm(options)"

% For performance optimization
% matlab -nodesktop -nosplash -r "options.opts='steak'; options.ai='ppr'; options.cmaLogFilenamePrefix='data/optimization/ppr/overPerformance'; options.metric='performanceMetric'; options.processes=7; options.numScenarios=7; options.scenarioDIR='data/scenarios/performanceSanityCheck/'; options.maxFrames=300; options.dataDir='/Volumes/Block0/SteerFit/PPR/Performance2'; OptimizeAlgorithm(options)"
% For coverage optimization
% matlab -nodesktop -nosplash -r "options.opts='steak'; options.ai='ppr'; options.cmaLogFilenamePrefix='data/optimization/ppr/overCoverage/CMAES'; options.metric='varopt'; options.processes=10; options.numScenarios=5000; options.scenarioDIR='data/scenarios/representativeSet_Intersections/'; options.maxFrames=2000; options.dataDir='/Volumes/Block0/SteerFit/PPR/Coverage2'; OptimizeAlgorithm(options)"

% for testing
% matlab -nodesktop -nosplash -r "options.MaxFunEvals=5; options.metricWeights='simulationTime=0.2,ple=0.2,coverage=0.2,distance=0.2,computation=0.2,entropy=0'; options.ai='ppr'; options.cmaLogFilenamePrefix='data/TestCMAES'; options.metric='varopt'; options.processes=2; options.numScenarios=50; options.scenarioDIR='data/scenarios/representativeSet_Intersections/'; options.maxFrames=2000; options.dataDir='data/'; OptimizeAlgorithm(options)"
% matlab -nodesktop -nosplash -r "options.MaxFunEvals=500; options.metricWeights='simulationTime=0.33,ple=0.33,coverage=0,distance=0.33,computation=0,entropy=0'; options.ai='ppr'; options.cmaLogFilenamePrefix='data/TestCMAES'; options.metric='varopt'; options.processes=2; options.numScenarios=50; options.scenarioDIR='data/scenarios/representativeSet_Intersections/'; options.maxFrames=2000; options.dataDir='data/'; OptimizeAlgorithm(options)"
%
% matlab -nodesktop -nosplash -r "options.MaxFunEvals=5; options.metricWeights='simulationTime=0,ple=0,coverage=1,distance=0,computation=0,entropy=0'; options.ai='rvo2d'; options.cmaLogFilenamePrefix='data/TestCMAES'; options.metric='varopt'; options.processes=2; options.numScenarios=50; options.scenarioDIR='data/scenarios/representativeSet_Intersections/'; options.maxFrames=2000; options.dataDir='data/'; options.RealWorldData='data/RealWorldData/uo-050-180-180_combined_MB.txt'; OptimizeAlgorithm(options)"
%
% matlab -nodesktop -nosplash -r "options.MaxFunEvals=5; options.metricWeights='simulationTime=0,ple=0,coverage=1,distance=0,computation=0,entropy=0'; options.ai='footstep'; options.cmaLogFilenamePrefix='data/TestCMAES'; options.metric='collisions'; options.processes=2; options.numScenarios=2; options.scenarioDIR='data/scenarios/customSet/'; options.maxFrames=2000; options.dataDir='data/'; OptimizeAlgorithm(options)"

% For testing footsteps optimization
% matlab -nodesktop -nosplash -r "options.MaxFunEvals=5; options.metricWeights='simulationTime=0,ple=0,coverage=1,distance=0,computation=0,entropy=0'; options.ai='footstep'; options.cmaLogFilenamePrefix='data/TestCMAES'; options.metric='varopt'; options.processes=2; options.numScenarios=10; options.scenarioDIR='data/scenarios/representativeSet_Intersections/'; options.maxFrames=1000; options.dataDir='data/'; OptimizeAlgorithm(options)"



%%%%%%%%%%%% time matlab -nodesktop -nosplash -r "options.MaxFunEvals=500; options.metricWeights='simulationTime=0,ple=0,coverage=1,distance=0,computation=0,entropy=0'; options.ai='footstep'; options.cmaLogFilenamePrefix='data/TestCMAES'; options.metric='varopt'; options.processes=6; options.numScenarios=5000; options.scenarioDIR='data/scenarios/representativeSet_Intersections/'; options.maxFrames=1000; options.dataDir='data/'; OptimizeAlgorithm(options); exit;"
%%%%%%%%%%%%%%

%
% FOr AUthoring Testing
% matlab -nodesktop -nosplash -r "options.MaxFunEvals=5; options.metricWeights='simulationTime=0,ple=0.33,coverage=0.33,distance=0.33,computation=0,entropy=0'; options.ai='hybrid'; options.cmaLogFilenamePrefix='data/TestCMAES'; options.metric='varopt'; options.processes=1; options.numScenarios=1; options.scenarioDIR='data/scenarios/AuthorTest/'; options.maxFrames=2000; options.dataDir='data/'; options.paramConfig='xml/config/ppr-param-config.xml'; OptimizeAlgorithm(options)"

% For octave
% Not currently working mostly because of lack of xml parsing support for octave
%  octave --eval "options.metricWeights='simulationTime=0.2,ple=0.2,coverage=0.2,distance=0.2,computation=0.2'; options.ai='ppr'; options.cmaLogFilenamePrefix='data/TestCMAES'; options.metric='varopt'; options.processes=2; options.numScenarios=50; options.scenarioDIR='data/scenarios/representativeSet_Intersections/'; options.maxFrames=2000; options.dataDir='data/'; OptimizeAlgorithm(options)"


% For running an optimization
% Make sure to change 
%   options.cmaLogFilenamePrefix
%   options.ai
%   options.dataDir
%   options.processes
% matlab -nodesktop -nosplash -r "options.metricWeights='simulationTime=0.2,ple=0.2,coverage=0.2,distance=0.2,computation=0.2'; options.ai='ppr'; options.cmaLogFilenamePrefix='data/optimization/ppr/overDistance/CMAES'; options.metric='varopt'; options.processes=10; options.numScenarios=5000; options.scenarioDIR='data/scenarios/representativeSet_Intersections/'; options.maxFrames=2000; options.dataDir='/Volumes/Block0/SteerFit/PPR/Coverage2/'; OptimizeAlgorithm(options)"
% or 
% matlab -nodesktop -nosplash -r "options.MaxFunEvals=1000; options.metricWeights='simulationTime=0,ple=0,coverage=1,distance=0,computation=0,entropy=0'; options.ai='sf'; options.cmaLogFilenamePrefix='data/optimization/hidac/overCov/CMAES'; options.metric='varopt'; options.processes=10; options.numScenarios=5000; options.scenarioDIR='data/scenarios/representativeSet_Intersections/'; options.maxFrames=2000; options.dataDir='/Volumes/Raidy1/SteerFit2/HiDAC/Cov/'; OptimizeAlgorithm(options)"
% matlab -nodesktop -nosplash -r "options.MaxFunEvals=1000; options.metricWeights='simulationTime=0.2,ple=0.2,coverage=0.2,distance=0.2,computation=0.2,entropy=0'; options.ai='sf'; options.cmaLogFilenamePrefix='data/optimization/sf/overDistance/CMAES'; options.metric='varopt'; options.processes=10; options.numScenarios=5000; options.scenarioDIR='data/scenarios/representativeSet_Intersections/'; options.maxFrames=2000; options.dataDir='/Volumes/Block0/SteerFit/SF/Dist/'; OptimizeAlgorithm(options)"
% matlab -nodesktop -nosplash -r "options.MaxFunEvals=1000; options.metricWeights='simulationTime=0,ple=0,coverage=0,distance=1,computation=0,entropy=0'; options.ai='sf'; options.cmaLogFilenamePrefix='data/optimization/sf/overDist/CMAES'; options.metric='varopt'; options.processes=8; options.numScenarios=5000; options.scenarioDIR='data/scenarios/representativeSet_Intersections/'; options.maxFrames=2000; options.dataDir='/Volumes/Block1/SteerSuite/SteerFit/SF/Dist/'; OptimizeAlgorithm(options)"
 
% or FOR Entropy only because Entropy needs a special config file that will slow down everything else.
% matlab -nodesktop -nosplash -r "options.MaxFunEvals=1500; options.metricWeights='simulationTime=0,ple=0,coverage=0,distance=0,computation=0,entropy=1'; options.ai='ppr'; options.cmaLogFilenamePrefix='data/optimization/ppr/overEntropy2BI/CMAES'; options.metric='varopt'; options.processes=8; options.numScenarios=5000; options.scenarioDIR='data/scenarios/representativeSet_Intersections/'; options.maxFrames=2000; options.dataDir='/Volumes/Raidy1/SteerFit2/PPR/Entropy2/'; options.RealWorldData='data/RealWorldData/bot-300-050-050_combined_MB.txt'; OptimizeAlgorithm(options)"
% matlab -nodesktop -nosplash -r "options.MaxFunEvals=1000; options.metricWeights='simulationTime=0,ple=0,coverage=0,distance=0,computation=0,entropy=1'; options.ai='sf'; options.cmaLogFilenamePrefix='data/optimization/sf/overEntropyBI/CMAES'; options.metric='varopt'; options.processes=1; options.numScenarios=1; options.scenarioDIR='data/scenarios/representativeSet_Intersections/'; options.maxFrames=2000; options.dataDir='/Volumes/Block1/SteerSuite/SteerFit/SF/overEntropyBI/'; options.RealWorldData='data/RealWorldData/uo-050-180-180_combined_MB.txt'; OptimizeAlgorithm(options)"
% For equally weighted combination of all 6
%  matlab -nodesktop -nosplash -r "options.MaxFunEvals=1500; options.metricWeights='simulationTime=0.1666,ple=0.1666,coverage=0.1666,distance=0.1666,computation=0.1666,entropy=0.1666'; options.ai='ppr'; options.cmaLogFilenamePrefix='data/optimization/ppr/overCombE/CMAES'; options.metric='varopt'; options.processes=8; options.numScenarios=5000; options.scenarioDIR='data/scenarios/representativeSet_Intersections/'; options.maxFrames=2000; options.dataDir='/Volumes/Raidy1/SteerFit2/PPR/CombE/'; options.RealWorldData='data/RealWorldData/bot-300-050-050_combined_MB.txt'; OptimizeAlgorithm(options)"
%
% matlab -nodesktop -nosplash -r "options.MaxFunEvals=500; options.metricWeights='simulationTime=0.75,ple=0,coverage=0,distance=0.25,computation=0,entropy=0'; options.ai='rvo2d'; options.cmaLogFilenamePrefix='data/optimization/ppr/overCombDist25Time75/CMAES'; options.metric='varopt'; options.processes=8; options.numScenarios=5000; options.scenarioDIR='data/scenarios/representativeSet_Intersections/'; options.maxFrames=2000; options.dataDir='/Volumes/Raidy1/SteerFit2/ORCA/CombDist25Time75/'; options.RealWorldData='data/RealWorldData/bot-300-050-050_combined_MB.txt'; OptimizeAlgorithm(options)"


% for large scale benchmarks
% ple
% matlab -nodesktop -nosplash -r "options.MaxFunEvals=500; options.metricWeights='simulationTime=0,ple=0,coverage=1,distance=0,computation=0,entropy=0'; options.ai='sf'; options.cmaLogFilenamePrefix='data/optimization/sf/global/ple/4way/CMAES'; options.metric='pleMetricGlobal'; options.processes=1; options.numScenarios=1; options.scenarioDIR='data/scenarios/customSet/hallway-4way/'; options.maxFrames=2500; options.dataDir='/Clust_0/SteerSuite/SteerFit/SF/PLE/Hallway4/'; OptimizeAlgorithm(options)"
% matlab -nodesktop -nosplash -r "options.MaxFunEvals=500; options.metricWeights='simulationTime=0,ple=0,coverage=1,distance=0,computation=0,entropy=0'; options.ai='sf'; options.cmaLogFilenamePrefix='data/optimization/sf/global/ple/4way/CMAES'; options.metric='pleMetricGlobal'; options.processes=1; options.numScenarios=1; options.scenarioDIR='data/scenarios/customSet/bottleneck-evac/'; options.maxFrames=2500; options.dataDir='/Clust_0/SteerSuite/SteerFit/SF/PLE/BottleEvac/'; OptimizeAlgorithm(options)"
% time
% matlab -nodesktop -nosplash -r "options.MaxFunEvals=500; options.metricWeights='simulationTime=0,ple=0,coverage=1,distance=0,computation=0,entropy=0'; options.ai='sf'; options.cmaLogFilenamePrefix='data/optimization/sf/global/ple/4way/CMAES'; options.metric='timeMetricGlobal'; options.processes=1; options.numScenarios=1; options.scenarioDIR='data/scenarios/customSet/bottleneck-evac/'; options.maxFrames=2500; options.dataDir='/Clust_0/SteerSuite/SteerFit/SF/PLE/BottleEvac/'; OptimizeAlgorithm(options)"
% for tethys
% matlab -nodesktop -nosplash -r "options.MaxFunEvals=500; options.metricWeights='simulationTime=0,ple=0,coverage=1,distance=0,computation=0,entropy=0'; options.ai='sf'; options.cmaLogFilenamePrefix='data/optimization/sf/global/ple/4way-obs2/CMAES'; options.metric='pleMetricGlobal'; options.processes=1; options.numScenarios=1; options.scenarioDIR='data/scenarios/customSet/hallway-4way-obstacle/'; options.maxFrames=2500; options.dataDir='/Clust_0/SteerSuite/SteerFit/SF/PLE/Hallway4Obs/'; OptimizeAlgorithm(options)"
% matlab -nodesktop -nosplash -r "options.MaxFunEvals=500; options.metricWeights='simulationTime=0,ple=0,coverage=1,distance=0,computation=0,entropy=0'; options.ai='rvo2d'; options.cmaLogFilenamePrefix='data/optimization/rvo2d/global/dist/4way-obs2/CMAES'; options.metric='distanceMetricGlobal'; options.processes=1; options.numScenarios=1; options.scenarioDIR='data/scenarios/customSet/hallway-4way-obstacle/'; options.maxFrames=2500; options.dataDir='/Clust_0/SteerSuite/SteerFit/ORCA/Dist/Hallway4Obs/'; OptimizeAlgorithm(options)"
% distance
% for titan
% matlab -nodesktop -nosplash -r "options.MaxFunEvals=500; options.metricWeights='simulationTime=0,ple=0,coverage=1,distance=0,computation=0,entropy=0'; options.ai='sf'; options.cmaLogFilenamePrefix='data/optimization/sf/global/ple/4way-obs2/CMAES'; options.metric='pleMetricGlobal'; options.processes=1; options.numScenarios=1; options.scenarioDIR='data/scenarios/customSet/hallway-4way/'; options.maxFrames=2500; options.dataDir='/Volumes/Raidy1/SteerFit2/SF/Global/Hallway4/'; OptimizeAlgorithm(options)"
function [X, F, E, STOP, OUT] = OptimizeAlgorithm(options)
    addpath('matlab/');
    addpath('matlab/steerstats/');

    if isfield(options, 'paramConfig')
        opExp = optimizationexperiment(strcat(options.paramConfig));
    else
        opExp = optimizationexperiment(strcat('xml/config/',options.ai,'-param-config.xml'));
    end
    params = parse_experiment(opExp);
    % params
    % fprintf('number of parameters: %d', params.getLength());
    % metric
    original_params = zeros(size(params,2),1);
    param_mins = zeros(size(params,2),1);
    param_maxes = zeros(size(params,2),1);
    param_types = cell(size(params,2),1);
    stair_widths = zeros(size(params,2),1);
    for i = 1:size(params,2)
       % disp(get_original_value(params(i))) ;
       original_params(i) = get_original_value(params(i));
       % disp(get_type(params(i))) ;
       param_types(i) = get_type(params(i));
       if strcmp(get_type(params(i)), 'integer') ~= 0
          stair_widths(i) = 1.0; 
       end
       param_mins(i) = get_min(params(i));
       param_maxes(i) = get_max(params(i));

    end
    
    %% Now for subspace optimization 
	if isfield(options, 'subspace')
		subspace=options.subspace;
		subspaceExp = optimizationexperiment(['xml/config/subspaces/', subspace, '.xml']);
		subspaceParams = parse_experiment(subspaceExp);
		
		subspace_original_params = zeros(size(subspaceParams,2),1);
	    subspace_param_mins = zeros(size(subspaceParams,2),1);
	    subspace_param_maxes = zeros(size(subspaceParams,2),1);
	    subspace_param_types = cell(size(subspaceParams,2),1);
	    subspace_stair_widths = zeros(size(subspaceParams,2),1);
	    for i = 1:size(subspaceParams,2)
	       subspace_original_params(i) = get_original_value(subspaceParams(i));
	       subspace_param_types(i) = get_type(subspaceParams(i));
	       if strcmp(get_type(subspaceParams(i)), 'integer') ~= 0
	          subspace_stair_widths(i) = 1.0; 
	       end
	       subspace_param_mins(i) = get_min(subspaceParams(i));
	       subspace_param_maxes(i) = get_max(subspaceParams(i));
	
	    end
	    original_params = subspace_original_params;
	    param_mins = subspace_param_mins;
	    param_maxes = subspace_param_maxes;
	    param_types = subspace_param_types;
	    stair_widths = subspace_stair_widths;
	end
	
    fprintf( 'About to call cmaes\n')

	original_params
	param_mins
	param_maxes
    opts=cmaes; 
    opts.StopFitness=-Inf;
    opts.TolFun=1e-5; % when function change less what this stop
    opts.TolHistFun=1e-10; %When opt stops over history
    if isfield(options, 'MaxFunEvals')
        fprintf('setting MaxFunEvals to %d\n', options.MaxFunEvals);
        % opts.MaxFunEvals=options.MaxFunEvals;
        opts.StopFunEvals=options.MaxFunEvals;
    else
        opts.StopFunEvals=2500;
    end
    
    % This allows a saved optimization to be resumed after stopping
    if isfield(options, 'Resume')
    		opts.Resume='yes';
    end
    
    % opts.MaxFunEvals=2;
    opts.LBounds = param_mins; opts.UBounds = param_maxes; 
    opts.StairWidths = stair_widths;
    % opts.LogFilenamePrefix = 'data/optimization/ppr_over_Performance/CMAES'; %options.cmaLogFilenamePrefix;
    opts.LogFilenamePrefix = options.cmaLogFilenamePrefix;
    % This option is used to save a log file that can be used to restart
    % the optimization from previous data.
    opts.SaveFilename = strcat(options.cmaLogFilenamePrefix, 'variablescmaes.mat');
    outDataFID = fopen(strcat(options.cmaLogFilenamePrefix, 'bestever.txt'),'w');
    f_default = steerStatsWrapper(original_params, options);
    
    if isfield(options, 'restarts')
    	defopts.Restarts=options.restarts
    end
    
    % cmaes(  steerStatsWrapper,  transpose([0, 2]),    0.24)
    % Everything after opts will be passed to steerStatsWrapper
    [XMIN, FMIN, COUNTEVAL, STOPFLAG, OUT, BESTEVER] = cmaes('steerStatsWrapper', original_params, [], opts, options)
    
    fprintf(outDataFID, 'f-default: %f\n', f_default);
    fprintf(outDataFID, '\n');
    fprintf(outDataFID, 'default args: ');
    for param = 1:1:length(original_params)
        if param < length(original_params) % Not equal
        	% This should stay with ',' so that I can re-run the data quick to get
        	% a better f value
            fprintf(outDataFID, '%f,', original_params(param));
        else
            fprintf(outDataFID, '%f', original_params(param));
        end
    end
    fprintf(outDataFID, '\n\n');
    fprintf(outDataFID, 'f-opt: %f\n', BESTEVER.f);
    fprintf(outDataFID, 'param args: ');
    for param = 1:1:length(BESTEVER.x)
        if param < length(BESTEVER.x) % Not equal
        	% This should stay with ',' so that I can re-run the data quick to get
        	% a better f value
            fprintf(outDataFID, '%f,', BESTEVER.x(param));
        else
            fprintf(outDataFID, '%f', BESTEVER.x(param));
        end
    end
    
    fprintf(outDataFID, '\n\n');
    fprintf(outDataFID, 'column args: ');
    for param = 1:1:length(BESTEVER.x)
        if param < length(BESTEVER.x) % Not equal
        	% This should stay with ',' so that I can re-run the data quick to get
        	% a better f value
            fprintf(outDataFID, '%f ', BESTEVER.x(param));
        else
            fprintf(outDataFID, '%f', BESTEVER.x(param));
        end
    end
    
    fclose(outDataFID);
end
