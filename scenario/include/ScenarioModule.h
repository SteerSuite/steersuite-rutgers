//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//



#ifndef __SCENARIO_MODULE__
#define __SCENARIO_MODULE__

/// @file SimpleAIModule.h
/// @brief Declares the SimpleAIModule plugin.

//Example:
//steersim.exe -module scenario,scenarioAI=simpleAI

#include "SteerLib.h"
#include "SimulationPlugin.h"
#include "LogManager.h"
#include "mersenne/MersenneTwister.h"


#include "util/FrameSaver.h"

// For creating directories to store data.
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

// globally accessible to the simpleAI plugin
extern SteerLib::EngineInterface * gEngine;
extern SteerLib::SpatialDataBaseInterface * gSpatialDatabase;

typedef struct {
	double angle;
	double radius;
	int thetaRes;
	int radiusRes;
} Region;

// A bunch of flag for drawing different things
// for Feature Space
// #define DrawPaths 1
#define PathLineThickness 0.5f
// #define DrawPathsRef 1
// #define DrawGoals 1
// #define DrawIntersections 1
// #define DrawPathIntersections 1
// #define DrawIntersectionsRef
// #define DrawPathInteractionsRef
// #define DrawPathInteractions
// #define DrawPathInteractionCircles
// #define DrawPathInteractionCirclesRef
// #define DrawInteractionDifficultyCircles
// #define DrawInteractionDifficultyCirclesRef

/**
 * @brief An example plugin for SteerSim that provides very basic AI agents.
 *
 * This class is an example of a plug-in module (as opposed to a built-in module).
 * It compiles as part of a dynamic library which is loaded by SteerSim at run-time.
 *
 * The simpleAI plugin consists of three parts:
 *  - This class inherits from SteerLib::ModuleInterface, and implements only the desired functionality.  In this case
 *    the desired functionality is to be able to create/destroy SimpleAgent agents.
 *  - The two global functions createModule() and destroyModule() are implemented so that the engine can load the
 *    dynamic library and get an instance of our SimpleAIModule.
 *  - The SimpleAgent class inherits from SteerLib::AgentInterface, which is the agent steering AI used by the engine.
 *    this agent serves as an example of how to create your own steering AI using SteerLib features.
 *
 */
class ScenarioModule : public SteerLib::ModuleInterface
{
public:
	std::string getDependencies() { return std::string(""); }
	std::string getConflicts() { return std::string(""); }
	std::string getData() { return _data; }
	LogData * getLogData()
	{
		LogData * lD = new LogData();
		lD->setLogger(this->_benchmarkLogger);
		lD->setLogData(this->_logData);
		return lD;
	}
	void init( const SteerLib::OptionDictionary & options, SteerLib::EngineInterface * engineInfo );
	void finish();

	/// This function is called before preprocessSimulation() to allow a module to create/initialize agents, obstacles, and any other data structures that should be allocated before the simulation begins.
	void initializeSimulation();
	/// This function is called after postprocessSimulation() to allow a module to cleanup/destroy agents and obstacles, and any other data structures that were allocated for the simulation.
	void cleanupSimulation();
	/// This update function is called once before the simulation begins.
	void preprocessSimulation();
	/// This update function is called once after the simulation ends; agents are still allocated when this is called, and ideally should be cleaned up by the modules that created them.
	void postprocessSimulation();
	/// This update function is called once per frame before all agents are updated.
	void preprocessFrame(float timeStamp, float dt, unsigned int frameNumber);
	/// This update function is called once per frame after all agents are updated.
	void postprocessFrame(float timeStamp, float dt, unsigned int frameNumber);
	/// This function called when user interacts with the program using the keyboard, called if the engine did not already recognize the keypress.
	void processKeyboardInput(int key, int action );
	/// Uses OpenGL to draw any module-specific information to the screen; <b>WARNING:</b> this may be called multiple times per simulation step.
	void draw();

private:

		/// generates scenario denoted by index 
		void generateScenario (unsigned int index);
		
		/// generates a random scenario within a non-discretized space
		void generateRandomScenario(bool reduced, bool egocentric);
		void generateRandomScenario2(bool reduced, bool egocentric);
		
		/// loads the scenario from the information in the given testCase
		void loadScenario(std::string scenarioPath, unsigned int currentScenarioNum);
		void loadScenario(SteerLib::TestCaseReader *);
		void loadSubSpace(std::string scenarioPath);
		void generateNextSubSpace();

		void setRandomNumberGenerator (long seed, long long numberOfRandCalls );

		/// loads the currently generated scenario for the ai module 
		void loadCurrentScenario ();

		/// loads the ai module specified in command line argument 
		void loadAIModule ();

		/// loads the steerBench module 
		void loadBenchmarkModule ();

		/// load simulation recorder module 
		void loadSimulationRecorderModule ();

		/// records certain elements of a scenario for SCA 2011
		void recordScenario(std::ofstream &outStream) ;
		void recordHeaderForScenarios(std::fstream &outStream) ;

		void clearScenario ();

		bool isAgentValid(SteerLib::AgentInitialConditions &ac,std::vector<Util::Point> &path  );
		bool isAgentRelevant( const SteerLib::AgentInitialConditions &ac,std::vector<Util::Point> &path  );
		bool isObstacleRelevant( const SteerLib::BoxObstacle &ac );

		/*
		isAgentValid -- does he have a path to goal 
		isAgentRelevant -- is he interacting with the egocentric agent 
		isObstacleRelevant -- is it near the path the agent takes to reach his goal ???? --> 
		*/

		void generateFailureSet(std::string &logFileName, std::set<long long> & failureSet, int numAlgos=1 );

		void writeFailureSet ( std::string &logFileName, std::set<long long> & failureSet);
		void readFailureSet ( std::string &logFileName ); // this populates the failure set from failureLog and runs them 

		void incrementRandomNumberGenerator ( long long numberOfRandomCalls);

		void saveScenario();

private:

	std::vector<long long> _failureSet;
	int _currentFailureSetIndex; 
	bool _runFailureSet; 

	bool _checkAgentValid;
	bool _checkAgentRelevant;
	bool _checkObstacleValid;
	bool _checkObstacleRelevant; 

	bool _fixedDirection; // if true, faces goal 
	bool _reducedGoals; 
	bool _fixedSpeed;

	// frame dump functionality 
	bool _dumpFrames; 
	bool _dumpFirstFrame;
	std::string _frameDumpRelativePath; 
	std::string _frameBaseName;


	Util::FrameSaver * _frameSaver;

	bool _egocentricFlag; 

	std::vector<std::vector<Util::Point>> agentPaths;

	float optimalPathLength;
	float optimalTime;


	std::vector<Region> regions;

	std::string _aiModuleName;
	std::string _aiModuleSearchPath;
	SteerLib::ModuleInterface * _aiModule;
	SteerLib::SteerBenchModule * _benchmarkModule;
	SteerLib::SimulationRecorderModule * _recorderModule;

	MTRand * randGen;

	double _radius;
	double _randomGeneratorRadius;
	int _gridResX;
	int _gridResY;
	int _maxAgents;
	int _minAgents; 
	int _numAgents; // if i want to specify the exact num of agents using parameter
	double _minSpeed;
	double _maxSpeed;
	long _randomSeed;

	int _maxNumFrames;
	
	/// total possible configurations of a single discretization point
	int _numStates;
	/// number of possible placements per agent
	int _numGoalStates;
	/// total number of scenarios 
	size_t _numScenarios; 
	/// current scenario that is loaded and being simulated 
	unsigned int _currentScenario;
	long long _currentNumberOfRandomCalls;
	
	std::string _recordScenarioFile ;
	bool _recordingScenariosOnly ;

	std::string _benchmarkTechnique; 
	bool _useBenchmark; 

	std::string _recFileBaseName;
	std::string _recFileFolder;

	/// data structures which are populated by generateScenario 
	std::vector<SteerLib::AgentInitialConditions> _agents;
	std::vector<SteerLib::ObstacleInitialConditions*> _obstacles;

	// log manager 
	LogManager * _logManager; 

	std::string _benchmarkLog; 
	Logger * _benchmarkLogger;

	// MUBBASIR MIG 2011 UPDATES
	int _obstacleState; 
	/*
	0 = no obstacles 
	1 = normal random sampling of obstacles as before 
	2 = narrow passage 
	3 = narrow corridors 
	4 = obstacle at origin -- here passageWidth is size of obstacle 
	5 = obstacles placed in center -- require lane formation
	//6 = narrow passage + doorway 

	*/
	float _passageWidth; 
	/*
	1.5 allows 1 agent to go through
	2.5 allows 2 agents to go through
	*/

	// agent positioning 
	int _agentState;
	/*
		1 = normal (random sampling in region which is whole world)
		2 =  agents arranged "regularly" along circumference of a circle with diametrically opposite goals 

		3 = group 
		4 = 2 oncoming groups
		5 = 2 crossing groups 
		6 = group confusion
		7 = i think this was added by cory for his contexts 


	*/

	// option to dump out test cases for each scenario generated
	bool _dumpTestCases;
	std::string _testCaseRelativePath;
	int startingNum;
	bool doNotSimulate;
	unsigned int contextNum;
	bool useContext;
	double obstacleFreq;

	// Glen
	// So options can be passed when loading other modules
	const SteerLib::OptionDictionary * _scenarioOptions;
	// flag tell scenario to load from a set of testcases.
	bool runScenarioSet;
	//
	std::string scenarioSetPath;
	// The initial
	unsigned int scenarioSetInitId;

	bool _checkAgentInteraction;
	float _checkAgentInteractionRadius;

	unsigned int windowSizeX;
	unsigned int windowSizeY;

	std::string scenarioFilePrefix;

	// Used to store the inital static points of agents with the represenatative agent
	std::vector<Util::Point> _refAgentIntersectPoints;
	std::vector<Util::Point> _agentIntersectionPoints;
	std::vector<Util::Point> _agentGoals;
	std::vector<Util::Circle> _agentInitialDisks;
	std::vector<Util::Vector> _agentInitialForwardDirections;

	// Used as a larger function to handle calculating graph data for the
	// CompositeGraph Benchmark.
	void saveGraphData();

	/**
	 * Calculates the points of likely interaction
	 * @param path1 is the first vector of points
	 * @param path2 is the second vector of points
	 * @param distance is the max distance a Point can be in path 2 from a Point
	 * in path one to be considered for interaction
	 * @param interval is the number of points in path2 that will be considered
	 * for every point in path 1. making this number very large will perform a
	 * comparison between every point in both lines.
	 * @param/return interactionPoints will end up with all of the points
	 * of suspected interaction
	 * @return the number of suspected points of interaction.
	 */
	unsigned int calcInteractionPoints( std::vector<Util::Point> &path1 ,
			std::vector<Util::Point> &path2, float distance,
			unsigned int interval, std::vector<Util::Point> * interactionPoints);
	std::vector<Util::Point> _agentStaticPathIntersections;
	std::vector<Util::Point> _refAgentStaticPathIntersections;

	void clearGraphData();
	unsigned int _interactionInterval;
	float _interactionDistance;
	bool _checkStaticPathInteractions;

	bool _calculateMaxFrames;
	float _scenarioTimeThreshold;

	unsigned int _maxNodesToExpandForSearch;

	std::string _data;
	std::vector<LogObject *> _logData;

	SteerLib::TestCaseReader * _subspace;
	std::vector<double> _subspaceParams;
	double wallDoorRadius;
	std::vector<double> _subspaceWallParams;

	bool _prettyLog;
};

#endif
