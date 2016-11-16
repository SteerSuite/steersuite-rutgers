//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//



//Example:
//steersim.exe -module scenario,scenarioAI=simpleAI -config mawang.xml
//steersim.exe -module scenario,scenarioAI=egocentricAI,numScenarios=1000 -config ../../config.xml

// For Linux
// run pre-generated set of scenarios
// ../build/bin/steersim -module scenario,scenarioAI=sfAI,numScenarios=10,egocentric,scenarioSetPath=data/scenarios/representativeSet_Intersections/ -config configs/sfAI-config.xml
// run a subspace
// ../build/bin/steersim -module scenario,scenarioAI=sfAI,numScenarios=10,egocentric,subspace=../subspaces/hallway-one-way.xml -config configs/sfAI-config.xml
// run subspace with some parameters
// ../build/bin/steersim -module scenario,scenarioAI=sfAI,numScenarios=10,egocentric,subspace=../subspaces/hallway-one-way.xml,subspaceParams="(0;0;2;2)" -config configs/sfAI-config.xml

// random scenarios
// ../build/bin/steersim -module scenario,scenarioAI=sfAI,useBenchmark,benchmarkTechnique=compositePLE,checkAgentValid,reducedGoals,fixedSpeed,checkAgentRelevant,minAgents=3,maxFrames=1000,checkAgentInteraction,egocentric,randomSeed=2891,numScenarios=14,scenarioSetInitId=0,dbName=steersuitedb,skipInsert=True
// ../build/bin/steersim -module scenario,scenarioAI=egocentricAI,useBenchmark,benchmarkTechnique=compositePLE,checkAgentValid,reducedGoals,fixedSpeed,checkAgentRelevant,minAgents=3,maxFrames=1000,checkAgentInteraction,egocentric,randomSeed=2891,numScenarios=10000,scenarioSetInitId=0,dbName=steersuitedb,skipInsert=True,ailogFileName=egocentricAI.log -config configs/scenarioSpace/scenarioSpace-config.xml -commandLine


#include "SteerLib.h"
#include "SimulationPlugin.h"
#include "ScenarioModule.h"
#include <math.h>
#include <regex>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <algorithm>



#define LINUX FALSE

#if LINUX
#include <sys/time.h>
#endif

#ifdef _WIN32
#include<windows.h>
#include<limits>
#undef max()
#endif

#include "LogObject.h"
#include "Logger.h"
#include "LogManager.h"
#include <sstream>
#include <fstream>

// #define _DEBUG 1

#define DESIRED_SPEED 1.33f // as per the PLE paper 

#define KE 0.5f * DESIRED_SPEED * DESIRED_SPEED

//#include "RVOPlusAIModule.h"

#define	 Min(a,b)               ((a)<(b) ? (a) : (b))

#define Empty_String ""

// globally accessible to the simpleAI plugin
SteerLib::EngineInterface * gEngine;
SteerLib::SpatialDataBaseInterface * gSpatialDatabase;

// #define _DEBUG1 1
// #define _DEBUG2 2


using namespace Util;
using namespace SteerLib;

std::string constructBenchDataString(std::vector<Util::Point> points);
std::string constructBenchDataString(std::vector<float> points);
std::string constructBenchDataString(std::vector<int> points);


int predictIntersection(double sposX, double sposZ, double sgoalX, double sgoalZ,
double tposX, double tposZ, double tgoalX, double tgoalZ)
{
	// implicit form of line 1
	double A1 = sgoalZ -sposZ ; 
	double B1 = sposX - sgoalX ;
	double C1 = A1*sposX + B1*sposZ ;

	// implicit form of line 1
	double A2 = tgoalZ -tposZ ; 
	double B2 = tposX - tgoalX ;
	double C2 = A2*tposX + B2*tposZ ;

	double det = A1*B2-A2*B1 ;
	if( fabs(det) < 1e-6 )
	{
		// parallel lines always indicate intersection, UNREASONABLE
		return 1 ;
	}
	double x = (B2*C1 - B1*C2)/det ;
	double z = (A1*C2 - A2*C1)/det ;

	if( x < tposX && x < tgoalX)
		return 0 ;
        if( x > tposX && x > tgoalX)
                return 0 ;
        if( z < tposZ && z < tgoalZ) 
                return 0 ;
        if( z > tposZ && z > tgoalZ) 
               return 0 ;
        if( x < sposX && x < sgoalX)
                return 0 ;
        if( x > sposX && x > sgoalX)
                return 0 ;
        if( z < sposZ && z < sgoalZ) 
                return 0 ;
        if( z > sposZ && z > sgoalZ)
                return 0 ;
        return 1 ;

}


PLUGIN_API SteerLib::ModuleInterface * createModule()
{
	return new ScenarioModule;
}

void ScenarioModule::init( const SteerLib::OptionDictionary & options, SteerLib::EngineInterface * engineInfo )
{
	typedef std::numeric_limits< double > dbl;
	std::cout.precision(dbl::digits10+1);

	_scenarioOptions = &options;

	std::cout << "number of options given to scenario module: " << _scenarioOptions->size() << "\n";

	gEngine = engineInfo;
	gSpatialDatabase = engineInfo->getSpatialDatabase();


	_aiModuleName = "";
	_aiModuleSearchPath = "";
	_aiModule = NULL;
	

	_maxNumFrames = 1000;
	_numScenarios = 0;
	_numGoalStates = 8;
	_numStates = _numGoalStates + 2;	//numGoalStates for each agent that is "on", plus the one state when the agent is "off", plus the one state when the agent is an obstacle

	_radius = 5; // I think this radius is an order higher than the units in the world, i.e. 5 = 0.5 in grid world.
	_randomGeneratorRadius = 0.5f; // default old value
	_gridResX = 10;
	_gridResY = 10;
	_maxAgents = 6;
	_numAgents = -1; // not used if not specified 
	_minAgents = 1;
	_minSpeed = 1;
	_maxSpeed = 2.5;
	_randomSeed = -1;
	_currentNumberOfRandomCalls = 0;

	_useBenchmark = false;
	_benchmarkTechnique = "";

	_benchmarkLog = "";
	_benchmarkLogger = NULL;

	_recordScenarioFile = "" ;
	_recordingScenariosOnly = false ; // added by Petros

	_recFileBaseName = "";
	_recFileFolder = "";

	_checkAgentRelevant = false;
	_checkAgentValid = false;
	_checkObstacleRelevant = false;
	_checkObstacleValid = false;

	_fixedDirection = false;
	_reducedGoals = false; 
	_fixedSpeed = false; 

	// clearing the vector of obstacles and agent information
	_agents.clear();
	_obstacles.clear ();

	std::string logFile, failureLog,runFailureLog; 

	_failureSet.clear ();
	_runFailureSet = false; 

	_frameSaver = new Util::FrameSaver();
	_frameSaver->StartRecord(1024);

	_egocentricFlag = false;

	_dumpFirstFrame = false;
	_dumpFrames = false; 
	_frameBaseName = "frame";
	_frameDumpRelativePath = Empty_String;


	// MUBBASIR MIG VARIABLES ADDED 
	_obstacleState = 1; // default is normal state 
	_passageWidth = 1.8f;

	_agentState = 1; 

	// test case dump 
	_dumpTestCases = false;
	_testCaseRelativePath = "";
	doNotSimulate = false;
	startingNum = 0;
	useContext = false;
	obstacleFreq = .25;

	// for batch running tests
	runScenarioSet = false;
	scenarioSetInitId = 0;

    _checkAgentInteraction = false;
	_checkAgentInteractionRadius = 1.21f;

	_refAgentIntersectPoints.clear();
	_agentIntersectionPoints.clear();
	_agentGoals.clear();
	_agentInitialDisks.clear();
	_agentInitialForwardDirections.clear();

	_agentStaticPathIntersections.clear();

	windowSizeX = 500;
	windowSizeY = 500;

	scenarioFilePrefix = "testcase-";

	_checkStaticPathInteractions = false;
	_interactionInterval = 1;
	_interactionDistance = 2.0f;

	_calculateMaxFrames = false;
	// _scenarioTimeThreshold = std::numeric_limits<float>::max(); Unix good
	_scenarioTimeThreshold = std::numeric_limits<float>::max();
	// std::cout << "scenario time threshold " << _scenarioTimeThreshold << std::endl;

	_maxNodesToExpandForSearch = 500;

	_prettyLog = false;

	_data = "";
	_subspace = NULL;

	char * tmpstr;

	// parse command line options
	SteerLib::OptionDictionary::const_iterator optionIter;
	try
	{
		for (optionIter = options.begin(); optionIter != options.end(); ++optionIter)
		{
#ifdef _DEBUG1
			std::cout << (*optionIter).first << ": " << (*optionIter).second << std::endl;
#endif

			if ((*optionIter).first == "scenarioAI")
			{
				_aiModuleName = (*optionIter).second;
	#ifdef _DEBUG1
	std::cout << " aiModuleName is " << _aiModuleName << " **\n";
	#endif
			}
			else if ((*optionIter).first == "regions") {
				std::string number("(\\d+|\\d+\\.\\d+)");
				std::string region("(" + number + "," + number + "," + number + "," + number + ")");
				std::regex r(region + "(:" + region + ")*");
				std::smatch m;
				bool result = std::regex_match(optionIter->second, m, r);
				if (result) {
					r = region;
					for(std::sregex_iterator i(optionIter->second.begin(), optionIter->second.end(), r), end; i != end; i++) {
						double angle;
						int angleRes;
						double radius;
						int radiusRes;

						angle = atof((*i)[2].str().c_str());
						angleRes = atoi((*i)[3].str().c_str());
						radius = atof((*i)[4].str().c_str());
						radiusRes = atoi((*i)[5].str().c_str());

						#ifdef _DEBUG1
						printf("angle = %f\n", angle);
						printf("angleRes = %d\n", angleRes);
						printf("radius = %f\n", radius);
						printf("radiusRes = %d\n", radiusRes);
						#endif

						// add region
						Region region = {angle, radius, angleRes, radiusRes};
						regions.push_back(region);

						_numScenarios += pow((double)_numStates, angleRes*radiusRes);
					}
				}
				else {
					throw Util::GenericException("poorly formed region specification: \"" + Util::toString((*optionIter).second) + "\".");
				}
			}
			/*
			 * After a little bit of a headache it was discovered the gcc does not
			 * support the std::regex library fully. Mostly only the error codes.
			 */
			else if ((*optionIter).first == "parameters") {
				#ifdef _DEBUG1
				printf("Here 1\n");
				#endif
				// std::string number("(\\d+\\.\\d+)|(\\d+)");
				std::string number("((0-9)+\\.(0-9)+)|(0-9)+");
				#ifdef _DEBUG1
				printf("Here 2\n");
				#endif
				// std::string integer("(\\d+)");
				std::string integer("(0-9)+");
				#ifdef _DEBUG1
				printf("Here 3\n");
				std::regex x(integer);
				std::cout << "Here 3.1\n";
				std::regex v(number);
				std::cout << "Here 3.2\n";
				std::string regex(number + "+" + integer + "+" + integer + "+" + integer + "+" + number + "+" + number + "+" + integer);
				std::cout << "regex = " << regex << "\n";
				#endif
				// This fails
				std::regex r(number + "+" + integer + "+" + integer + "+" + integer + "+" + number + "+" + number + "+" + integer);
				#ifdef _DEBUG1
				// std::regex r(regex);
				printf("Here 4\n");
				#endif
				std::smatch m;
				#ifdef _DEBUG1
				printf("Here 5\n");
				#endif
				bool result = std::regex_match(optionIter->second, m, r);
				#ifdef _DEBUG1
				printf("Here 6\n");
				#endif
				if (result) {
				#ifdef _DEBUG1
				printf("Here 7\n");
				#endif
					_radius = atof(m[1].str().c_str());
					_gridResX = atof(m[2].str().c_str());
					_gridResY = atof(m[3].str().c_str());
					_maxAgents = atof(m[4].str().c_str());
					_minSpeed = atof(m[5].str().c_str());
					_maxSpeed = atof(m[6].str().c_str());
					//_randomSeed = atof(m[7].str().c_str());

					#ifdef _DEBUG1
					std::cout << _radius << std::endl;
					std::cout << _gridResX << std::endl;
					std::cout << _gridResY << std::endl;
					std::cout << _maxAgents << std::endl;
					std::cout << _minSpeed << std::endl;
					std::cout << _maxSpeed << std::endl;
					//std::cout << _randomSeed << std::endl;
					#endif
				}
				else {
					#ifdef _DEBUG1
					printf("Here 8\n");
					#endif
					throw Util::GenericException("poorly formed parameter specification: \"" + Util::toString((*optionIter).second) + "\".");
				}
			}
			else if ((*optionIter).first == "subspace")
			{
				// _subspace = (*optionIter).second;
				loadSubSpace((*optionIter).second);
			}
			else if ((*optionIter).first == "subspaceParams")
			{
				// _subspace = (*optionIter).second;

				std::string params = (*optionIter).second;
				params.erase(std::remove(params.begin(), params.end(), '('), params.end());
				params.erase(std::remove(params.begin(), params.end(), ')'), params.end());
				std::cout << "subspaceParams " << params << std::endl;
				std::vector<std::string> tokens = stringSplit(params, ";");
				for (size_t i=0; i < tokens.size(); i++)
				{
					_subspaceParams.push_back(atof(tokens.at(i).c_str()));
					std::cout << "token " << i << "; " << _subspaceParams.at(i) << std::endl;
				}

			}
			else if ((*optionIter).first == "subspaceWallParams")
			{
				// _subspace = (*optionIter).second;

				std::string params = (*optionIter).second;
				params.erase(std::remove(params.begin(), params.end(), '('), params.end());
				params.erase(std::remove(params.begin(), params.end(), ')'), params.end());
				std::cout << "subspaceWallParams " << params << std::endl;
				std::vector<std::string> tokens = stringSplit(params, ";");
				for (size_t i=0; i < tokens.size(); i++)
				{
					_subspaceWallParams.push_back(atof(tokens.at(i).c_str()));
					std::cout << "token " << i << "; " << _subspaceWallParams.at(i) << std::endl;
				}

			}
			else if ((*optionIter).first == "wallDoorRadius")
			{
				this->wallDoorRadius = atof((*optionIter).second.c_str());
			}
			else if ((*optionIter).first == "prettyLog")
			{
				_prettyLog = true;
			}
			else if ((*optionIter).first == "maxNodesToExpandForSearch")
			{
				_maxNodesToExpandForSearch = atoi((*optionIter).second.c_str());
				// std::cout << "scenario time threshold " << _scenarioTimeThreshold << std::endl;
			}
			else if ((*optionIter).first == "scenarioTimeThreshold")
			{
				_scenarioTimeThreshold = atof((*optionIter).second.c_str());
				std::cout << "scenario time threshold " << _scenarioTimeThreshold << std::endl;
			}
			else if ((*optionIter).first == "calculateMaxFrames")
			{
				_calculateMaxFrames = true;
			}
			else if ((*optionIter).first == "checkStaticPathInteractions")
			{
				_checkStaticPathInteractions = true;
			}
			else if ((*optionIter).first == "interactionInterval")
			{
				_interactionInterval = strtol((*optionIter).second.c_str(), NULL, 10);
			}
			else if ((*optionIter).first == "interactionDistance")
			{
				std::cout << "Setting interactionDistance: " << (*optionIter).second << std::endl;
				_interactionDistance = strtod((*optionIter).second.c_str(), NULL);
			}
			else if ((*optionIter).first == "windowSizeX")
			{
				windowSizeX = atoi((*optionIter).second.c_str());
				std::cout << "Setting X window size: " << windowSizeX << std::endl;
			}
			else if ((*optionIter).first == "windowSizeY")
			{
				windowSizeY = atoi((*optionIter).second.c_str());
			}
			else if ((*optionIter).first == "maxFrames")
			{
				// Set max number of frame per scenario
				_maxNumFrames = atoi((*optionIter).second.c_str());
			}
			else if ((*optionIter).first == "checkAgentInteraction")
			{
				// If intersection is being forced then so must relevency.
			    _checkAgentInteraction = true;
			    _checkAgentRelevant = true;
			}
			else if ((*optionIter).first == "checkAgentInteractionRadius")
			{
				_checkAgentInteractionRadius = atof((*optionIter).second.c_str());
			}
			else if ((*optionIter).first == "agentRadius")
			{
			    _radius = atof((*optionIter).second.c_str());
			}
			else if ((*optionIter).first == "scenarioSetPath")
			{
				runScenarioSet = true;
				scenarioSetPath = (*optionIter).second;
			}
			else if ((*optionIter).first == "scenarioSetInitId")
			{
				scenarioSetInitId = atoi((*optionIter).second.c_str());
			}
			else if ((*optionIter).first == "gridResolutionX")
			{
				_gridResX = atoi((*optionIter).second.c_str());
			}
			else if ((*optionIter).first == "gridResolutionY")
			{
				_gridResY = atoi((*optionIter).second.c_str());
			}
			else if ((*optionIter).first == "maxNumAgents")
			{
				_maxAgents = atoi((*optionIter).second.c_str());
			}
			else if ((*optionIter).first == "agentMinSpeed")
			{
				_minSpeed = atof((*optionIter).second.c_str());
			}
			else if ((*optionIter).first == "randomGeneratorRadius")
			{
				_randomGeneratorRadius = atof((*optionIter).second.c_str());
			}
			else if ((*optionIter).first == "agentMaxSpeed")
			{
			    _maxSpeed = atof((*optionIter).second.c_str());
			}
			else if ((*optionIter).first == "benchmarkTechnique") {
				_benchmarkTechnique = (*optionIter).second.c_str();
			}
			else if ((*optionIter).first == "useBenchmark") {
				_useBenchmark = true;
			}
			else if ((*optionIter).first == "benchmarkLog")
			{
				_benchmarkLog = (*optionIter).second.c_str();
				std::cout << "=====Forcing benchmark" << _benchmarkLog << std::endl;
/*
#ifdef _DEBUG1
		std::cout << "Forcing benchmark log to be log/test.log\n";
		_benchmarkLog = "log/test.log";
#endif
 */
			}
			else if ((*optionIter).first == "randomSeed") {
				_randomSeed = atoi ( (*optionIter).second.c_str() );
			}
			else if ((*optionIter).first == "randomCalls") {
				_currentNumberOfRandomCalls = atoi ( (*optionIter).second.c_str() );
			}
			else if ((*optionIter).first == "recFile") {
				_recFileBaseName =  (*optionIter).second;
			}
			else if ((*optionIter).first == "recFileFolder") {
				_recFileFolder =  (*optionIter).second;
			}
			else if ((*optionIter).first == "fileToAppendScenarios") {
				 _recordScenarioFile = (*optionIter).second;
				#ifdef _DEBUG1
				std::cout << _recordScenarioFile << "\n" ;
				#endif
				_recordingScenariosOnly = true ;
			}
			else if ((*optionIter).first == "checkAgentValid") {
				_checkAgentValid = true;
			}
			else if ((*optionIter).first == "checkAgentRelevant") {
				_checkAgentRelevant = true;
			}
			else if ((*optionIter).first == "checkObstacleValid") {
				_checkObstacleValid = true;
			}
			else if ((*optionIter).first == "checkObstacleRelevant") {
				_checkObstacleRelevant = true;
			}
			else if ((*optionIter).first == "fixedDirection") {
				_fixedDirection = true;
			}
			else if ((*optionIter).first == "reducedGoals") {
				_reducedGoals = true;
			}
			else if ((*optionIter).first == "fixedSpeed") {
				_fixedSpeed = true;
			}
			else if ((*optionIter).first == "minAgents") {
				_minAgents = atoi ( (*optionIter).second.c_str() );

			}
			else if ((*optionIter).first == "maxAgents") {
				_maxAgents = atoi ( (*optionIter).second.c_str() );

			}
			else if ((*optionIter).first == "numAgents") {
				_numAgents = atoi ( (*optionIter).second.c_str() );

			}
			else if ((*optionIter).first == "grid") {
				_gridResX = _gridResY = atoi ( (*optionIter).second.c_str() );

			}
			else if ((*optionIter).first == "radius") {
				_radius = atoi ( (*optionIter).second.c_str() );

			}
			else if ((*optionIter).first == "logFile") {
				logFile =  (*optionIter).second;

			}
			else if ((*optionIter).first == "failureLog") {
				failureLog =  (*optionIter).second;

			}
			else if ((*optionIter).first == "runFailureLog") {
				runFailureLog =  (*optionIter).second;
				_runFailureSet = true;
			}
			else if ((*optionIter).first == "egocentric") {
				_egocentricFlag = true;
			}
			else if ((*optionIter).first == "dumpFrames") {
				_dumpFrames = true;
			}
			else if ((*optionIter).first == "dumpFirstFrame") {
				_dumpFirstFrame = true;
			}
			else if ((*optionIter).first == "frameName") {
				_frameBaseName = (*optionIter).second;
			}
			else if ((*optionIter).first == "framePath")
			{
				// This should not create this directory
				_frameDumpRelativePath = (*optionIter).second;
			}
			else if ((*optionIter).first == "obstacleState") {
				_obstacleState = atoi ( (*optionIter).second.c_str() );

			}
			else if ((*optionIter).first == "agentState") {
				_agentState = atoi ( (*optionIter).second.c_str() );

			}
			else if ((*optionIter).first == "passageWidth") {
				_passageWidth = atof ( (*optionIter).second.c_str() );

			}
			else if ((*optionIter).first == "dumpTestCases") {
				_dumpTestCases = true;
			}
			//CORY -- ADDED COMMAND LINE ARGUMENT FOR CASE NUMBER
			else if ((*optionIter).first == "startingNum") {
				startingNum = atoi ( (*optionIter).second.c_str() );
			}
			else if ((*optionIter).first == "testCasePath") {
				_testCaseRelativePath =  (*optionIter).second.c_str();
			}
			else if ((*optionIter).first == "contextID") {
				contextNum = atoi((*optionIter).second.c_str());
				useContext = true;
			}
			else if ((*optionIter).first == "numScenarios") {
				_numScenarios = atoi((*optionIter).second.c_str());
				std::cout << "Number of scenarios: " << _numScenarios << std::endl;
			}
			else if ((*optionIter).first == "obstacleFrequency") {
				obstacleFreq = atof((*optionIter).second.c_str());
			}
			//RECORD NEEDS TO BE PROCESSED
			else
			{
				// Glen - I'll leave this for now but it would be better if
				// options were passed to all others modules.
				// throw Util::GenericException("unrecognized option \"" + Util::toString((*optionIter).first) + "\" given to scenario module.");
				// std::cerr << "option: " << (*optionIter).first << " not recognized by " <<
					// 	"scenario module" <<  std::endl;
			}
		}
	}
	catch  (std::regex_error& e)
	{
		if (e.code() == std::regex_constants::error_ctype)
		{
			std::cerr << "The expression contained an invalid character class name\n";
		}
		else if ( e.code() == std::regex_constants::error_escape )
		{
			std::cerr << "The expression contained an invalid escaped character, or a trailing escape.\n";
		}
		else if ( e.code() == std::regex_constants::error_backref )
		{
			std::cerr << "The expression contained an invalid back reference.\n";
		}
		else if ( e.code() == std::regex_constants::error_brack )
		{
			std::cerr << "The expression contained mismatched brackets ([ and ]).\n";
		}
		else
		{
			std::cerr << "Some other regex exception happened." << e.code() << "****** \n";
		}
		exit(0);
	}
	//exit(1);
	// specifiying default ai 

	//override the _min and _max agent numbers
	if(useContext)
	{
		switch((contextNum >= 12) ? contextNum - 12 : contextNum)
		{
		case 0:
			//no special circumstances
			_minAgents = 1;
			_maxAgents = 4;
			break;
		case 1:
			//light oncoming
		case 6:
			//light crossing
			_minAgents = 2;
			_maxAgents = 5;
			break;

		case 2:
			//medium oncoming
		case 7:
			//medium crossing
			_minAgents = 4;
			_maxAgents = 8;
			break;

		case 3:
			//heavy oncoming
		case 5:
			//winning-side oncoming
		case 8:
			//heavy crossing
		case 10:
			//winning-side crossing
		case 4:
			//matched oncoming; note that while small, each unit will be 2 agents to enforce the even sides
		case 9:
			//matched crossing
		case 11:
			//chaos
			_minAgents = 7;
			_maxAgents = 11;
			break;


			_minAgents = 3;
			_maxAgents = 6;
			break;
		default:
			throw new Util::GenericException("Contexts " + std::string("" + contextNum) + " is not yet implemented");
			break;
		}
	}

	if ( _aiModuleName.empty() )
	{
#ifdef _DEBUG1
std::cout << " Renaming aiModuleName to make Glen crazy*\n";
#endif
		_aiModuleName = "simpleAI";
		doNotSimulate = true; //implementing simulation cut-off is on the to-do list...may require a new AI module that simply teleports the agent to the goal...
	}
	else {
		// loading ai module 
		loadAIModule ();
	}

	// loading steerBench module 
	if ( _useBenchmark )
	{
		loadBenchmarkModule ();

		if ( ! _benchmarkTechnique.empty())
			_benchmarkModule->setBenchmarkTechnique ( _benchmarkTechnique, options, engineInfo );
		else
			_benchmarkTechnique = "composite02"; // this is the default technique loaded by SteerBench.
	}

	
	// initialize regions to default specification if none given:
	/*
	if(regions.size() == 0) {
		Region region1 = {50, 10, 3, 3};
		regions.push_back(region1);
		Region region2 = {50, 10, 2, 2};
		regions.push_back(region2);
		Region region3 = {90, 10, 2, 1};
		regions.push_back(region3);
		_numScenarios = 1000;
	}
	*/

	if ( _runFailureSet == true )
	{
		readFailureSet (runFailureLog);
		_currentFailureSetIndex = 0;

		#ifdef _DEBUG1
		std::cout << " failure set size " << _failureSet.size (); 
		#endif

		_numScenarios = _failureSet.size();
		long long numberOfRandomCallsToMake = _failureSet[_currentFailureSetIndex];

		setRandomNumberGenerator(_randomSeed,0); 
		incrementRandomNumberGenerator(numberOfRandomCallsToMake);
	}
	else if(_randomSeed == -1)
	{	//no seed provided, generate one, this was previously done wrong and used "clock" instead of "time"
#ifdef _WIN32
		time_t currentTime;
#else
		long currentTime;
#endif
		time(&currentTime);
		_randomSeed = static_cast<long>(currentTime);
		setRandomNumberGenerator(_randomSeed,_currentNumberOfRandomCalls);
	}
	else if ( _subspace != NULL )
	{
		setRandomNumberGenerator(10, _currentNumberOfRandomCalls);
	}
	else
	{
		setRandomNumberGenerator(_randomSeed,_currentNumberOfRandomCalls);
	}


	// checking to see if logFile and failureLog are specified 
	if ( logFile.empty() == false)
	{
		std::set<long long> failureSet;
		//generateFailureSet (logFile,failureSet);
		generateFailureSet (logFile,failureSet,2);

		if ( failureLog.empty() == false)
			writeFailureSet(failureLog,failureSet);

		exit(0);
	}



	/// loading initial scenario 
	_currentScenario = 0;

/*
	if ( _currentScenario < _numScenarios )	//this is impossible unless -1 passed from the command line...was this for DEBUG1ging?
	{
		if( runScenarioSet )
		{
			loadScenario(scenarioSetPath, _currentScenario);
		}
		else if (_subspace != NULL)
		{
			generateNextSubSpace();
		}
		else
		{
			generateScenario(_currentScenario);
		}
		//loadCurrentScenario ();
		gEngine->getPathPlanner()->refresh();
	}
*/


	_logManager = LogManager::getInstance();

#ifdef _DEBUG1
	std::cout << "_useBenchmark = " << _useBenchmark << ",  ! _benmarkLog.empty() = " << !_benchmarkLog.empty() << "\n";
#endif

	if ( _useBenchmark )
	{
		// creating benchmark logger 
		_benchmarkLogger = LogManager::getInstance()->createLogger(_benchmarkLog,LoggerType::BASIC_WRITE);
		_benchmarkLogger->addDataField("scenario_id",DataType::Integer);
		_benchmarkLogger->addDataField("frames", DataType::Integer);
		_benchmarkLogger->addDataField("rand_calls",DataType::LongLong);

		// total simulation metrics 
		_benchmarkLogger->addDataField("collisions", DataType::Float);
		_benchmarkLogger->addDataField("time", DataType::Float);
		_benchmarkLogger->addDataField("effort", DataType::Float);

		if (_benchmarkTechnique == "composite02" || _benchmarkTechnique == "compositePLE" ||
				_benchmarkTechnique == "compositeGraph" || _benchmarkTechnique == "compositeEntropy" ) // right now both are identical
		{
			std::cout << "***********************Adding acceleration " << std::endl;
			_benchmarkLogger->addDataField("acceleration", DataType::Float);
		}

		_benchmarkLogger->addDataField("score",DataType::Float);

		// did the scenario complete or not ? 
		_benchmarkLogger->addDataField("success",DataType::Integer);

		// egocentric agent metrics 
		//_benchmarkLogger->addDataField("NumCollisions", DataType::Float);
		_benchmarkLogger->addDataField("num_unique_collisions", DataType::Float);
		_benchmarkLogger->addDataField("total_time_enabled", DataType::Float);
		_benchmarkLogger->addDataField("total_acceleration", DataType::Float);
		_benchmarkLogger->addDataField("total_distance_traveled", DataType::Float);
		_benchmarkLogger->addDataField("total_change_in_speed", DataType::Float);
		_benchmarkLogger->addDataField("total_degrees_turned", DataType::Float);
		_benchmarkLogger->addDataField("sum_total_of_instantaneous_acceleration", DataType::Float);
		_benchmarkLogger->addDataField("sum_total_of_instantaneous_kinetic_energies", DataType::Float);
		_benchmarkLogger->addDataField("average_kinetic_energy", DataType::Float);

		// this gets added to agent scores for compositePLE 
		if ( _benchmarkTechnique == "compositePLE") 
		{
			_benchmarkLogger->addDataField("ple_energy_dummy", DataType::Float);
			_benchmarkLogger->addDataField("penetration_penalty", DataType::Float);
		}

		_benchmarkLogger->addDataField("agent_complete", DataType::Integer);
		_benchmarkLogger->addDataField("agent_success", DataType::Integer);

		_benchmarkLogger->addDataField("optimal_path_length", DataType::Float);
		_benchmarkLogger->addDataField("length_ratio", DataType::Float);

		_benchmarkLogger->addDataField("optimal_time", DataType::Float);
		_benchmarkLogger->addDataField("time_ratio", DataType::Float);

		_benchmarkLogger->addDataField("agent_path_lengths", DataType::String);
		_benchmarkLogger->addDataField("agent_time_enableds", DataType::String);
		_benchmarkLogger->addDataField("agent_distance_traveleds", DataType::String);
		_benchmarkLogger->addDataField("agent_completes", DataType::String);
		_benchmarkLogger->addDataField("agent_successes", DataType::String);

		if ( _benchmarkTechnique == "compositeGraph")
		{
			_benchmarkLogger->addDataField("agent_intersections", DataType::String);
			_benchmarkLogger->addDataField("ref_agent_intersections", DataType::String);
			_benchmarkLogger->addDataField("box_obstacles", DataType::String);
			_benchmarkLogger->addDataField("agent_goals", DataType::String);
			_benchmarkLogger->addDataField("agent_initial_disks", DataType::String);
			_benchmarkLogger->addDataField("agent_initial_forward_direction", DataType::String);
			_benchmarkLogger->addDataField("agent_static_paths", DataType::String);
			_benchmarkLogger->addDataField("agent_static_path_intersection_points", DataType::String);
			_benchmarkLogger->addDataField("ref_agent_static_path_intersection_points", DataType::String);

		}
		//_benchmarkLogger->addDataField("optimalKE", DataType::Float);
		//_benchmarkLogger->addDataField("KERatio", DataType::Float);

		// adding new metrics for compositePLE
		if ( _benchmarkTechnique == "compositePLE") 
		{
			_benchmarkLogger->addDataField("ple_energy", DataType::Float);
			_benchmarkLogger->addDataField("ple_energy_optimal", DataType::Float);
			_benchmarkLogger->addDataField("ple_energy_ratio", DataType::Float);
			_benchmarkLogger->addDataField("agent_ple_energys", DataType::String);
		}

#ifdef _DEBUG1
	std::cout << "Writing data to log file**************************************************" << _benchmarkLog << "\n";
	std::cout << "Random seed = " << _randomSeed << "\n";
#endif
		// todo -- do we need -- did the egocentric agent complete or not? 

		//_benchmarkLogger->writeMetaData();
		if ( ! _benchmarkLog.empty() )
		{
			_benchmarkLogger->writeData(_randomSeed);


			// LETS TRY TO WRITE THE LABELS OF EACH FIELD
			std::stringstream labelStream;
			unsigned int i;
			for (i=0; i < _benchmarkLogger->getNumberOfFields() - 1; i++)
				labelStream << _benchmarkLogger->getFieldName(i) << " ";
			labelStream << _benchmarkLogger->getFieldName(i);
			_data = labelStream.str();

			_benchmarkLogger->writeData(labelStream.str());
		}


	}

	if ( _recFileBaseName != "")
	{
		loadSimulationRecorderModule ();

		std::stringstream st1;
		if ( _recFileFolder != "" )
		{
			st1 << _recFileFolder << "/";
		}

		st1 << _recFileBaseName;
		st1 << ((int)_currentScenario + (int)scenarioSetInitId);
		st1 << ".rec";
		std::string tempS = st1.str();
		_recorderModule->initializeRecorder(tempS);
	}

	if ( _dumpFrames )
	{
		// dumping frames;
		_frameSaver->baseName = _frameBaseName;
		_frameSaver->filePath = _frameDumpRelativePath;
		_frameSaver->m_record = 1; // recording animation (postfix with frame number)
	}
	if ( _dumpFirstFrame )
	{
		// dumping first frame of every scenario
		_frameSaver->baseName = _frameBaseName;
		_frameSaver->filePath = _frameDumpRelativePath;
		_frameSaver->m_record = 0; // recording just one frame per scenario (no postfix)
	}

}

void destroyModule( SteerLib::ModuleInterface*  module )
{
	if (module) delete module;
}

void ScenarioModule::saveGraphData()
{


	if ( _agents.size() > 1 && _checkAgentInteraction)
	{
		// This section adds the direct path intersections with the reference agent.
		Util::Point intersectionPoint2 = Util::Point(0,0,0);

		bool agentIntersects = Util::intersect2Lines2D(_agents.at(_agents.size()-1).position,
				_agents.at(_agents.size()-1).position +
				((_agents.at(_agents.size()-1).goals.at(_agents.at(_agents.size()-1).goals.size()-1).targetLocation -
						_agents.at(_agents.size()-1).position)),
				_agents.at(0).position, _agents.at(0).position +
				(_agents.at(0).goals.at(_agents.at(0).goals.size()-1).targetLocation -
						_agents.at(0).position),
				intersectionPoint2);
		if ( agentIntersects )
		{
			_refAgentIntersectPoints.push_back(intersectionPoint2);
		}
	}

	// Calculate wrt to the static paths, likely points of interaction
	if ( agentPaths.size() > 1 && _checkStaticPathInteractions)
	{
		// Computes intersection with just reference agent
		std::vector<Util::Point> * _intersectionPoints = new std::vector<Util::Point>();
		std::vector<Util::Point> * _refIntersectionPoints = new std::vector<Util::Point>();
		_intersectionPoints->clear();
		_refIntersectionPoints->clear();

		calcInteractionPoints( agentPaths.at(0), agentPaths.at(agentPaths.size()-1),
				_interactionDistance, _interactionInterval, _intersectionPoints);
		_refAgentStaticPathIntersections.insert(_refAgentStaticPathIntersections.end(),
				_intersectionPoints->begin(), _intersectionPoints->end());

		// Computes intersections between all agents.
		for (int i = 0; i < agentPaths.size() - 1; i++ )
		{
			calcInteractionPoints( agentPaths.at(i), agentPaths.at(agentPaths.size()-1),
					_interactionDistance, _interactionInterval, _refIntersectionPoints);
		}
		_agentStaticPathIntersections.insert(_agentStaticPathIntersections.end(),
							_refIntersectionPoints->begin(), _refIntersectionPoints->end());
	}
}

void ScenarioModule::clearGraphData()
{
	_agentStaticPathIntersections.clear();
	_refAgentStaticPathIntersections.clear();
}

void ScenarioModule::loadSubSpace(std::string subSpacePath)
{
	SteerLib::TestCaseReader * testCaseReader = new SteerLib::TestCaseReader();
	testCaseReader->readTestCaseFromFile(subSpacePath);
	// loadSubSpace(*testCaseReader);
	this->_subspace = testCaseReader;
}


void ScenarioModule::generateNextSubSpace()
{
	// clearScenario();
	//Create the obstacles
	const SteerLib::TestCaseReader & testCaseReader = *(this->_subspace);
	for (unsigned int i=0; i < testCaseReader.getNumObstacles(); i++)
	{
		// const SteerLib::ObstacleInitialConditions * ic = testCaseReader->getObstacleInitialConditions(i);
		if (typeid(*testCaseReader.getObstacleInitialConditions(i)) == typeid(SteerLib::BoxObstacleInitialConditions))
		{
			SteerLib::BoxObstacleInitialConditions *bic = const_cast<SteerLib::BoxObstacleInitialConditions*>(dynamic_cast<const SteerLib::BoxObstacleInitialConditions*>(testCaseReader.getObstacleInitialConditions(i))); /// TODO: Create support for all types of obstacles in scenarios
			//SteerLib::BoxObstacle  b(bic->xmin, bic->xmax, bic->ymin, bic->ymax, bic->zmin, bic->zmax);
			// b = new SteerLib::BoxObstacle(ic.xmin, ic.xmax, ic.ymin, ic.ymax, ic.zmin, ic.zmax);
			SteerLib::BoxObstacle *b = dynamic_cast<SteerLib::BoxObstacle*>(bic->createObstacle());
			_obstacles.push_back(bic);
			delete b;
		}
		else if (typeid(*testCaseReader.getObstacleInitialConditions(i)) == typeid(SteerLib::CircleObstacleInitialConditions))
		{
			SteerLib::CircleObstacleInitialConditions *cic = const_cast<SteerLib::CircleObstacleInitialConditions*>(dynamic_cast<const SteerLib::CircleObstacleInitialConditions*>(testCaseReader.getObstacleInitialConditions(i))); /// TODO: Create support for all types of obstacles in scenarios
			//SteerLib::BoxObstacle  b(bic->xmin, bic->xmax, bic->ymin, bic->ymax, bic->zmin, bic->zmax);
			// b = new SteerLib::BoxObstacle(ic.xmin, ic.xmax, ic.ymin, ic.ymax, ic.zmin, ic.zmax);
			// SteerLib::CircleObstacle *b = dynamic_cast<SteerLib::CircleObstacle*>(cic->createObstacle());
			_obstacles.push_back(cic);
			// delete b;
		}
		else if (typeid(*testCaseReader.getObstacleInitialConditions(i)) == typeid(SteerLib::OrientedBoxObstacle))
		{
			SteerLib::OrientedBoxObstacleInitialConditions *cic = const_cast<SteerLib::OrientedBoxObstacleInitialConditions*>(dynamic_cast<const SteerLib::OrientedBoxObstacleInitialConditions*>(testCaseReader.getObstacleInitialConditions(i))); /// TODO: Create support for all types of obstacles in scenarios
			//SteerLib::BoxObstacle  b(bic->xmin, bic->xmax, bic->ymin, bic->ymax, bic->zmin, bic->zmax);
			// b = new SteerLib::BoxObstacle(ic.xmin, ic.xmax, ic.ymin, ic.ymax, ic.zmin, ic.zmax);
			// SteerLib::CircleObstacle *b = dynamic_cast<SteerLib::CircleObstacle*>(cic->createObstacle());
			_obstacles.push_back(cic);
			// delete b;
		}
		else if (typeid(*testCaseReader.getObstacleInitialConditions(i)) == typeid(SteerLib::OrientedWallObstacle))
		{
			const SteerLib::OrientedWallObstacleInitialConditions * oroic = dynamic_cast<const SteerLib::OrientedWallObstacleInitialConditions*>(testCaseReader.getObstacleInitialConditions(i));
			// if ( oroic->)

			SteerLib::OrientedWallObstacleInitialConditions *cic = const_cast<SteerLib::OrientedWallObstacleInitialConditions*>(oroic); /// TODO: Create support for all types of obstacles in scenarios

			_obstacles.push_back(cic);
			// delete b;
		}
		// _obstacles.push_back(*ic);
	}

	for (size_t i = 0; i < _subspaceParams.size(); i+=2)
	{
		// TODO create circle obstacle
		// radius = 0.5
		SteerLib::CircleObstacleInitialConditions * cic = new SteerLib::CircleObstacleInitialConditions();
		cic->position = Util::Point(_subspaceParams.at(i), 0.0, _subspaceParams.at(i+1));
		cic->radius = 0.2286f;
		cic->height = 1.0f;
		_obstacles.push_back(cic);

	}
	for (size_t i = 0; i < _subspaceWallParams.size(); i+=2)
	{
		// TODO create circle obstacle
		// radius = 0.5
		double radius = 0.2286f;
		Util::Point t_position = Util::Point(_subspaceWallParams.at(i), 0.0, _subspaceWallParams.at(i+1));
		// std::cout << "Box point: " << t_position << std::endl;
		SteerLib::BoxObstacleInitialConditions * bic = new SteerLib::BoxObstacleInitialConditions( t_position.x - radius, t_position.x + radius,
				0.0, 1.0, t_position.z - radius, t_position.z + radius);
		// bic->xmax = t_position.x + radius;
		// std::cout << "box obstacle is: " << *bic << std::endl;
		_obstacles.push_back(bic);

	}

	// The pointers that need to be given to the spacial database should be from the _obstacles list
	for (unsigned int i=0; i < _obstacles.size(); i++)
	{
		SteerLib::ObstacleInterface* obs = (_obstacles[i])->createObstacle();
		// if (typeid(*_obstacles[i]) == typeid(SteerLib::OrientedWallObstacle))
		// {
		// 	SteerLib::OrientedWallObstacle * owo = dynamic_cast<SteerLib::OrientedWallObstacle *>(_obstacles[i]);
		//	if ( owo->)
		// }
		gEngine->addObstacle(obs);
		gEngine->getSpatialDatabase()->addObject( obs, obs->getBounds());
	}
	gEngine->getPathPlanner()->refresh();

	//Create the agents
	for (unsigned int i=0; i < testCaseReader.getNumAgents(); i++)
	{
		// std::cout << "working on agent " << i << std::endl;
		SteerLib::AgentInitialConditions ic = testCaseReader.getAgentInitialConditions(i);
		if (ic.fromRandom)
		{
			ic.position = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(ic.randBox, ic.radius, false, *randGen);
		}
		AgentInterface * agent = gEngine->createAgent( ic, _aiModule );
		agent->reset(ic, gEngine);
		std::vector<Util::Point> agentPath;
		ic.name = std::string("agent") + std::to_string(i);
		_agents.push_back(ic);
		isAgentValid( (SteerLib::AgentInitialConditions&) ic, agentPath);
		SteerLib::AgentInitialConditions & ac = (SteerLib::AgentInitialConditions&) ic;
		agentPath.clear ();
		bool pathComplete = gEngine->getPathPlanner()->findSmoothPath(ac.position,
				ac.goals[0].targetLocation,agentPath, _maxNodesToExpandForSearch);
		agentPaths.push_back(agentPath);

		_agentGoals.push_back(ic.goals.at(0).targetLocation);
		_agentInitialDisks.push_back(Util::Circle(ic.position.x, ic.position.z, ic.radius));
		_agentInitialForwardDirections.push_back(Util::normalize(ic.direction));
		saveGraphData();
	}
	if ( _dumpTestCases )
	{
		this->saveScenario();
	}
}

void ScenarioModule::loadScenario(SteerLib::TestCaseReader * testCaseReader)
{
	//Create the obstacles
	for (unsigned int i=0; i < testCaseReader->getNumObstacles(); i++)
	{
		// const SteerLib::ObstacleInitialConditions * ic = testCaseReader->getObstacleInitialConditions(i);
		if (typeid(*testCaseReader->getObstacleInitialConditions(i)) == typeid(SteerLib::BoxObstacleInitialConditions))
		{
			SteerLib::BoxObstacleInitialConditions *bic = const_cast<SteerLib::BoxObstacleInitialConditions*>(dynamic_cast<const SteerLib::BoxObstacleInitialConditions*>(testCaseReader->getObstacleInitialConditions(i))); /// TODO: Create support for all types of obstacles in scenarios
			//SteerLib::BoxObstacle  b(bic->xmin, bic->xmax, bic->ymin, bic->ymax, bic->zmin, bic->zmax);
			// b = new SteerLib::BoxObstacle(ic.xmin, ic.xmax, ic.ymin, ic.ymax, ic.zmin, ic.zmax);
			SteerLib::BoxObstacle *b = dynamic_cast<SteerLib::BoxObstacle*>(bic->createObstacle());
			_obstacles.push_back(bic);
			delete b;
		}
		else if (typeid(*testCaseReader->getObstacleInitialConditions(i)) == typeid(SteerLib::CircleObstacleInitialConditions))
		{
			SteerLib::CircleObstacleInitialConditions *cic = const_cast<SteerLib::CircleObstacleInitialConditions*>(dynamic_cast<const SteerLib::CircleObstacleInitialConditions*>(testCaseReader->getObstacleInitialConditions(i))); /// TODO: Create support for all types of obstacles in scenarios
			//SteerLib::BoxObstacle  b(bic->xmin, bic->xmax, bic->ymin, bic->ymax, bic->zmin, bic->zmax);
			// b = new SteerLib::BoxObstacle(ic.xmin, ic.xmax, ic.ymin, ic.ymax, ic.zmin, ic.zmax);
			SteerLib::CircleObstacle *b = dynamic_cast<SteerLib::CircleObstacle*>(cic->createObstacle());
			_obstacles.push_back(cic);
			delete b;
		}
		// _obstacles.push_back(*ic);
	}
	// The pointers that need to be given to the spacial database should be from the _obstacles list
	for (unsigned int i=0; i < _obstacles.size(); i++)
	{
		SteerLib::ObstacleInterface* obs = (_obstacles[i])->createObstacle();
		gEngine->addObstacle(obs);
		gEngine->getSpatialDatabase()->addObject( obs, obs->getBounds());
	}
	gEngine->getPathPlanner()->refresh();
	//Create the agents
	for (unsigned int i=0; i < testCaseReader->getNumAgents(); i++)
	{
		// std::cout << "working on agent " << i << std::endl;
		const SteerLib::AgentInitialConditions ic = testCaseReader->getAgentInitialConditions(i);
		AgentInterface * agent = gEngine->createAgent( ic, _aiModule );
		agent->reset(ic, gEngine);
		std::vector<Util::Point> agentPath;
		_agents.push_back(ic);
		isAgentValid( (SteerLib::AgentInitialConditions&) ic, agentPath);
		SteerLib::AgentInitialConditions & ac = (SteerLib::AgentInitialConditions&) ic;
		agentPath.clear ();
		bool pathComplete = gEngine->getPathPlanner()->findSmoothPath(ac.position,
				ac.goals[0].targetLocation,agentPath, _maxNodesToExpandForSearch);
		agentPaths.push_back(agentPath);

		_agentGoals.push_back(ic.goals.at(0).targetLocation);
		_agentInitialDisks.push_back(Util::Circle(ic.position.x, ic.position.z, ic.radius));
		_agentInitialForwardDirections.push_back(Util::normalize(ic.direction));
		saveGraphData();
	}

	if ( _calculateMaxFrames )
	{
		float framesPerSec = 20;
		float velocity = 1.25f; // m/20 frames
		float metersPerFrame = velocity/framesPerSec;
		Util::AxisAlignedBox worldBounds = testCaseReader->getWorldBounds();

		float width = worldBounds.xmax - worldBounds.xmin;
		float height = worldBounds.zmax - worldBounds.zmin;
		float diagonalx2 = sqrt((width*width) + (height*height)) * 2;
		_maxNumFrames = (int) diagonalx2 / metersPerFrame;
#ifdef _DEBUG1
		std::cout << "The diagonal x 2 is: " << diagonalx2 << std::endl;
		std::cout << "The maximum number of frames is: " << _maxNumFrames << std::endl;
#endif
	}

}

void ScenarioModule::loadScenario(std::string scenarioPath, unsigned int currentScenarioNum)
{
	agentPaths.clear();
	// _currentNumberOfRandomCalls = randGen->getNumberOfRandCalls ();
	// randGen->
	/*
	std::string scenarioName;
	scenarioName.append("scenario");
	scenarioName.append(itoa(currentScenarioNum));
	scenarioName.append(".xml");
	 */
	std::stringstream scenarioName;
	// scenarioName << "testcase-" << currentScenarioNum << ".xml";
	SteerLib::TestCaseReader * testCaseReader = new SteerLib::TestCaseReader();
#ifdef _WIN32
	scenarioName << scenarioPath << "\\" << scenarioFilePrefix << currentScenarioNum + scenarioSetInitId << ".xml";
#else
	scenarioName << scenarioPath << "/" << scenarioFilePrefix << currentScenarioNum + scenarioSetInitId << ".xml";
#endif

	testCaseReader->readTestCaseFromFile(scenarioName.str());
	loadScenario(testCaseReader);

	if (_egocentricFlag )
	{
		for (unsigned int i=0; i < gEngine->getAgents().size(); i++)
			gEngine->unselectAgent(gEngine->getAgents()[i]);

		gEngine->selectAgent(gEngine->getAgents()[0]);
	}

	if (_dumpFrames)
	{
		int e;
		std::stringstream out;
		if ( strcmp(_frameDumpRelativePath.c_str(), Empty_String) == 0) // no relitive path
		{
			out << "scenario" << (_currentScenario + scenarioSetInitId);
		}
		else
		{
			out << _frameDumpRelativePath;
			out << "/scenario" << (_currentScenario + scenarioSetInitId);
		}
#ifdef _WIN32
				// Code to create a directory on Windows
		e = CreateDirectory(out.str().c_str(), NULL);
		if (e == 0)
		{
			// printf("mkdir failed; errno=%d\n",errno);
			perror("CreateDirectory failed for creating framePath");
		}
		else
		{
			printf("created the directory %s\n",_frameDumpRelativePath.c_str());
		}
#else
		e = mkdir(out.str().c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if (e != 0)
		{
			// printf("mkdir failed; errno=%d\n",errno);
			perror("mkdir failed for creating scenario path");
		}
		else
		{
			printf("created the directory %s\n",out.str().c_str());
		}
#endif
	}

	if (_checkAgentRelevant )
	{
		// computes all agent path intersections
		Util::Point intersectPoint;
		bool agentsIntersect = false;
		for ( int g = 0; g < _agents.size(); g++)
		{
			// std::cout << _agents.at(g).name << " position is " << _agents.at(g).position << " for all intersections" << std::endl;
			for (int t = g + 1; t < _agents.size(); t++ )
			{
				agentsIntersect = Util::intersect2Lines2D(_agents.at(g).position, _agents.at(g).position +
												((_agents.at(g).goals.at(0).targetLocation - _agents.at(g).position)),
												_agents.at(t).position, _agents.at(t).position +
												(_agents.at(t).goals.at(0).targetLocation - _agents.at(t).position),
												intersectPoint);
				if ( agentsIntersect )
				{
					_agentIntersectionPoints.push_back(intersectPoint);
				}
			}
		}

		for (int t = 1; t < _agents.size(); t++ )
		{
			agentsIntersect = Util::intersect2Lines2D(_agents.at(0).position, _agents.at(0).position +
											((_agents.at(0).goals.at(0).targetLocation - _agents.at(0).position)),
											_agents.at(t).position, _agents.at(t).position +
											(_agents.at(t).goals.at(0).targetLocation - _agents.at(t).position),
											intersectPoint);
			if ( agentsIntersect )
			{
				_agentIntersectionPoints.push_back(intersectPoint);
			}
		}

		std::stringstream intersectionPoints;
		intersectionPoints << "(";
		for (int prrrr = 0; prrrr < _agentIntersectionPoints.size(); prrrr++)
		{
			intersectionPoints << "(" << _agentIntersectionPoints.at(prrrr).x <<
					"," << _agentIntersectionPoints.at(prrrr).z << ")";

			if ( !(prrrr == (_agentIntersectionPoints.size() - 1)) )
			{
				intersectionPoints << ",";
			}

		}
		intersectionPoints << ")";
#ifdef _DEBUG1
		std::cout << "Intersection points: " << intersectionPoints.str() << std::endl;
#endif
	}

	delete testCaseReader;
}

bool ScenarioModule::isAgentValid(SteerLib::AgentInitialConditions &ac, std::vector<Util::Point> &path )
{
	//if ( Util::distanceBetween(ac.position, ac.goals[0].targetLocation) < 1.0f ) // goal is too close 
	//	return false; 

	path.clear ();
	bool pathComplete = gEngine->getPathPlanner()->findPath(ac.position,ac.goals[0].targetLocation,path,
			_maxNodesToExpandForSearch);
	return pathComplete; // std::vector<std::vector<Util::Point>> agentPaths
}
#undef min
bool ScenarioModule::isAgentRelevant( const SteerLib::AgentInitialConditions &ac,std::vector<Util::Point> &path  )
{
	assert (agentPaths.size() > 0 ); // egocentric agent must be set 

	unsigned int j=0;
	unsigned int testa = path.size();
	unsigned int testb = agentPaths[0].size();
	for(; j < std::min(path.size(), agentPaths[0].size()); j++)
	{
		if ( Util::distanceBetween(agentPaths[0][j],path[j]) < 2.0f ) 
			break;
	}
	if ( j == std::min(path.size(), agentPaths[0].size()) ) // this agent was not interacting with the egocentric agent
	{
			std::cerr << " no interaction with this agent \n";
			return false;
	}
	return true;
}

unsigned int ScenarioModule::calcInteractionPoints( std::vector<Util::Point> &path1 ,
		std::vector<Util::Point> &path2, float distance, unsigned int interval,
		std::vector<Util::Point> * _intersectionPoints)
{
	int num = 0;
	if ( (path1.size() > 0) && (path2.size() > 0))
	{
		for (unsigned int i = 0; i < path1.size(); i++)
		{
			int minq = i - interval;
			if ( minq < 0 )
			{
				minq = 0;
			}
			int max_q = i + interval;
			if ( max_q >= path2.size())
			{
				max_q = path2.size()-1;
			}
			for (int q = minq; q <= max_q; q++ )
			{
				// check for intersection.
				if	( Util::distanceBetween(path1.at(i),path2.at(q)) < distance )
				{
					num++;
					Util::Vector meanPointLine = (path1.at(i) - path2.at(q))/2.0f;
					Util::Point meanPoint = path2.at(q) + meanPointLine;
					_intersectionPoints->push_back(meanPoint);
#ifdef _DEBUG1
					std::cout << "meanPointLine: " << meanPointLine <<
							", path2.point: " << path2.at(q) << std::endl;
					std::cout << "Found some static intersection: " << meanPoint << std::endl;
#endif
				}
			}
		}
	}
	return num;
}

bool ScenarioModule::isObstacleRelevant( const SteerLib::BoxObstacle &ac )
{
	return true;
}


void ScenarioModule::loadAIModule ()
{

	// Construct options string again to pass to AIModule
	std::string * optionsString = new std::string("scenario");
	SteerLib::OptionDictionary::const_iterator optionIter;
	for (optionIter = (*_scenarioOptions).begin();
			optionIter != (*_scenarioOptions).end(); ++optionIter)
	{
		optionsString->append(",");
		optionsString->append((*optionIter).first.c_str());
		optionsString->append("=");
		optionsString->append((*optionIter).second.c_str());
	}
	// load the module if it is not already
	if ( !gEngine->isModuleLoaded(_aiModuleName) ) {
		gEngine->loadModule(_aiModuleName,_aiModuleSearchPath,*optionsString);
		/// @todo add a boolean that flags this module was loaded here, so that we can unload the module after the simulation is done.
	}

	// get a pointer to the loaded module
	_aiModule = gEngine->getModule(_aiModuleName);

}

void ScenarioModule::loadBenchmarkModule ()
{
	// load the module if it is not already
	if ( !gEngine->isModuleLoaded("steerBench") ) {
		gEngine->loadModule("steerBench","","");
		/// @todo add a boolean that flags this module was loaded here, so that we can unload the module after the simulation is done.
	}

	// get a pointer to the loaded module
	_benchmarkModule = static_cast<SteerLib::SteerBenchModule *> (gEngine->getModule("steerBench"));

}

void ScenarioModule::loadSimulationRecorderModule ()
{
	// load the module if it is not already
	if ( !gEngine->isModuleLoaded("simulationRecorder") ) {
		gEngine->loadModule("simulationRecorder","","");
		/// @todo add a boolean that flags this module was loaded here, so that we can unload the module after the simulation is done.
	}

	// get a pointer to the loaded module
	_recorderModule = static_cast<SteerLib::SimulationRecorderModule *> (gEngine->getModule("simulationRecorder"));

}



void ScenarioModule::loadCurrentScenario ()
{
	#ifdef _DEBUG1
	std::cout << "\nLoading Scenario: " << _currentScenario << "/" << _numScenarios << " ...\n";
	#endif
	for (unsigned int i=0; i < _obstacles.size(); i++) {
		//const SteerLib::ObstacleInitialConditions & ic = _obstacles[i];
		//SteerLib::BoxObstacle * b;
		//b = new SteerLib::BoxObstacle(ic.xmin, ic.xmax, ic.ymin, ic.ymax, ic.zmin, ic.zmax);
		std::cout << "adding obstacle to database " << &_obstacles[i] << std::endl;
		SteerLib::ObstacleInterface* obs = (_obstacles[i])->createObstacle();
		gEngine->addObstacle(obs);
		gEngine->getSpatialDatabase()->addObject( obs, obs->getBounds());
	}

	for (unsigned int i=0; i < _agents.size(); i++)
	{
		const SteerLib::AgentInitialConditions & ic = _agents[i];
		#ifdef _DEBUG1
		printf("ScenarioModule::loadCurrentScenario (%f, %f, %f) {\n", ic.goals[0].targetLocation.x, ic.goals[0].targetLocation.y, ic.goals[0].targetLocation.z);
		#endif
		AgentInterface * agent = gEngine->createAgent( ic, _aiModule );
		agent->reset(ic, gEngine);
		#ifdef _DEBUG1
		printf("}\n");
		#endif

		// do we need to manually add the agent to the spatial database ? 
	}

	#ifdef _DEBUG1
	std::cout << "Number of agents in scenario : " << gEngine->getAgents().size() << "\n";
	std::cout << "Number of obstacles in scenario : " << gEngine->getObstacles().size() << "\n";
	std::cout << "Scenario successfully loaded. \n\n";
	#endif
}

  /* reverse:  reverse string s in place */
 void reverse(char s[])
 {
     int i, j;
     char c;
 
     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }



 /* itoa:  convert n to characters in s */
 void myitoa(int n, char s[], int radix)
 {
     int i, sign;
 
     if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
     i = 0;
     do {       /* generate digits in reverse order */
         s[i++] = n % radix + '0';   /* get next digit */
     } while ((n /= radix) > 0);     /* delete it */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
 }

void ScenarioModule::saveScenario()
{
	std::string testCaseName;
	std::stringstream out;
	out << _testCaseRelativePath;
	//CORY -- COMMENTED OUT LINE BELOW IS ORIGINAL NAME CONSTRUCTION
	if(!useContext) {
		out << "testcase-" << _currentScenario + scenarioSetInitId;
	}
	else {
		// scenarioSetInitId does not need to be added here because it will
		// never be used when this is used. Glen .. Shows how much I know...
		out << "scenario_" << contextNum << "_" << _currentScenario + + scenarioSetInitId;
	}
	testCaseName = out.str();
	SteerLib::TestCaseWriter testCaseWriter;
	testCaseWriter.writeTestCaseToFile(testCaseName,_agents,_obstacles,gEngine);
	std::cout << "Dumped test case : " << testCaseName << "\n";
}

void ScenarioModule::generateScenario(unsigned int index)
{
	// clearScenario(); // this clears from the engine

	if (_dumpFrames)
	{
		int e;
		std::stringstream out;
		if ( strcmp(_frameDumpRelativePath.c_str(), Empty_String) == 0) // no relitive path
		{
			out << "scenario" << (_currentScenario + scenarioSetInitId);
		}
		else
		{
			out << _frameDumpRelativePath;
			out << "/scenario" << (_currentScenario + scenarioSetInitId);
		}
#ifdef _WIN32
				// Code to create a directory on Windows
		e = CreateDirectory(out.str().c_str(), NULL);
		if (e == 0)
		{
			// printf("mkdir failed; errno=%d\n",errno);
			perror("CreateDirectory failed for creating framePath");
		}
		else
		{
			printf("created the directory %s\n",_frameDumpRelativePath.c_str());
		}
#else
		e = mkdir(out.str().c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if (e != 0)
		{
			// printf("mkdir failed; errno=%d\n",errno);
			perror("mkdir failed for creating scenario path");
		}
		else
		{
			printf("created the directory %s\n",out.str().c_str());
		}
#endif
	}

	generateRandomScenario(false, _egocentricFlag);
	// generateRandomScenario2(false, _egocentricFlag);

	//generateRandomScenario(false, true);
	//generateRandomScenario(false, true);
	#ifdef _DEBUG1
	std::cout << _agents.size() << " (agents) " << _obstacles.size () << " (obstacles) \n";
	std::cout << _agents.size() << " (agents) " << _obstacles.size () << " (obstacles) \n";
	int iq = 0;
	for (std::vector<SteerLib::AgentInitialConditions>::iterator neighbor = _agents.begin();  neighbor != _agents.end();  neighbor++)
	{
		std::cout << "agent[" << iq++ << "] name = " << (*neighbor).name << " \n";
	}
	#endif

	if (_egocentricFlag )	
	{
		for (unsigned int i=0; i < gEngine->getAgents().size(); i++)
			gEngine->unselectAgent(gEngine->getAgents()[i]);

		gEngine->selectAgent(gEngine->getAgents()[0]);
	}

	//egocentricAgentPath.clear ();
	//findAgentPath(0,egocentricAgentPath);
	//std::cout << " Egocentric agent path " << egocentricAgentPath.size() << "\n";

	// dumping test cases 
	if ( _dumpTestCases )
	{
		this->saveScenario();
	}

	/*
	// resetting data structures 
	_agents.clear ();
	_obstacles.clear ();

	double goalRadius = 15;	//TODO determine this from region specification
	
	int bufferSize;
	if (index == 0) {
		bufferSize = 2;
	} else {
		bufferSize = int(log((double)index)/log((double)_numStates) + 1) + 1;
	}
	char* buffer = (char*)malloc(sizeof(char)*bufferSize);
	int agentNum = 0;
	int direction = 1;
	for(int t = 0; t < 2; t++) {
		direction = -direction;
		double startTheta = 0;
		for(int i = 0; i < regions.size(); i++) {
			for(int j = 0; j < regions[i].thetaRes; j++) {
				for(int k = 0; k < regions[i].radiusRes; k++) {
					
					itoa(index, buffer, _numStates);
					//printf("agentNum = %d\n", agentNum);
					//printf("buffer = %s\n", buffer);
					//printf("\n");
					if(agentNum < bufferSize - 1) {
						//buffer[agentNum] = '1' + _numGoalStates;
					//int num = 1<<agentNum;
					//printf("%d | %d = %d\n", index, num, index|num);
					//if((index|num) == index) {	//if the agent's spot is a '1'
						//printf("%d | %d = %d == %d\n", index, num, index|num, index);
						if (buffer[agentNum] > '0') {
							double theta = startTheta + regions[i].angle/(double)(regions[i].thetaRes)*(j+0.5);
							double radius = regions[i].radius/(double)(regions[i].radiusRes)*(k+1);
							
							printf("region radius ================================================= %f\n", regions[i].radius);
							printf("radius ======================================================== %f\n", radius);

							double x = -sin(theta*direction/180*PI)*radius;
							double y = cos(theta*direction/180*PI)*radius;

							if (buffer[agentNum] <= '0' + _numGoalStates) {

								double goalTheta = (buffer[agentNum] - '0')/(double)_numGoalStates*360;
							
								double goalX = -sin(goalTheta/180*PI)*goalRadius;
								double goalY = cos(goalTheta/180*PI)*goalRadius;
							
								printf("goalTheta = %f\n", goalTheta);
								printf("goal = (%f, %f)\n", goalX, goalY);
								
								double radius = 5.5;


								std::set<SteerLib::SpatialDatabaseItemPtr> neighbors;
								//neighbors.clear();
								gEngine->getSpatialDatabase()->getItemsInRange(neighbors, (float)(x-radius), (float)(x+radius), (float)(y-radius),(float)( y+radius), NULL);

								for (std::set<SteerLib::SpatialDatabaseItemPtr>::iterator neighbor = neighbors.begin(); neighbor != neighbors.end(); ++neighbor) {
									//SteerLib::AgentInitialConditions* neighbor = dynamic_cast<SteerLib::AgentInitialConditions*>(*neighbor);
									if(true) {
										printf("found SpaceTimeAgent neighbor!\n");
										exit(1);
									}
								}

						
								SteerLib::AgentInitialConditions ai;
								ai.position = Util::Point(x, 0.0f, y);
								ai.direction = Util::Vector(goalX-x, 0.0f,goalY-y);
								ai.radius = 0.5f;
								ai.speed = 0.0f;

								SteerLib::AgentGoalInfo agentGoal;
								agentGoal.goalType = SteerLib::GOAL_TYPE_SEEK_STATIC_TARGET;
								agentGoal.targetIsRandom = false;
							
								// desired speed needs to be initialized to some value. 
								// this could be a dimension we vary as well (slow, medium and, fast moving agents) 
								agentGoal.desiredSpeed = 1.0f;

								agentGoal.targetLocation = Util::Point(goalX, 0.0f, goalY);
								//agentGoal.targetLocation = Util::Point(0.0f, 0.0f, 0.0f);
								ai.goals.push_back(agentGoal);
								printf("ai.goals.size() = %d\n", ai.goals.size());

								_agents.push_back(ai);
							} else if (buffer[agentNum] == '1' + _numGoalStates) {
								//printf("Here!\n");
								//exit(0);
								double obstacleSize = 0.5;
								SteerLib::BoxObstacle b(x-obstacleSize, x+obstacleSize, 0, obstacleSize, y-obstacleSize, y+obstacleSize);
								_obstacles.push_back(b);
							}
						}
					}
					agentNum++;
					//glTranslatef(-sin(theta*direction/180*PI)*radius, cos(theta*direction/180*PI)*radius, 0);
				}
			}
			startTheta += regions[i].angle;
		}
	}

	// in case we are using RVOPlusAI
	if ( _aiModuleName.compare("RVOPlusAI") == 0)
	{
		std::cout << " coming here to preprocess rvo simulator \n";
		dynamic_cast<RVOPlusAIModule *>(_aiModule)->preprocessAgents(_agents);
		dynamic_cast<RVOPlusAIModule *>(_aiModule)->preprocessObstacles(_obstacles);
	}
	*/


}



void ScenarioModule::generateRandomScenario(bool reduced, bool egocentric)
{
	#ifdef _DEBUG1
	std::cerr << "generating random scenario .... \n";
	/*
	gSpatialDatabase->getItemsInRange(_neighbors, _position.x-PED_QUERY_RADIUS, _position.x+PED_QUERY_RADIUS,
			_position.z-PED_QUERY_RADIUS, _position.z+PED_QUERY_RADIUS, dynamic_cast<SpatialDatabaseItemPtr>(this));
			*/
	#endif

	_currentNumberOfRandomCalls = randGen->getNumberOfRandCalls (); // total number of rand calls before this scenario was generated

	//SteerLib::GridDatabase2D grid = SteerLib::GridDatabase2D(-_radius, _radius, -_radius, _radius, 100, 100, 20);

	double goalRadius = sqrt(2.0)/2 + 0.001;	//this is assuming the grid database resolution is 1

	double obstacleWidth = _radius*2.0/_gridResX;
	double obstacleHeight = _radius*2.0/_gridResY;

	// Badly named. This can not be set form passed arguments.
	// Means it is the radius used to randomly generate scenario.
	double agentRadius = _randomGeneratorRadius;// 0.66f; // Consider giving the agents a little more buffer room around them
	double obstacleTallness = 1.0f;
	// narrow passage
	BoxObstacleInitialConditions b1(-_radius,-_passageWidth/2.0f,0.0f,obstacleTallness,-_radius,_radius);
	BoxObstacleInitialConditions b2(_passageWidth/2.0f,_radius,0.0f,obstacleTallness,-_radius,_radius);

	// narrow corridoor
	BoxObstacleInitialConditions b11(-_radius,-_passageWidth/2.0f,0.0f,obstacleTallness,-_radius,-_passageWidth/2.0f);
	BoxObstacleInitialConditions b12(-_radius,-_passageWidth/2.0f,0.0f,obstacleTallness,1.0f,_radius);
	BoxObstacleInitialConditions b13(_passageWidth/2.0f,_radius,0.0f,obstacleTallness,-_radius,-_passageWidth/2.0f);
	BoxObstacleInitialConditions b14(_passageWidth/2.0f,_radius,0.0f,obstacleTallness,_passageWidth/2.0f,_radius);

	// obstacle at origin
	BoxObstacleInitialConditions bOrigin(-_passageWidth/2.0f,_passageWidth/2.0f,0.0f,obstacleTallness,-_passageWidth/2.0f,_passageWidth/2.0f);

	// obstacles at left and right of origing
	BoxObstacleInitialConditions bOriginLeft(-_passageWidth/2.0f - _passageWidth*3,_passageWidth/2.0f-_passageWidth*3,0.0f,obstacleTallness,-_passageWidth/2.0f,_passageWidth/2.0f);
	BoxObstacleInitialConditions bOriginRight(-_passageWidth/2.0f + _passageWidth*3,_passageWidth/2.0f+_passageWidth*3,0.0f,obstacleTallness,-_passageWidth/2.0f,_passageWidth/2.0f);

	switch (_obstacleState)
	{
	case 0:
		// no obstacles
		break;
	case 1:
	default:
		if(useContext && contextNum <= 11) {
			obstacleFreq = 0.0f;
		}
		else if(useContext) {
			obstacleFreq = 0.15f;
		}

		// normal random sampling of obstacles
		for(int x = 0; x < _gridResX; x++) {
			for(int y = 0; y < _gridResY; y++) {

				//double on = rand()/((double)RAND_MAX);
				double on = randGen->rand();
				if (on > (1.0 - obstacleFreq)) {
					double positionX = -_radius + obstacleWidth*(x+0.5);
					double positionY = -_radius + obstacleHeight*(y+0.5);

					if(!egocentric || (sqrt(positionX*positionX + positionY*positionY) - sqrt(obstacleWidth*obstacleWidth + obstacleHeight*obstacleHeight) > agentRadius)) {

						SteerLib::BoxObstacleInitialConditions b(positionX-obstacleWidth/2, positionX+obstacleWidth/2, 0, obstacleHeight, positionY-obstacleHeight/2, positionY+obstacleHeight/2);
						// SteerLib::BoxObstacleInitialConditions * b = new SteerLib::BoxObstacleInitialConditions(positionX-obstacleWidth/2, positionX+obstacleWidth/2, 0, 0.25, positionY-obstacleHeight/2, positionY+obstacleHeight/2);

						// TODO -- check if an obstacle is relevant

						_obstacles.push_back(new SteerLib::BoxObstacleInitialConditions(b));
						//grid.addObject(&b, b.getBounds());

						//int i = _obstacles.size()-1;
						//gEngine->addObstacle(&_obstacles[i]);
						//gEngine->getSpatialDatabase()->addObject( &_obstacles[i], _obstacles[i].getBounds());

						//gEngine->addObstacle(&_obstacles[_obstacles.size()-1]);
						//gEngine->getSpatialDatabase()->addObject(&_obstacles[_obstacles.size()-1], _obstacles[_obstacles.size()-1].getBounds());
					}
				}
			}
		}

		break;

	case 2:
		// narrow passage
		_obstacles.push_back(new SteerLib::BoxObstacleInitialConditions(b1));
		_obstacles.push_back(new SteerLib::BoxObstacleInitialConditions(b2));
		break;

	case 3:
		// narrow corridoor
		_obstacles.push_back(new SteerLib::BoxObstacleInitialConditions(b11));
		_obstacles.push_back(new SteerLib::BoxObstacleInitialConditions(b12));
		_obstacles.push_back(new SteerLib::BoxObstacleInitialConditions(b13));
		_obstacles.push_back(new SteerLib::BoxObstacleInitialConditions(b14));
		break;

	case 4:
		// obstacle at origin
		_obstacles.push_back(new SteerLib::BoxObstacleInitialConditions(bOrigin));
		break;

	case 5:
		// obstacles at center of world require lanes to form between oncoming groups

		_obstacles.push_back(new SteerLib::BoxObstacleInitialConditions(bOrigin));
		_obstacles.push_back(new SteerLib::BoxObstacleInitialConditions(bOriginLeft));
		_obstacles.push_back(new SteerLib::BoxObstacleInitialConditions(bOriginRight));
		break;

	}

	// obstacles added to database
	for (unsigned int i=0; i < _obstacles.size(); i++)
	{
		if ( typeid(*(_obstacles.at(i))) == typeid(SteerLib::BoxObstacleInitialConditions) )
		{
			// std::cout << "**** created Box obstacle:" << _obstacles.at(i) <<  std::endl;
			// SteerLib::BoxObstacleInitialConditions *bic = dynamic_cast<SteerLib::BoxObstacleInitialConditions*>(_obstacles[i]); /// TODO: Create support for all types of obstacles in scenarios
			//SteerLib::BoxObstacle  b(bic->xmin, bic->xmax, bic->ymin, bic->ymax, bic->zmin, bic->zmax);
			// b = new SteerLib::BoxObstacle(ic.xmin, ic.xmax, ic.ymin, ic.ymax, ic.zmin, ic.zmax);
			// SteerLib::BoxObstacle *b = dynamic_cast<SteerLib::BoxObstacle*>(bic->createObstacle());
			// _obstacles.push_back(bic);
			SteerLib::ObstacleInterface* obs = (_obstacles[i])->createObstacle();
			gEngine->addObstacle(obs);
			gEngine->getSpatialDatabase()->addObject( obs, obs->getBounds());
		}
		else
		{
			std::cout << "**** could not create this type of obstacle" << std::endl;
		}
	}

	// need to be before agents are added, but after obstacles are added.
	gEngine->getPathPlanner()->refresh();

	double theta;
	//int numAgents = rand()/((double)RAND_MAX+1)*(_maxAgents+1);
	//int numAgents = 1 + randGen.rand()  * (_maxAgents); // a minimum of 1 agent in the scene
	int numAgents = _minAgents + randGen->randInt(_maxAgents - _minAgents);	//_minAgents defaults to 1

	if (_numAgents != -1) // has been specified
		numAgents = _numAgents; // override


	Util::AxisAlignedBox region = Util::AxisAlignedBox(-_radius, _radius, -1, 1, -_radius, _radius);

	// region definition for groups
	Util::AxisAlignedBox groupRegion1 = (!useContext) ? Util::AxisAlignedBox(-_radius/2.0f, _radius/2.0f, -1, 1, _radius/2.0f, _radius) : Util::AxisAlignedBox(-_radius, _radius, -1, 1, _radius/2.0f, _radius);
	Util::AxisAlignedBox groupRegion2 = (!useContext) ? Util::AxisAlignedBox(-_radius/2.0f, _radius/2.0f, -1, 1, -_radius, -_radius/2.0f) : Util::AxisAlignedBox(-_radius, _radius, -1, 1, -_radius, -_radius/2.0f);
	Util::AxisAlignedBox groupRegion3 = (!useContext) ? Util::AxisAlignedBox(_radius/2.0f, _radius, -1, 1, -_radius/2.0f, _radius/2.0f) : Util::AxisAlignedBox(_radius/2.0f, _radius, -1, 1, -5, _radius);
	Util::AxisAlignedBox groupRegion4 = (!useContext) ? Util::AxisAlignedBox(-_radius, -_radius/2.0f, -1, 1, -_radius/2.0f, _radius/2.0f) : Util::AxisAlignedBox(-_radius, -_radius/2.0f, -1, 1, -5, _radius);
	Util::AxisAlignedBox groupRegion5 = Util::AxisAlignedBox(-_radius, _radius, -1, 1, -15, 2);	//this region is for use with the "matching" and "winning" contexts so the agent is not solely in front, may need tuning
	Util::AxisAlignedBox groupRegion6 = Util::AxisAlignedBox(-_radius/2.0f, _radius/2.0f, -1, 1, -_radius/2.0f, _radius/2.0f);

	// clearing agentPaths
	agentPaths.clear ();

	// There should be no lingering intersection points from previous scenarios
	assert( _refAgentIntersectPoints.size() == 0);

	for(int i = 0; i < numAgents; i++)
	{

		SteerLib::AgentInitialConditions ai;


		// assigning agent name (needed for test case dump)
		std::stringstream out;
		out << "agent" << i;
		ai.name = out.str();

		bool agentIntersects = false;
		bool solidAgent = false;
#ifdef _DEBUG1
	std::cout << "trying to create agent  " << ai.name << " at " << &ai << "\n";

#endif

		std::vector<Util::Point> agentPath;
		int maxCount = 0; // this sometimes keeps on running
		do
		{
			solidAgent=true;

			if ( maxCount > 1000 )
			{
				std::cout << "Gave up trying to place agent after " << maxCount << " tries." << std::endl;
				break;
			}
			//theta = rand()/((double)RAND_MAX)*2*PI;
			//radius = rand()/((double)RAND_MAX)*_radius;
			//theta =  randGen.rand()  *2*PI;
			//radius = randGen.rand() *_radius;


			//pick non-uniform points within _radius.  these coordinates are clustered towards the center, but that may be what we want

			Util::Point agentPosition;

			SteerLib::AgentGoalInfo agentGoal;
			agentGoal.goalType = SteerLib::GOAL_TYPE_SEEK_STATIC_TARGET;
			agentGoal.targetIsRandom = false;

			switch (_agentState * !useContext + 7 * useContext)
			{

			case 1 : // normal
			default:

				if ( egocentric && i ==0) // first agent when egocentric flag is active
					agentPosition = Util::Point(0.0f,0.0f,0.0f);
				else
				{
					agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(region, agentRadius, false, *randGen);
				}

				// randomly generated goal
				if ( _reducedGoals == false )
				{
					Util::Point goalPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(region, goalRadius, false,*randGen);
					double goalX = goalPosition.x;
					double goalY = goalPosition.z;
					agentGoal.targetLocation = Util::Point(goalX, 0.0f, goalY);
				}
				else
				{
					// goal in one of 8 directions
					int randomGoalDirection = randGen->rand(8);
					double g = _radius + 1.0;
					double d = g;
					double goalX, goalY;
					switch (randomGoalDirection)
					{
						case 0: goalX = 0.0; goalY =g; break;
						case 1: goalX = goalY = d; break;
						case 2: goalX = g;goalY = 0.0;break;
						case 3: goalX = d; goalY = -d; break;
						case 4: goalX = 0.0; goalY = -g; break;
						case 5: goalX = -d; goalY = -d; break;
						case 6 : goalX = -g; goalY = 0.0; break;
						case 7 : goalX = -d; goalY = d; break;
					}
					agentGoal.targetLocation = Util::Point(goalX, 0.0f, goalY);

				}
				break;

			case 3: // 1 group
				agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion1, agentRadius, false, *randGen);
				//agentGoal.targetLocation = Util::Point(0.0f, 0.0f, -_radius);
				agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion2, agentRadius,false, *randGen);
				break;

			case 4: // 2 oncoming groups
				if ( i < numAgents/2)
				{
					agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion1, agentRadius, false, *randGen);
					//agentGoal.targetLocation = Util::Point(0.0f, 0.0f, -_radius);
					agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion2, agentRadius,false,*randGen);
				}
				else
				{
					agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion2, agentRadius, false, *randGen);
					//agentGoal.targetLocation = Util::Point(0.0f, 0.0f, _radius);
					agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion1, agentRadius,false,*randGen);
				}
				break;

			case 5: // 2 crossing groups
				if ( i < numAgents/2)
				{
					agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion1, agentRadius, false, *randGen);
					//agentGoal.targetLocation = Util::Point(0.0f, 0.0f, -_radius);
					agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion2, agentRadius,false,*randGen);
				}
				else
				{
					agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion3, agentRadius, false, *randGen);
					//agentGoal.targetLocation = Util::Point(0.0f, 0.0f, _radius);
					agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion4, agentRadius,false,*randGen);
				}
				break;


			case 6: // group confusion
				if ( i < numAgents/4)
				{
					agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion1, agentRadius, false, *randGen);
					//agentGoal.targetLocation = Util::Point(0.0f, 0.0f, -_radius);
					agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion2, agentRadius,false,*randGen);
				}
				else if ( i < numAgents/2)
				{
					agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion2, agentRadius, false, *randGen);
					//agentGoal.targetLocation = Util::Point(0.0f, 0.0f, _radius);
					agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion1, agentRadius,false,*randGen);
				}
				else if ( i < 3* numAgents/4)
				{
					agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion3, agentRadius, false, *randGen);
					//agentGoal.targetLocation = Util::Point(0.0f, 0.0f, _radius);
					agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion4, agentRadius,false,*randGen);
				}
				else
				{
					agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion4, agentRadius, false, *randGen);
					//agentGoal.targetLocation = Util::Point(0.0f, 0.0f, _radius);
					agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion3, agentRadius,false,*randGen);
				}
				break;

			case 7:
				{
					if(i == 0) {
						agentPosition = Util::Point(0.0f,0.0f,0.0f);

						Util::Point goalPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion1, goalRadius, false,*randGen);	//subject always goes north
						double goalX = goalPosition.x;
						double goalY = goalPosition.z;
						agentGoal.targetLocation = Util::Point(goalX, 0.0f, goalY);
						break;
					}

					//contextNum specific
					switch ((contextNum >= 12) ? contextNum - 12 : contextNum) {
					case 0:
						//agent on its own or sparse other agents that aren't factors
						{
							agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(region, agentRadius, false, *randGen);

							Util::Point goalPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(region, goalRadius, false,*randGen);	//subject always goes north
							double goalX = goalPosition.x;
							double goalY = goalPosition.z;
							agentGoal.targetLocation = Util::Point(goalX, 0.0f, goalY);
							break;
						}
						break;
					case 1:
						//light oncoming
					case 2:
						//medium oncoming
					case 3:
						//heavy oncoming
						{
							if(i > 9) {
								agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(region, agentRadius, false, *randGen);
								agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(region, agentRadius,false,*randGen);
							}
							else {
								agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion1, agentRadius, false, *randGen);
								agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion2, agentRadius,false,*randGen);
							}
						}
						break;

					case 4:
						//matched oncoming
						{
							if(i > 9) {
								agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(region, agentRadius, false, *randGen);
								agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(region, agentRadius,false,*randGen);
							}
							else if (i % 2)
							{
								agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion1, agentRadius, false, *randGen);
								agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion2, agentRadius,false,*randGen);
							}
							else
							{
								agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion5, agentRadius, false, *randGen);
								agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion1, agentRadius,false,*randGen);
							}
						}
						break;
					case 5:
						//winning-side oncoming
						{
							if(i < 3) {
								agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion1, agentRadius, false, *randGen);
								agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion2, agentRadius,false,*randGen);
							}
							else if(i > 9) {
								agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(region, agentRadius, false, *randGen);
								agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(region, agentRadius,false,*randGen);
							}
							else {
								agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion5, agentRadius, false, *randGen);
								agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion1, agentRadius,false,*randGen);
							}
						}
						break;

					case 6:
						//light crossing
					case 7:
						//medium crossing
					case 8:
						//heavy crossing
						{
							if(i > 9) {
								agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(region, agentRadius, false, *randGen);
								agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(region, agentRadius,false,*randGen);
							}
							else if(_currentScenario % 2) {
								agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion4, agentRadius, false, *randGen);
								agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion3, agentRadius,false,*randGen);
							}
							else {
								agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion3, agentRadius, false, *randGen);
								agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion4, agentRadius,false,*randGen);
							}
						}
						break;

					case 9:
						//matched crossing
						{
							if(i > 9) {
								agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(region, agentRadius, false, *randGen);
								agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(region, agentRadius,false,*randGen);
							}
							else if(_currentScenario % 2) {
								if (i % 2)
								{
									agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion4, agentRadius, false, *randGen);
									agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion3, agentRadius,false,*randGen);
								}
								else
								{
									agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion5, agentRadius, false, *randGen);
									agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion1, agentRadius,false,*randGen);
								}
							}
							else {
								if (i % 2)
								{
									agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion3, agentRadius, false, *randGen);
									agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion4, agentRadius,false,*randGen);
								}
								else
								{
									agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion5, agentRadius, false, *randGen);
									agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion1, agentRadius,false,*randGen);
								}
							}
						}
						break;
					case 10:
						//winning-side crossing
						{
							if(i < 3) {
								if(_currentScenario % 2) {
									agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion4, agentRadius, false, *randGen);
									agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion3, agentRadius,false,*randGen);
								}
								else {
									agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion3, agentRadius, false, *randGen);
									agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion4, agentRadius,false,*randGen);
								}
							}
							else if(i > 9) {
								agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(region, agentRadius, false, *randGen);
								agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(region, agentRadius,false,*randGen);
							}
							else {
								agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion5, agentRadius, false, *randGen);
								agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion1, agentRadius,false,*randGen);
							}
						}
						break;

					case 11:
						if ( i < numAgents/4)
						{
							agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion6, agentRadius, false, *randGen);
							agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion2, agentRadius,false,*randGen);
						}
						else if ( i < numAgents/2)
						{
							agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion6, agentRadius, false, *randGen);
							agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion1, agentRadius,false,*randGen);
						}
						else if ( i < 3* numAgents/4)
						{
							agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion6, agentRadius, false, *randGen);
							agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion4, agentRadius,false,*randGen);
						}
						else
						{
							agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion6, agentRadius, false, *randGen);
							agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion3, agentRadius,false,*randGen);
						}
						break;
					case 12:
					case 13:
					default:
						//won't actually get here because an exception would've been thrown at the beginning of the program
						break;
					}
				}
				break;

			case 2: // placing agents regularly along the circumference of the circle with diametrically opposite goals 


				double degreeIncrement = 360.0 / numAgents;
				double radIncrement = degreeIncrement * 0.0174532925; 
				double thetaRad = 0.0;

				thetaRad = radIncrement * i;

				agentPosition.x = _radius * cos(thetaRad);
				agentPosition.y = 0.0f;
				agentPosition.z = _radius * sin(thetaRad);

				/*
				this was the previous code which was randomly placing agents along circumference 

				bool collides = false;
				do
				{
					thetaRad = randGen->rand(M_2_PI);
					agentPosition.x = _radius * cos(thetaRad);
					agentPosition.y = 0.0f;
					agentPosition.z = _radius * sin(thetaRad);

					// check if no collision
					unsigned int j=0;
					for (; j < _agents.size(); j++)
						if (Util::circleOverlapsCircle2D(agentPosition,0.5f,_agents[j].position,0.5f))
						{
							collides = true;
							break;
						}

					if (j == _agents.size()) collides = false;

				}
				while (collides == true);
				*/



				// agent goal
				double goalThetaRad = thetaRad + M_PI;
				if (goalThetaRad > M_2_PI)
					goalThetaRad = goalThetaRad - M_2_PI;

				agentGoal.targetLocation = Util::Point(_radius * cos(goalThetaRad), 0.0f, _radius * sin(goalThetaRad));
				break;


			}



			//double x = cos(theta)*radius;
			//double y = sin(theta)*radius;


			//theta = randGen.rand() *2*PI;
			//radius = randGen.rand() *_radius;

			//printf("goalPosition = (%f, %f, %f)\n", goalPosition.x, goalPosition.y, goalPosition.z);
			//double goalX = cos(theta)*radius;
			//double goalY = sin(theta)*radius;

			ai.position = agentPosition;

			ai.radius = agentRadius; // This initializes the agents radius, could be dangerous
			ai.speed = 0.0f;
			ai.name = "agent" + _agents.size();


			// randomly generated speed
			if ( _fixedSpeed == false )
			{
				//double speed = rand()/((double)RAND_MAX)*((double)_maxSpeed-_minSpeed) + _minSpeed;
				double speed = randGen->rand() * ((double)_maxSpeed-_minSpeed) + _minSpeed;
				agentGoal.desiredSpeed = speed;
			}
			else
			{
				agentGoal.desiredSpeed = DESIRED_SPEED;
			}



			if ( _fixedDirection == false )
			{
				// randomly generated direction
				theta = randGen->rand() * M_2_PI;
				// std::cout << "" << std::endl << "Check for good facing direction for agent: " << ai.name << ", " << _agents.size() <<
				//		" at location " << ai.position << std::endl;

				double directionX = cos(theta);
				double directionY = sin(theta);
				ai.direction = Util::Vector(directionX, 0.0f, directionY);

				if (_aiModuleName == "footstepAI")
				{
					float FORWARD_FOOTSTEP_BOX = 0.7f;
					float shoulder_comfort_zone = 0.325f;

					Util::Ray ai_dir_ray;
					Util::Ray ai_dir_ray_L;
					Util::Ray ai_dir_ray_R;
					size_t dir_tries = 20;
					size_t ii = 0;
					for (; ii < dir_tries; ii++)
					{// tries to find a good facing direction
						Util::Ray right_shoulder_ray;
						Util::Ray left_shoulder_ray;
						Util::Ray front_ray;

						right_shoulder_ray.dir = ai.direction*FORWARD_FOOTSTEP_BOX;// forward();
						left_shoulder_ray.dir = ai.direction*FORWARD_FOOTSTEP_BOX;
						// front_ray.dir = Util::rotateInXZPlane(collisionForward*0.8f, M_PI_2);

						right_shoulder_ray.pos = ai.position +  Util::rotateInXZPlane(ai.direction*shoulder_comfort_zone, -M_PI_2);
						left_shoulder_ray.pos = ai.position +  Util::rotateInXZPlane(ai.direction*shoulder_comfort_zone, M_PI_2);
						front_ray.dir = Util::rotateInXZPlane(ai.direction*(shoulder_comfort_zone*2.0f), M_PI_2);
						front_ray.pos = right_shoulder_ray.pos + right_shoulder_ray.dir;

						right_shoulder_ray.mint = 0;
						right_shoulder_ray.maxt = 1.0f;

						left_shoulder_ray.mint = 0;
						left_shoulder_ray.maxt = 1.0f;

						// std::cout << "Testing ray collision:" << std::endl;

						front_ray.mint = 0.0f;
						front_ray.maxt = 1.0f;
						std::set<SteerLib::ObstacleInterface*>::iterator neighbour = gEngine->getObstacles().begin();
						for (;  neighbour != gEngine->getObstacles().end();  neighbour++)
						{
							SteerLib::ObstacleInterface * obstacle = *neighbour;
							float right_t = 0, left_t = 0, front_t=0;
							// std::cout << "Checking ray obstacle intersection: " << std::endl;

							if ( obstacle->intersects(right_shoulder_ray, right_t) )
							{
								// Util::DrawLib::drawStar(right_shoulder_ray.pos + (right_shoulder_ray.dir * t_time ) , Util::Vector(1,0,0), 0.34f, gGreen);
								// std::cout << "Found foot ray test intersection: " << std::endl;
								break;
							}
							if ( obstacle->intersects(left_shoulder_ray, left_t) )
							{
								// Util::DrawLib::drawStar(right_shoulder_ray.pos + (right_shoulder_ray.dir * t_time ) , Util::Vector(1,0,0), 0.34f, gGreen);
								// std::cout << "Found foot ray test intersection: " << std::endl;
								break;
							}
							if ( obstacle->intersects(front_ray, front_t) )
							{
								// Util::DrawLib::drawStar(right_shoulder_ray.pos + (right_shoulder_ray.dir * t_time ) , Util::Vector(1,0,0), 0.34f, gGreen);
								// std::cout << "Found foot ray test intersection: " << std::endl;
								break;
							}

						}
						if ( neighbour == gEngine->getObstacles().end() )
						{ // went though all obstacles with no intersection
							break;
						}
						else
						{ // try another direction
							theta = randGen->rand() * M_2_PI;
							double directionX = cos(theta);
							double directionY = sin(theta);
							ai.direction = Util::Vector(directionX, 0.0f, directionY);
							// std::cout << "Trying different agent direction: " << std::endl;
						}
					}
					if ( ii == dir_tries )
					{
						std::cout << "Could not find good direction to place agent." << std::endl;
					}
				}
			}
			else
			{
				std::cout << "fixed direction is true" << std::endl;
				// we just want it to face its goal
				Util::Vector direction = agentGoal.targetLocation - ai.position;
				direction = Util::normalize(direction);
				ai.direction = direction;

			}

			ai.goals.clear ();
			ai.goals.push_back(agentGoal);
			#ifdef _DEBUG1
			std::cout << "max count " << maxCount << " " << i << "\n";
			#endif
			maxCount ++;

			Util::Point intersectionPoint2 = Util::Point(0, 0, 0);
			// std::cout << "i: " << i << std::endl;

			/*
			 * At this point all of the logic needed to validate the scenario wrt the scenario desired
			 * should be computed.
			 */
			if (_agents.size() > 0 && _checkAgentRelevant) // egocentric agents need to already exsist.
			{
				/*
				 * This is ambiguity of whether or now the true path line should be used
				 * 1. Having it the full length induces the trend of many agents having the same goal
				 * 2. Having it a little shorter to avoid one makes it impossible for any other agent to have the
				 * same goal as the reference agent.
				 * It is decided to be the least biased the algorithm will randomely choose which one it will be.
				 */
				SteerLib::AgentInitialConditions refernceAgent = _agents.at(0);

				float fullLineLengthOrNot = 0;
				if ( randGen->rand() > 0.5f)
				{
					fullLineLengthOrNot = 1.0f;
				}
				else
				{
					fullLineLengthOrNot = 0.99f;
				}
				intersectionPoint2 = Util::Point(0,0,0);

				agentIntersects = Util::intersect2Lines2D(ai.position, ai.position +
						((agentGoal.targetLocation - ai.position)*fullLineLengthOrNot),
						_agents.at(0).position, _agents.at(0).position +
						(_agents.at(0).goals.at(_agents.at(0).goals.size()-1).targetLocation - _agents.at(0).position),
						intersectionPoint2);
				/*std::cout << "Calculated intersection Point: " << intersectionPoint2 << std::endl;
				std::cout << "The ai position is: " << ai.position << " and goal is: " <<
						agentGoal.targetLocation << std::endl;
				std::cout << "The reference agent position is: " << _agents.at(0).position << " and goal is: " <<
						_agents.at(0).goals.at(_agents.at(0).goals.size()-1).targetLocation << std::endl;
	*/
				if ( !agentIntersects )
				{
					solidAgent = solidAgent && false;
					// At this point we know the Agent is not valid.
					continue;
				}
				else
				{
					if ( _checkAgentInteraction )
					{
						// Here we check to see if we have an expected interaction.

						// Check if agent interacts with reference agent
						// Compute the time it will take for the reference agent to reach the intersection point

						float ref_intersect_time = (intersectionPoint2 - refernceAgent.position).length()/refernceAgent.goals.front().desiredSpeed;
						float other_intersect_time = (intersectionPoint2 - ai.position).length() / ai.goals.front().desiredSpeed;

						// Compute the point the other agent will be at when the reference agent is supposed to be at the
						// intersection point.
						Util::Point other_agent_point_at_ref_time = ai.position +
								((normalize(intersectionPoint2 - ai.position) * ai.goals.front().desiredSpeed) // velocity vector
								* ref_intersect_time);

						// Compute the point the reference agent will be at when the other agent is supposed to be at the
						// intersection point.
						Util::Point ref_agent_point_at_other_time = refernceAgent.position +
							((normalize(intersectionPoint2 - refernceAgent.position) * refernceAgent.goals.front().desiredSpeed) // velocity vector
							* other_intersect_time);

						// Check if either of these two points are withing the interaction Radius


						if ( ((ref_agent_point_at_other_time - intersectionPoint2).length() < _checkAgentInteractionRadius) ||
								(other_agent_point_at_ref_time - intersectionPoint2).length() < _checkAgentInteractionRadius )
						{
							solidAgent = solidAgent && true;
						}
						else
						{
							solidAgent = solidAgent && false;
							continue;
						}

						/*
						std::cout << "current intersection point: " << intersectionPoint2 << std::endl;
						std::cout << "reference Agent position: " << ref_agent_point_at_other_time << std::endl;
						std::cout << "reference Agent desired speed: " << refernceAgent.goals.front().desiredSpeed << std::endl;
						std::cout << "other Agent position: " << other_agent_point_at_ref_time << std::endl;
						std::cout << "other Agent desired speed: " << ai.goals.front().desiredSpeed << std::endl;
						*/

					}
					else
					{
						solidAgent = solidAgent && true;
					}
				}

			}

			if (_checkAgentValid)
			{
				solidAgent = solidAgent  && (isAgentValid(ai, agentPath));
				if (!solidAgent)
				{
					continue;
				}
			}

			if (_checkAgentRelevant && i > 0)
			{
				_refAgentIntersectPoints.push_back(intersectionPoint2);
				// std::cout << ai.name << " position is " << ai.position << " for ref intersections" << std::endl;
				// std::cout << ai.name << " goal is " << agentGoal.targetLocation << " for ref intersections" << std::endl;
			}

			// std::cout << "i2: " << i << std::endl;
		}
		while ( !solidAgent );
		// while ( !solidAgent && (maxCount < 110) );
		// while( !agentIntersects && i != 0 && maxCount < 100); // This needs a bound to keep from
			//note i disabled most relevancy checking
		/*
		 * while(((_checkAgentValid || _checkAgentRelevant) && isAgentValid(ai,agentPath) == false)  &&
			 ((maxCount < 10) ||
			 (i != 0 && (contextNum == 0 && _checkAgentRelevant && useContext && isAgentRelevant(ai,agentPath)) ) ) ); // we don't want relevant agents here??
		 */

		agentPaths.push_back(agentPath);

		_agents.push_back(ai);

		_agentGoals.push_back(ai.goals.at(0).targetLocation);
		_agentInitialDisks.push_back(Util::Circle(ai.position.x, ai.position.z, ai.radius));
		_agentInitialForwardDirections.push_back(ai.direction);

		const SteerLib::AgentInitialConditions & ic = _agents[_agents.size()-1];
		AgentInterface * agent = gEngine->createAgent( ic, _aiModule );
		agent->reset(ic, gEngine);


		 // Could not hold this assertion because there can be issues placing agents
		// to meet all of the conditions.
		if (_checkAgentRelevant && false)
		{
			/*
			 * If we are checking for agent path intersection there should be
			 * one less intersection then there are agents.
			 */
			if ( !(_refAgentIntersectPoints.size() == (_agents.size() - 1)) )
			{
				std::cout << "Improper number of agent intersection points." << std::endl;
				std::cout << "Should be " << (_agents.size() - 1);
				// std::cout << "Intersection points: " << _repAgentIntersectPoints << " agents: " << _agents.size() << std::endl;
				std::cout << " is " << _refAgentIntersectPoints.size() << std::endl;
			}
			assert( _refAgentIntersectPoints.size() == (_agents.size() - 1));
		}

		//Util::AxisAlignedBox newBounds(x-agentRadius, x+agentRadius, 0.0f, 0.0f, y-agentRadius, y+agentRadius);
		//grid.addObject( dynamic_cast<SteerLib::SpatialDatabaseItemPtr>(ai), newBounds);
	}
	//printf("HERE\n");
	//loadCurrentScenario();
	if (_checkAgentRelevant )
	{
		// computes all agent path intersections
		Util::Point intersectPoint;
		bool agentsIntersect = false;
		for ( int g = 0; g < _agents.size(); g++)
		{
			// std::cout << _agents.at(g).name << " position is " << _agents.at(g).position << " for all intersections" << std::endl;
			// std::cout << _agents.at(g).name << " goal is " << _agents.at(g).goals.at(0).targetLocation << " for all intersections" << std::endl;
			for (int t = g + 1; t < _agents.size(); t++ )
			{
				agentsIntersect = Util::intersect2Lines2D(_agents.at(g).position, _agents.at(g).position +
												((_agents.at(g).goals.at(0).targetLocation - _agents.at(g).position)),
												_agents.at(t).position, _agents.at(t).position +
												(_agents.at(t).goals.at(0).targetLocation - _agents.at(t).position),
												intersectPoint);
				if ( agentsIntersect )
				{
					_agentIntersectionPoints.push_back(intersectPoint);
					intersectPoint = Util::Point(0,0,0);
				}
			}
		}
	}


	#ifdef _DEBUG1
	std::cerr << "scenario generated.  \n";
	#endif

}

/*
 * Uses a different method to check agent placement checks.
 * It acctually loads an agent and resets it, then checks if that agent has collisions.
 */
void ScenarioModule::generateRandomScenario2(bool reduced, bool egocentric)
{
	#ifdef _DEBUG1
	std::cerr << "generating random scenario .... \n";
	/*
	gSpatialDatabase->getItemsInRange(_neighbors, _position.x-PED_QUERY_RADIUS, _position.x+PED_QUERY_RADIUS,
			_position.z-PED_QUERY_RADIUS, _position.z+PED_QUERY_RADIUS, dynamic_cast<SpatialDatabaseItemPtr>(this));
			*/
	#endif

	_currentNumberOfRandomCalls = randGen->getNumberOfRandCalls (); // total number of rand calls before this scenario was generated

	//SteerLib::GridDatabase2D grid = SteerLib::GridDatabase2D(-_radius, _radius, -_radius, _radius, 100, 100, 20);

	double goalRadius = sqrt(2.0)/2 + 0.001;	//this is assuming the grid database resolution is 1

	double obstacleWidth = _radius*2.0/_gridResX;
	double obstacleHeight = _radius*2.0/_gridResY;

	// Badly named. This can not be set form passed arguments.
	// Means it is the radius used to randomly generate scenario.
	double agentRadius = _randomGeneratorRadius;// 0.66f; // Consider giving the agents a little more buffer room around them

	// narrow passage
	BoxObstacleInitialConditions b1(-_radius,-_passageWidth/2.0f,0.0f,0.25f,-_radius,_radius);
	BoxObstacleInitialConditions b2(_passageWidth/2.0f,_radius,0.0f,0.25f,-_radius,_radius);

	// narrow corridoor
	BoxObstacleInitialConditions b11(-_radius,-_passageWidth/2.0f,0.0f,0.25f,-_radius,-_passageWidth/2.0f);
	BoxObstacleInitialConditions b12(-_radius,-_passageWidth/2.0f,0.0f,0.25f,1.0f,_radius);
	BoxObstacleInitialConditions b13(_passageWidth/2.0f,_radius,0.0f,0.25f,-_radius,-_passageWidth/2.0f);
	BoxObstacleInitialConditions b14(_passageWidth/2.0f,_radius,0.0f,0.25f,_passageWidth/2.0f,_radius);

	// obstacle at origin
	BoxObstacleInitialConditions bOrigin(-_passageWidth/2.0f,_passageWidth/2.0f,0.0f,0.25f,-_passageWidth/2.0f,_passageWidth/2.0f);

	// obstacles at left and right of origing
	BoxObstacleInitialConditions bOriginLeft(-_passageWidth/2.0f - _passageWidth*3,_passageWidth/2.0f-_passageWidth*3,0.0f,0.25f,-_passageWidth/2.0f,_passageWidth/2.0f);
	BoxObstacleInitialConditions bOriginRight(-_passageWidth/2.0f + _passageWidth*3,_passageWidth/2.0f+_passageWidth*3,0.0f,0.25f,-_passageWidth/2.0f,_passageWidth/2.0f);

	switch (_obstacleState)
	{
	case 0:
		// no obstacles
		break;
	case 1:
	default:
		if(useContext && contextNum <= 11) {
			obstacleFreq = 0.0f;
		}
		else if(useContext) {
			obstacleFreq = 0.15f;
		}

		// normal random sampling of obstacles
		for(int x = 0; x < _gridResX; x++) {
			for(int y = 0; y < _gridResY; y++) {

				//double on = rand()/((double)RAND_MAX);
				double on = randGen->rand();
				if (on > (1.0 - obstacleFreq)) {
					double positionX = -_radius + obstacleWidth*(x+0.5);
					double positionY = -_radius + obstacleHeight*(y+0.5);

					if(!egocentric || (sqrt(positionX*positionX + positionY*positionY) - sqrt(obstacleWidth*obstacleWidth + obstacleHeight*obstacleHeight) > agentRadius)) {

						SteerLib::BoxObstacleInitialConditions b(positionX-obstacleWidth/2, positionX+obstacleWidth/2, 0, 0.25, positionY-obstacleHeight/2, positionY+obstacleHeight/2);
						// SteerLib::BoxObstacleInitialConditions * b = new SteerLib::BoxObstacleInitialConditions(positionX-obstacleWidth/2, positionX+obstacleWidth/2, 0, 0.25, positionY-obstacleHeight/2, positionY+obstacleHeight/2);

						// TODO -- check if an obstacle is relevant

						_obstacles.push_back(new SteerLib::BoxObstacleInitialConditions(b));
						//grid.addObject(&b, b.getBounds());

						//int i = _obstacles.size()-1;
						//gEngine->addObstacle(&_obstacles[i]);
						//gEngine->getSpatialDatabase()->addObject( &_obstacles[i], _obstacles[i].getBounds());

						//gEngine->addObstacle(&_obstacles[_obstacles.size()-1]);
						//gEngine->getSpatialDatabase()->addObject(&_obstacles[_obstacles.size()-1], _obstacles[_obstacles.size()-1].getBounds());
					}
				}
			}
		}

		break;

	case 2:
		// narrow passage
		_obstacles.push_back(new SteerLib::BoxObstacleInitialConditions(b1));
		_obstacles.push_back(new SteerLib::BoxObstacleInitialConditions(b2));
		break;

	case 3:
		// narrow corridoor
		_obstacles.push_back(new SteerLib::BoxObstacleInitialConditions(b11));
		_obstacles.push_back(new SteerLib::BoxObstacleInitialConditions(b12));
		_obstacles.push_back(new SteerLib::BoxObstacleInitialConditions(b13));
		_obstacles.push_back(new SteerLib::BoxObstacleInitialConditions(b14));
		break;

	case 4:
		// obstacle at origin
		_obstacles.push_back(new SteerLib::BoxObstacleInitialConditions(bOrigin));
		break;

	case 5:
		// obstacles at center of world require lanes to form between oncoming groups

		_obstacles.push_back(new SteerLib::BoxObstacleInitialConditions(bOrigin));
		_obstacles.push_back(new SteerLib::BoxObstacleInitialConditions(bOriginLeft));
		_obstacles.push_back(new SteerLib::BoxObstacleInitialConditions(bOriginRight));
		break;

	}

	// obstacles added to database
	for (unsigned int i=0; i < _obstacles.size(); i++)
	{
		if ( typeid(*(_obstacles.at(i))) == typeid(SteerLib::BoxObstacleInitialConditions) )
		{
			// std::cout << "**** created Box obstacle:" << _obstacles.at(i) <<  std::endl;
			// SteerLib::BoxObstacleInitialConditions *bic = dynamic_cast<SteerLib::BoxObstacleInitialConditions*>(_obstacles[i]); /// TODO: Create support for all types of obstacles in scenarios
			//SteerLib::BoxObstacle  b(bic->xmin, bic->xmax, bic->ymin, bic->ymax, bic->zmin, bic->zmax);
			// b = new SteerLib::BoxObstacle(ic.xmin, ic.xmax, ic.ymin, ic.ymax, ic.zmin, ic.zmax);
			// SteerLib::BoxObstacle *b = dynamic_cast<SteerLib::BoxObstacle*>(bic->createObstacle());
			// _obstacles.push_back(bic);
			SteerLib::ObstacleInterface* obs = (_obstacles[i])->createObstacle();
			gEngine->addObstacle(obs);
			gEngine->getSpatialDatabase()->addObject( obs, obs->getBounds());
		}
		else
		{
			std::cout << "**** could not create this type of obstacle" << std::endl;
		}
	}

	double theta;
	//int numAgents = rand()/((double)RAND_MAX+1)*(_maxAgents+1);
	//int numAgents = 1 + randGen.rand()  * (_maxAgents); // a minimum of 1 agent in the scene
	int numAgents = _minAgents + randGen->randInt(_maxAgents - _minAgents);	//_minAgents defaults to 1

	if (_numAgents != -1) // has been specified
		numAgents = _numAgents; // override


	Util::AxisAlignedBox region = Util::AxisAlignedBox(-_radius, _radius, -1, 1, -_radius, _radius);

	// region definition for groups
	Util::AxisAlignedBox groupRegion1 = (!useContext) ? Util::AxisAlignedBox(-_radius/2.0f, _radius/2.0f, -1, 1, _radius/2.0f, _radius) : Util::AxisAlignedBox(-_radius, _radius, -1, 1, _radius/2.0f, _radius);
	Util::AxisAlignedBox groupRegion2 = (!useContext) ? Util::AxisAlignedBox(-_radius/2.0f, _radius/2.0f, -1, 1, -_radius, -_radius/2.0f) : Util::AxisAlignedBox(-_radius, _radius, -1, 1, -_radius, -_radius/2.0f);
	Util::AxisAlignedBox groupRegion3 = (!useContext) ? Util::AxisAlignedBox(_radius/2.0f, _radius, -1, 1, -_radius/2.0f, _radius/2.0f) : Util::AxisAlignedBox(_radius/2.0f, _radius, -1, 1, -5, _radius);
	Util::AxisAlignedBox groupRegion4 = (!useContext) ? Util::AxisAlignedBox(-_radius, -_radius/2.0f, -1, 1, -_radius/2.0f, _radius/2.0f) : Util::AxisAlignedBox(-_radius, -_radius/2.0f, -1, 1, -5, _radius);
	Util::AxisAlignedBox groupRegion5 = Util::AxisAlignedBox(-_radius, _radius, -1, 1, -15, 2);	//this region is for use with the "matching" and "winning" contexts so the agent is not solely in front, may need tuning
	Util::AxisAlignedBox groupRegion6 = Util::AxisAlignedBox(-_radius/2.0f, _radius/2.0f, -1, 1, -_radius/2.0f, _radius/2.0f);

	// _agents.reserve(numAgents);	//TODO:  bad practice, it's still possible the pointers to elements could change -- when??

	// clearing agentPaths
	agentPaths.clear ();

	// There should be no lingering intersection points from previous scenarios
	assert( _refAgentIntersectPoints.size() == 0);

	for(int i = 0; i < numAgents; i++)
	{

		SteerLib::AgentInitialConditions ai;


		// assigning agent name (needed for test case dump)
		std::stringstream out;
		out << "agent" << i;
		ai.name = out.str();

		bool agentIntersects = false;
		bool solidAgent = false;
#ifdef _DEBUG1
	std::cout << "trying to create agent  " << ai.name << " at " << &ai << "\n";

#endif
		AgentInterface * dummyAgent = this->_aiModule->createAgent();
		AgentInitialConditions aic;
		aic.position = Util::Point(0,0,0);
		aic.fromRandom = false;
		aic.radius = 0.5f;
		aic.direction = Util::Vector(0,0,1);
		AgentGoalInfo agi;
		agi.goalType = GOAL_TYPE_SEEK_STATIC_TARGET;
		agi.targetLocation = aic.position;
		aic.goals.push_back(agi);
		dummyAgent->reset(aic,gEngine );
		dummyAgent->disable();
		bool foundGoodAgent = false;
		Util::Vector agentForward;
		std::vector<Util::Point> agentPath;
		int maxCount = 0; // this sometimes keeps on running
		do
		{
			solidAgent=true;

			if ( maxCount > 1000 )
			{
				std::cout << "Gave up trying to place agent after " << maxCount << " tries." << std::endl;
				break;
			}
			//theta = rand()/((double)RAND_MAX)*2*PI;
			//radius = rand()/((double)RAND_MAX)*_radius;
			//theta =  randGen.rand()  *2*PI;
			//radius = randGen.rand() *_radius;


			//pick non-uniform points within _radius.  these coordinates are clustered towards the center, but that may be what we want

			Util::Point agentPosition;

			SteerLib::AgentGoalInfo agentGoal;
			agentGoal.goalType = SteerLib::GOAL_TYPE_SEEK_STATIC_TARGET;
			agentGoal.targetIsRandom = false;

			switch ( 1 ) //_agentState * !useContext + 7 * useContext)
			{

			case 1 : // normal
			default:

				if ( egocentric && i ==0) // first agent when egocentric flag is active
					agentPosition = Util::Point(0.0f,0.0f,0.0f);
				else
				{
					foundGoodAgent = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(region, dummyAgent, false, *randGen);
					agentPosition = dummyAgent->position();
					agentForward = dummyAgent->forward();
					// std::cout << "**** Trying to place footstep agent *****" << std::endl;
				}

				// randomly generated goal
				if ( _reducedGoals == false )
				{
					Util::Point goalPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(region, goalRadius, false,*randGen);
					double goalX = goalPosition.x;
					double goalY = goalPosition.z;
					agentGoal.targetLocation = Util::Point(goalX, 0.0f, goalY);
				}
				else
				{
					// goal in one of 8 directions
					int randomGoalDirection = randGen->rand(8);
					double g = _radius + 1.0;
					double d = g;
					double goalX, goalY;
					switch (randomGoalDirection)
					{
						case 0: goalX = 0.0; goalY =g; break;
						case 1: goalX = goalY = d; break;
						case 2: goalX = g;goalY = 0.0;break;
						case 3: goalX = d; goalY = -d; break;
						case 4: goalX = 0.0; goalY = -g; break;
						case 5: goalX = -d; goalY = -d; break;
						case 6 : goalX = -g; goalY = 0.0; break;
						case 7 : goalX = -d; goalY = d; break;
					}
					agentGoal.targetLocation = Util::Point(goalX, 0.0f, goalY);

				}
				break;

			case 3: // 1 group
				agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion1, agentRadius, false, *randGen);
				//agentGoal.targetLocation = Util::Point(0.0f, 0.0f, -_radius);
				agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion2, agentRadius,false, *randGen);
				break;

			case 4: // 2 oncoming groups
				if ( i < numAgents/2)
				{
					agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion1, agentRadius, false, *randGen);
					//agentGoal.targetLocation = Util::Point(0.0f, 0.0f, -_radius);
					agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion2, agentRadius,false,*randGen);
				}
				else
				{
					agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion2, agentRadius, false, *randGen);
					//agentGoal.targetLocation = Util::Point(0.0f, 0.0f, _radius);
					agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion1, agentRadius,false,*randGen);
				}
				break;

			case 5: // 2 crossing groups
				if ( i < numAgents/2)
				{
					agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion1, agentRadius, false, *randGen);
					//agentGoal.targetLocation = Util::Point(0.0f, 0.0f, -_radius);
					agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion2, agentRadius,false,*randGen);
				}
				else
				{
					agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion3, agentRadius, false, *randGen);
					//agentGoal.targetLocation = Util::Point(0.0f, 0.0f, _radius);
					agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion4, agentRadius,false,*randGen);
				}
				break;


			case 6: // group confusion
				if ( i < numAgents/4)
				{
					agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion1, agentRadius, false, *randGen);
					//agentGoal.targetLocation = Util::Point(0.0f, 0.0f, -_radius);
					agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion2, agentRadius,false,*randGen);
				}
				else if ( i < numAgents/2)
				{
					agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion2, agentRadius, false, *randGen);
					//agentGoal.targetLocation = Util::Point(0.0f, 0.0f, _radius);
					agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion1, agentRadius,false,*randGen);
				}
				else if ( i < 3* numAgents/4)
				{
					agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion3, agentRadius, false, *randGen);
					//agentGoal.targetLocation = Util::Point(0.0f, 0.0f, _radius);
					agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion4, agentRadius,false,*randGen);
				}
				else
				{
					agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion4, agentRadius, false, *randGen);
					//agentGoal.targetLocation = Util::Point(0.0f, 0.0f, _radius);
					agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion3, agentRadius,false,*randGen);
				}
				break;

			case 7:
				{
					if(i == 0) {
						agentPosition = Util::Point(0.0f,0.0f,0.0f);

						Util::Point goalPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion1, goalRadius, false,*randGen);	//subject always goes north
						double goalX = goalPosition.x;
						double goalY = goalPosition.z;
						agentGoal.targetLocation = Util::Point(goalX, 0.0f, goalY);
						break;
					}

					//contextNum specific
					switch ((contextNum >= 12) ? contextNum - 12 : contextNum) {
					case 0:
						//agent on its own or sparse other agents that aren't factors
						{
							agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(region, agentRadius, false, *randGen);

							Util::Point goalPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(region, goalRadius, false,*randGen);	//subject always goes north
							double goalX = goalPosition.x;
							double goalY = goalPosition.z;
							agentGoal.targetLocation = Util::Point(goalX, 0.0f, goalY);
							break;
						}
						break;
					case 1:
						//light oncoming
					case 2:
						//medium oncoming
					case 3:
						//heavy oncoming
						{
							if(i > 9) {
								agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(region, agentRadius, false, *randGen);
								agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(region, agentRadius,false,*randGen);
							}
							else {
								agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion1, agentRadius, false, *randGen);
								agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion2, agentRadius,false,*randGen);
							}
						}
						break;

					case 4:
						//matched oncoming
						{
							if(i > 9) {
								agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(region, agentRadius, false, *randGen);
								agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(region, agentRadius,false,*randGen);
							}
							else if (i % 2)
							{
								agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion1, agentRadius, false, *randGen);
								agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion2, agentRadius,false,*randGen);
							}
							else
							{
								agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion5, agentRadius, false, *randGen);
								agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion1, agentRadius,false,*randGen);
							}
						}
						break;
					case 5:
						//winning-side oncoming
						{
							if(i < 3) {
								agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion1, agentRadius, false, *randGen);
								agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion2, agentRadius,false,*randGen);
							}
							else if(i > 9) {
								agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(region, agentRadius, false, *randGen);
								agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(region, agentRadius,false,*randGen);
							}
							else {
								agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion5, agentRadius, false, *randGen);
								agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion1, agentRadius,false,*randGen);
							}
						}
						break;

					case 6:
						//light crossing
					case 7:
						//medium crossing
					case 8:
						//heavy crossing
						{
							if(i > 9) {
								agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(region, agentRadius, false, *randGen);
								agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(region, agentRadius,false,*randGen);
							}
							else if(_currentScenario % 2) {
								agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion4, agentRadius, false, *randGen);
								agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion3, agentRadius,false,*randGen);
							}
							else {
								agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion3, agentRadius, false, *randGen);
								agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion4, agentRadius,false,*randGen);
							}
						}
						break;

					case 9:
						//matched crossing
						{
							if(i > 9) {
								agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(region, agentRadius, false, *randGen);
								agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(region, agentRadius,false,*randGen);
							}
							else if(_currentScenario % 2) {
								if (i % 2)
								{
									agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion4, agentRadius, false, *randGen);
									agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion3, agentRadius,false,*randGen);
								}
								else
								{
									agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion5, agentRadius, false, *randGen);
									agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion1, agentRadius,false,*randGen);
								}
							}
							else {
								if (i % 2)
								{
									agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion3, agentRadius, false, *randGen);
									agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion4, agentRadius,false,*randGen);
								}
								else
								{
									agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion5, agentRadius, false, *randGen);
									agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion1, agentRadius,false,*randGen);
								}
							}
						}
						break;
					case 10:
						//winning-side crossing
						{
							if(i < 3) {
								if(_currentScenario % 2) {
									agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion4, agentRadius, false, *randGen);
									agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion3, agentRadius,false,*randGen);
								}
								else {
									agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion3, agentRadius, false, *randGen);
									agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion4, agentRadius,false,*randGen);
								}
							}
							else if(i > 9) {
								agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(region, agentRadius, false, *randGen);
								agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(region, agentRadius,false,*randGen);
							}
							else {
								agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion5, agentRadius, false, *randGen);
								agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion1, agentRadius,false,*randGen);
							}
						}
						break;

					case 11:
						if ( i < numAgents/4)
						{
							agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion6, agentRadius, false, *randGen);
							agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion2, agentRadius,false,*randGen);
						}
						else if ( i < numAgents/2)
						{
							agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion6, agentRadius, false, *randGen);
							agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion1, agentRadius,false,*randGen);
						}
						else if ( i < 3* numAgents/4)
						{
							agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion6, agentRadius, false, *randGen);
							agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion4, agentRadius,false,*randGen);
						}
						else
						{
							agentPosition = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion6, agentRadius, false, *randGen);
							agentGoal.targetLocation = gEngine->getSpatialDatabase()->randomPositionInRegionWithoutCollisions(groupRegion3, agentRadius,false,*randGen);
						}
						break;
					case 12:
					case 13:
					default:
						//won't actually get here because an exception would've been thrown at the beginning of the program
						break;
					}
				}
				break;

			case 2: // placing agents regularly along the circumference of the circle with diametrically opposite goals


				double degreeIncrement = 360.0 / numAgents;
				double radIncrement = degreeIncrement * 0.0174532925;
				double thetaRad = 0.0;

				thetaRad = radIncrement * i;

				agentPosition.x = _radius * cos(thetaRad);
				agentPosition.y = 0.0f;
				agentPosition.z = _radius * sin(thetaRad);

				/*
				this was the previous code which was randomly placing agents along circumference

				bool collides = false;
				do
				{
					thetaRad = randGen->rand(M_2_PI);
					agentPosition.x = _radius * cos(thetaRad);
					agentPosition.y = 0.0f;
					agentPosition.z = _radius * sin(thetaRad);

					// check if no collision
					unsigned int j=0;
					for (; j < _agents.size(); j++)
						if (Util::circleOverlapsCircle2D(agentPosition,0.5f,_agents[j].position,0.5f))
						{
							collides = true;
							break;
						}

					if (j == _agents.size()) collides = false;

				}
				while (collides == true);
				*/



				// agent goal
				double goalThetaRad = thetaRad + M_PI;
				if (goalThetaRad > M_2_PI)
					goalThetaRad = goalThetaRad - M_2_PI;

				agentGoal.targetLocation = Util::Point(_radius * cos(goalThetaRad), 0.0f, _radius * sin(goalThetaRad));
				break;


			}



			//double x = cos(theta)*radius;
			//double y = sin(theta)*radius;


			//theta = randGen.rand() *2*PI;
			//radius = randGen.rand() *_radius;

			//printf("goalPosition = (%f, %f, %f)\n", goalPosition.x, goalPosition.y, goalPosition.z);
			//double goalX = cos(theta)*radius;
			//double goalY = sin(theta)*radius;

			ai.position = agentPosition;

			ai.radius = agentRadius; // This initializes the agents radius, could be dangerous
			ai.speed = 0.0f;


			// randomly generated speed
			if ( _fixedSpeed == false )
			{
				//double speed = rand()/((double)RAND_MAX)*((double)_maxSpeed-_minSpeed) + _minSpeed;
				double speed = randGen->rand() * ((double)_maxSpeed-_minSpeed) + _minSpeed;
				agentGoal.desiredSpeed = speed;
			}
			else
			{
				agentGoal.desiredSpeed = DESIRED_SPEED;
			}



			if ( _fixedDirection == false )
			{
				// randomly generated direction
				if (foundGoodAgent)
				{
					// agentPosition = dummyAgent->position();
					agentForward = dummyAgent->forward();
					ai.direction = agentForward;
				}
				else
				{
					theta = randGen->rand() * M_2_PI;

					double directionX = cos(theta);
					double directionY = sin(theta);
					ai.direction = Util::Vector(directionX, 0.0f, directionY);
				}
			}
			else
			{
				std::cout << "fixed direction is true" << std::endl;
				// we just want it to face its goal
				Util::Vector direction = agentGoal.targetLocation - ai.position;
				direction = Util::normalize(direction);
				ai.direction = direction;

			}

			ai.goals.clear ();
			ai.goals.push_back(agentGoal);
			#ifdef _DEBUG1
			std::cout << "max count " << maxCount << " " << i << "\n";
			#endif
			maxCount ++;

			Util::Point intersectionPoint2 = Util::Point(0, 0, 0);
			// std::cout << "i: " << i << std::endl;

			/*
			 * At this point all of the logic needed to validate the scenario wrt the scenario desired
			 * should be computed.
			 */
			if (_agents.size() > 0 && _checkAgentRelevant) // egocentric agents need to already exsist.
			{
				/*
				 * This is ambiguity of whether or now the true path line should be used
				 * 1. Having it the full length induces the trend of many agents having the same goal
				 * 2. Having it a little shorter to avoid one makes it impossible for any other agent to have the
				 * same goal as the reference agent.
				 * It is decided to be the least biased the algorithm will randomely choose which one it will be.
				 */
				SteerLib::AgentInitialConditions refernceAgent = _agents.at(0);

				float fullLineLengthOrNot = 0;
				if ( randGen->rand() > 0.5f)
				{
					fullLineLengthOrNot = 1.0f;
				}
				else
				{
					fullLineLengthOrNot = 0.99f;
				}
				intersectionPoint2 = Util::Point(0,0,0);

				agentIntersects = Util::intersect2Lines2D(ai.position, ai.position +
						((agentGoal.targetLocation - ai.position)*fullLineLengthOrNot),
						_agents.at(0).position, _agents.at(0).position +
						(_agents.at(0).goals.at(_agents.at(0).goals.size()-1).targetLocation - _agents.at(0).position),
						intersectionPoint2);
				/*std::cout << "Calculated intersection Point: " << intersectionPoint2 << std::endl;
				std::cout << "The ai position is: " << ai.position << " and goal is: " <<
						agentGoal.targetLocation << std::endl;
				std::cout << "The reference agent position is: " << _agents.at(0).position << " and goal is: " <<
						_agents.at(0).goals.at(_agents.at(0).goals.size()-1).targetLocation << std::endl;
	*/
				if ( !agentIntersects )
				{
					solidAgent = solidAgent && false;
					// At this point we know the Agent is not valid.
					continue;
				}
				else
				{
					if ( _checkAgentInteraction )
					{
						// Here we check to see if we have an expected interaction.

						// Check if agent interacts with reference agent
						// Compute the time it will take for the reference agent to reach the intersection point

						float ref_intersect_time = (intersectionPoint2 - refernceAgent.position).length()/refernceAgent.goals.front().desiredSpeed;
						float other_intersect_time = (intersectionPoint2 - ai.position).length() / ai.goals.front().desiredSpeed;

						// Compute the point the other agent will be at when the reference agent is supposed to be at the
						// intersection point.
						Util::Point other_agent_point_at_ref_time = ai.position +
								((normalize(intersectionPoint2 - ai.position) * ai.goals.front().desiredSpeed) // velocity vector
								* ref_intersect_time);

						// Compute the point the reference agent will be at when the other agent is supposed to be at the
						// intersection point.
						Util::Point ref_agent_point_at_other_time = refernceAgent.position +
							((normalize(intersectionPoint2 - refernceAgent.position) * refernceAgent.goals.front().desiredSpeed) // velocity vector
							* other_intersect_time);

						// Check if either of these two points are withing the interaction Radius


						if ( ((ref_agent_point_at_other_time - intersectionPoint2).length() < _checkAgentInteractionRadius) ||
								(other_agent_point_at_ref_time - intersectionPoint2).length() < _checkAgentInteractionRadius )
						{
							solidAgent = solidAgent && true;
						}
						else
						{
							solidAgent = solidAgent && false;
							continue;
						}

						/*
						std::cout << "current intersection point: " << intersectionPoint2 << std::endl;
						std::cout << "reference Agent position: " << ref_agent_point_at_other_time << std::endl;
						std::cout << "reference Agent desired speed: " << refernceAgent.goals.front().desiredSpeed << std::endl;
						std::cout << "other Agent position: " << other_agent_point_at_ref_time << std::endl;
						std::cout << "other Agent desired speed: " << ai.goals.front().desiredSpeed << std::endl;
						*/

					}
					else
					{
						solidAgent = solidAgent && true;
					}
				}

			}

			if (_checkAgentValid)
			{
				solidAgent = solidAgent  && (isAgentValid(ai, agentPath));
				if (!solidAgent)
				{
					continue;
				}
			}

			if (_checkAgentRelevant && i > 0)
			{
				_refAgentIntersectPoints.push_back(intersectionPoint2);
				// std::cout << ai.name << " position is " << ai.position << " for ref intersections" << std::endl;
				// std::cout << ai.name << " goal is " << agentGoal.targetLocation << " for ref intersections" << std::endl;
			}

			// std::cout << "i2: " << i << std::endl;
		}
		while ( !solidAgent );
		// while ( !solidAgent && (maxCount < 110) );
		// while( !agentIntersects && i != 0 && maxCount < 100); // This needs a bound to keep from
			//note i disabled most relevancy checking
		/*
		 * while(((_checkAgentValid || _checkAgentRelevant) && isAgentValid(ai,agentPath) == false)  &&
			 ((maxCount < 10) ||
			 (i != 0 && (contextNum == 0 && _checkAgentRelevant && useContext && isAgentRelevant(ai,agentPath)) ) ) ); // we don't want relevant agents here??
		 */

		agentPaths.push_back(agentPath);

		_agents.push_back(ai);

		_agentGoals.push_back(ai.goals.at(0).targetLocation);
		_agentInitialDisks.push_back(Util::Circle(ai.position.x, ai.position.z, ai.radius));
		_agentInitialForwardDirections.push_back(ai.direction);

		const SteerLib::AgentInitialConditions & ic = _agents[_agents.size()-1];
		AgentInterface * agent = gEngine->createAgent( ic, _aiModule );
		agent->reset(ic, gEngine);


		 // Could not hold this assertion because there can be issues placing agents
		// to meet all of the conditions.
		if (_checkAgentRelevant && false)
		{
			/*
			 * If we are checking for agent path intersection there should be
			 * one less intersection then there are agents.
			 */
			if ( !(_refAgentIntersectPoints.size() == (_agents.size() - 1)) )
			{
				std::cout << "Improper number of agent intersection points." << std::endl;
				std::cout << "Should be " << (_agents.size() - 1);
				// std::cout << "Intersection points: " << _repAgentIntersectPoints << " agents: " << _agents.size() << std::endl;
				std::cout << " is " << _refAgentIntersectPoints.size() << std::endl;
			}
			assert( _refAgentIntersectPoints.size() == (_agents.size() - 1));
		}

		//Util::AxisAlignedBox newBounds(x-agentRadius, x+agentRadius, 0.0f, 0.0f, y-agentRadius, y+agentRadius);
		//grid.addObject( dynamic_cast<SteerLib::SpatialDatabaseItemPtr>(ai), newBounds);
	}
	//printf("HERE\n");
	//loadCurrentScenario();
	if (_checkAgentRelevant )
	{
		// computes all agent path intersections
		Util::Point intersectPoint;
		bool agentsIntersect = false;
		for ( int g = 0; g < _agents.size(); g++)
		{
			// std::cout << _agents.at(g).name << " position is " << _agents.at(g).position << " for all intersections" << std::endl;
			// std::cout << _agents.at(g).name << " goal is " << _agents.at(g).goals.at(0).targetLocation << " for all intersections" << std::endl;
			for (int t = g + 1; t < _agents.size(); t++ )
			{
				agentsIntersect = Util::intersect2Lines2D(_agents.at(g).position, _agents.at(g).position +
												((_agents.at(g).goals.at(0).targetLocation - _agents.at(g).position)),
												_agents.at(t).position, _agents.at(t).position +
												(_agents.at(t).goals.at(0).targetLocation - _agents.at(t).position),
												intersectPoint);
				if ( agentsIntersect )
				{
					_agentIntersectionPoints.push_back(intersectPoint);
					intersectPoint = Util::Point(0,0,0);
				}
			}
		}
	}


	#ifdef _DEBUG1
	std::cerr << "scenario generated.  \n";
	#endif

}


void 
ScenarioModule::setRandomNumberGenerator (long seed, long long numberOfRandCalls )
{


	randGen = new MTRand (seed);

	for(long long i=1; i <= numberOfRandCalls; i++)
		randGen->rand();

	//generateRandomScenario();

}


void ScenarioModule::finish()
{
	// delete randGen;
}


void 
ScenarioModule::initializeSimulation()
{
	std::cout << "scenarioModule initializeSimulation " << std::endl;
}

void 
ScenarioModule::cleanupSimulation()
{
}


void 
ScenarioModule::preprocessSimulation()
{
	if ( _currentScenario < _numScenarios )	//this is impossible unless -1 passed from the command line...was this for DEBUG1ging?
	{
		if( runScenarioSet )
		{
			loadScenario(scenarioSetPath, _currentScenario);
		}
		else if (_subspace != NULL)
		{
			generateNextSubSpace();
		}
		else
		{
			generateScenario(_currentScenario);
		}
		//loadCurrentScenario ();
		gEngine->getPathPlanner()->refresh();
	}
}


void 
ScenarioModule::postprocessSimulation()
{
}


void 
ScenarioModule::preprocessFrame(float timeStamp, float dt, unsigned int frameNumber)
{
	if (_dumpFirstFrame)
	{
		if ( frameNumber == 2 ) 
		{
			// we need to generate new base name 
			std::string newBaseName;
			std::stringstream out;
			out << _frameBaseName;
			out << _currentScenario;
			newBaseName = out.str();
			_frameSaver->baseName = newBaseName;

			_frameSaver->DumpPPM(windowSizeX,windowSizeY);
		}
	}
	if (_dumpFrames)	
	{
		// here, we should frame for each scenario in a seperate folder 
		// we need to generate a new file path for each scenario 

		std::string newFilePath;
		std::stringstream out;
		if ( strcmp(_frameDumpRelativePath.c_str(), Empty_String) == 0)
		{
			out << "scenario" << _currentScenario + scenarioSetInitId << "/";
		}
		else
		{
			out << _frameDumpRelativePath;
			out << "/scenario" << _currentScenario + scenarioSetInitId << "/";
		}
		newFilePath = out.str();
		_frameSaver->filePath = newFilePath;

		_frameSaver->DumpPPM(windowSizeX,windowSizeY);

	}
}


void 
ScenarioModule::postprocessFrame(float timeStamp, float dt, unsigned int frameNumber)
{
	// std::cout << "ScenarioModule::postprocessFrame" << std::endl;
	// checking to see if all scenarios have been processed
	if (_currentScenario == _numScenarios)
	{
		// printf("} returning 1\n");
		return;
	}


	unsigned int numAgentsDisabled = 0;
#ifdef _DEBUG1
	std::cout << "disabling agents " << numAgentsDisabled << "\n";
	std::cout << "Max Number of frames is " << _maxNumFrames << "\n";
	std::cout << "On frame " << frameNumber << "\n";
#endif
	for (unsigned int i=0; i < gEngine->getAgents().size(); i++)
	{
		if ( gEngine->getAgents()[i]->enabled() == false)
			numAgentsDisabled ++;
#ifdef _DEBUG1
	std::cout << "number of agents disabled " << numAgentsDisabled << "\n";
#endif
	}
#ifdef _DEBUG1
	if (frameNumber % 25 == 0)
	{
		std::cout << "Finished frame " << frameNumber << " time thus far: " << gEngine->getClock().getCurrentRealTime()
				<< std::endl;

	}
#endif
	// this checks to see if the ai module has finished simulating all its agents. 
	// This logic in here is a little dangerous this means that the scenario can end and still have
	// non disabled agents still in the gSpatialDatabase
	if ( _recordingScenariosOnly || numAgentsDisabled == gEngine->getAgents().size() ||
			(int) frameNumber > _maxNumFrames || gEngine->getClock().getCurrentRealTime() > _scenarioTimeThreshold)
	{
		// the ai module has finished simulating
		#ifdef _DEBUG1
		std::cout << " finished simulating ... \n";
		#endif

		if ( gEngine->getClock().getCurrentRealTime() > _scenarioTimeThreshold )
		{
			std::cout << "Scenario execution time maxed out" << std::endl;
		}

		if (_currentScenario % 25 == 0)
		{
			std::cout << "Finished scenario " << _currentScenario + scenarioSetInitId << std::endl;
		}

		if ( _runFailureSet )
		{
			_currentFailureSetIndex ++; // this should always be less than the size
			assert(_currentFailureSetIndex < _failureSet.size());

			long long numRandomCallsToMake = _failureSet[_currentFailureSetIndex] - _failureSet[_currentFailureSetIndex-1];
			incrementRandomNumberGenerator(numRandomCallsToMake);
		}

		// report steerbench analysis -- disabling output on console for now 
		//if ( _useBenchmark )
		//	_benchmarkModule->reportBenchmarkResults();

		if( _recordingScenariosOnly )
		{
			std::ofstream recordScenarioStream ;
			recordScenarioStream.open(_recordScenarioFile.c_str(), std::fstream::out | std::fstream::app) ;
			if( recordScenarioStream.fail()) 
			{
				std::cerr << "Cannot open " << _recordScenarioFile << "\n";
				exit(-1) ;
			}
			recordScenario(recordScenarioStream) ;
			recordScenarioStream.close() ;
		}
		if ( _useBenchmark )
		{
			int success = (int) frameNumber < _maxNumFrames; // 1 - succeed, 0 - fail
			// TODO COME UP WITH SMARTER MEASURE OF SUCCESS 
			// SCENARIO COMPLETED + NO COLLISIONS 

			LogObject benchmarkLogObject;
			benchmarkLogObject.addLogData((int)_currentScenario + (int)scenarioSetInitId);

			// Add the number of frameS it took to complete
			benchmarkLogObject.addLogData( (int) frameNumber);

			benchmarkLogObject.addLogData(randGen->getNumberOfRandCalls () - _currentNumberOfRandomCalls);
			// _currentNumberOfRandomCalls is really equal to the number of calls at the beginning of this scenario

			// COMPLETE SIMULATION 
#ifdef _DEBUG1
			std::cerr << " computing total scores .. \n";
#endif
			// collisions, time, effort, and accceleration (for composite02)
			std::vector<float> scores;
			scores.clear();

			float totalScore = _benchmarkModule->getTotalBenchmarkScore();

			_benchmarkModule->getBenchmarkTechnique()->getScores(scores);

			for(unsigned int i=0; i < scores.size(); i++)
				benchmarkLogObject.addLogData(scores[i]);

			float numCollisions = scores[0];

			benchmarkLogObject.addLogData(totalScore);

			// DID ENTIRE SCENARIO COMPLETE ? 
			benchmarkLogObject.addLogData(success);

			// EGOCENTRIC AGENT 
#ifdef _DEBUG1
			std::cerr << " computing egocentric agent scores .. \n";
#endif
			scores.clear ();
			_benchmarkModule->getAgentScore(0,scores);

			for(unsigned int i=0; i < scores.size(); i++)
				benchmarkLogObject.addLogData(scores[i]);
#ifdef _DEBUG1
std::cout << " before agent success... \n";
#endif
			// agent success 
			int egocentricAgentReachedGoal = gEngine->getAgents()[0]->enabled() == false ? 1 : 0;
			float numCollisionsForEgocentricAgent = scores[0];

			int egocentricAgentSuccess;
			if ( egocentricAgentReachedGoal == 1 && numCollisionsForEgocentricAgent < 1.0f && numCollisions < gEngine->getAgents().size() ) 
			{
			    egocentricAgentSuccess = 1;
			}
			else
			{
			    egocentricAgentSuccess = 0;
			    /*
			  if (!(egocentricAgentReachedGoal == 1))
			  {
			    egocentricAgentSuccess = egocentricAgentSuccess + -1;
			  }
			  if (!(numCollisionsForEgocentricAgent < 1.0f))
			  {
			    egocentricAgentSuccess = egocentricAgentSuccess + -10;
			  }
			  if (!(numCollisions < gEngine->getAgents().size()))
			  {
			    egocentricAgentSuccess = egocentricAgentSuccess + -100;
			  }
			  */
			}

			benchmarkLogObject.addLogData(egocentricAgentReachedGoal);
			benchmarkLogObject.addLogData(egocentricAgentSuccess);

			// optimal values distance 
			optimalPathLength = 0.0f;
			//optimalPathLength = Util::distanceBetween(agentPaths[0][0],agentPaths[0][agentPaths[0].size()-1]) - 0.5f;
			//optimalPathLength = 1.0f; 
#ifdef _DEBUG1
std::cout << " before optimal path length ... \n";
#endif
			for (unsigned int i=1; i < agentPaths[0].size();i=i+1)
				optimalPathLength += Util::distanceBetween(agentPaths[0][i-1],agentPaths[0][i]);

			optimalPathLength -= 2.0f;

#ifdef _DEBUG1
std::cout << " after optimal path length \n";
#endif
			// percentage to optimal distance 
			// TODO something about this seems fishy
			float totalDistanceTraveled = scores[3];
			float diff = totalDistanceTraveled - optimalPathLength;
			if (diff < 0.0f) diff = 0.0f; // in case it is less than optimal
			float diffRatio = diff/optimalPathLength;
			if ( diffRatio > 1.0f ) diffRatio = 1.0f; 

			float ratioToOptimalPathLength = (1.0f - diffRatio); 


			benchmarkLogObject.addLogData(optimalPathLength);
			benchmarkLogObject.addLogData(ratioToOptimalPathLength);


			// optimal time 
			float optimalTime = optimalPathLength / DESIRED_SPEED;

			// percentage to optimal distance 
			float totalTime = scores[1];
			float diffT = totalTime - optimalTime;
			if (diffT < 0.0f) diffT = 0.0f; // in case it is less than optimal
			float diffTRatio = diffT/optimalTime;
			if ( diffTRatio > 1.0f ) diffTRatio = 1.0f; 

			float ratioToOptimalTime = (1.0f - diffTRatio); 

			benchmarkLogObject.addLogData(optimalTime);
			benchmarkLogObject.addLogData(ratioToOptimalTime);

			float pathLength = 0.0f;
			std::vector<float> pathLengths;
			for (unsigned int a=0; a < agentPaths.size(); a++)
			{
				for (unsigned int i=1; i < agentPaths[a].size();i=i+1)
				{
					pathLength += Util::distanceBetween(agentPaths[a][i-1],agentPaths[a][i]);
				}
				pathLength -= 2.0f;
				pathLengths.push_back(pathLength);
				pathLength = 0.0f;
			}

			DataItem thing;
			thing.string = constructBenchDataString(pathLengths);
			benchmarkLogObject.addLogDataItem(thing);

			std::vector<float> agent_metrics;
			std::vector<float> agent_times;
			std::vector<float> agent_distances;
			std::vector<int> agent_complete;
			std::vector<int> agent_successes;
			int agent_succ = 0;
			for (unsigned int a=0; a < gEngine->getAgents().size(); a++)
			{
				_benchmarkModule->getAgentScore(a,agent_metrics);
				agent_times.push_back(agent_metrics.at(1));
				agent_distances.push_back(agent_metrics.at(3));
				agent_complete.push_back( gEngine->getAgents()[a]->enabled() == false ? 1 : 0);
				if ( (agent_complete.at(a) == 1) && (agent_metrics.at(0) < 1.0f) )
				{ // complete and no collisions
					agent_successes.push_back(1);
				}
				else
				{
					agent_successes.push_back(0);
				}
				/*
				if ( agent_metrics.at(0) > 0.0)
				{
					std::cout << "agent " << a << " complete: " << agent_complete.at(a) << std::endl;
					std::cout << "agent " << a << " succes: " << agent_successes.at(a) << std::endl;
					std::cout << "agent " << a << " collisions: " << agent_metrics.at(0) << std::endl;
				}
				*/
				agent_metrics.clear();
			}

			DataItem times_thing;
			times_thing.string = constructBenchDataString(agent_times);
			benchmarkLogObject.addLogDataItem(times_thing);

			DataItem distances_thing;
			distances_thing.string = constructBenchDataString(agent_distances);
			benchmarkLogObject.addLogDataItem(distances_thing);

			DataItem completes_thing;
			completes_thing.string = constructBenchDataString(agent_complete);
			benchmarkLogObject.addLogDataItem(completes_thing);

			DataItem succeses_thing;
			succeses_thing.string = constructBenchDataString(agent_successes);
			benchmarkLogObject.addLogDataItem(succeses_thing);


			if ( _benchmarkTechnique == "compositeGraph" )
			{


				std::stringstream polygon_string;
				polygon_string << "(";
				for (int p=0; p < _agentIntersectionPoints.size(); p++)
				{
					polygon_string << "(" << _agentIntersectionPoints.at(p).x << "," <<
							_agentIntersectionPoints.at(p).z << ")";
					if ( p != (_agentIntersectionPoints.size() - 1))
					{
						polygon_string << ",";
					}
				}
				polygon_string << ")";

#ifdef _DEBUG1
				std::cout << "agent intersection points: " << polygon_string.str() << std::endl;
#endif
				DataItem thing;
				thing.string = polygon_string.str();
				benchmarkLogObject.addLogDataItem( thing);

				std::stringstream refAgentIntersect_str;
				refAgentIntersect_str << "(";
				for (int p=0; p < _refAgentIntersectPoints.size(); p++)
				{
					refAgentIntersect_str << "(" << _refAgentIntersectPoints.at(p).x << "," <<
							_refAgentIntersectPoints.at(p).z << ")";
					if ( p != (_refAgentIntersectPoints.size() - 1))
					{
						refAgentIntersect_str << ",";
					}
				}
				refAgentIntersect_str << ")";

#ifdef _DEBUG1
				std::cout << "Reference agent intersection points: " << refAgentIntersect_str.str() << std::endl;
#endif
				DataItem thing2;
				thing2.string = refAgentIntersect_str.str();
				benchmarkLogObject.addLogDataItem( thing2);

				std::stringstream box_obstacle_string;
				// box_obstacle_string << "{";
				// std::vector<SteerLib::BoxObstacle> tmpsticles(gEngine->getObstacles().begin(),
					// 		gEngine->getObstacles().end());
				if ( _obstacles.size() == 0 )
				{
					DataItem goals_Item;
					goals_Item.string = "null";
					benchmarkLogObject.addLogDataItem(goals_Item);
				}
				else
				{
					for (int p=0; p < _obstacles.size(); p++)
					{
						// Create a polygon string for the postgres database describing the
						// BoxObstacle
						box_obstacle_string << "(";
						box_obstacle_string << "(" << _obstacles.at(p)->createObstacle()->getBounds().xmin << "," <<
													_obstacles.at(p)->createObstacle()->getBounds().zmin << "),";
						box_obstacle_string << "(" << _obstacles.at(p)->createObstacle()->getBounds().xmin << "," <<
													_obstacles.at(p)->createObstacle()->getBounds().zmax << "),";
						box_obstacle_string << "(" << _obstacles.at(p)->createObstacle()->getBounds().xmax << "," <<
													_obstacles.at(p)->createObstacle()->getBounds().zmax << "),";
						box_obstacle_string << "(" << _obstacles.at(p)->createObstacle()->getBounds().xmax << "," <<
													_obstacles.at(p)->createObstacle()->getBounds().zmin << "),";
						box_obstacle_string << "(" << _obstacles.at(p)->createObstacle()->getBounds().xmin << "," <<
													_obstacles.at(p)->createObstacle()->getBounds().zmin << ")";
						box_obstacle_string << ")";
						if ( p != (_obstacles.size() - 1))
						{
							box_obstacle_string << ";";
						}
					}
					// box_obstacle_string << "}";

	#ifdef _DEBUG1
					std::cout << "Box Obstacle polygons: " << box_obstacle_string.str() << std::endl;
	#endif
					DataItem box_obstacles;
					box_obstacles.string = box_obstacle_string.str();
					benchmarkLogObject.addLogDataItem( box_obstacles);
				}

				std::stringstream agent_goals;
				// agent_goals << "(";
				if ( _agentGoals.size() == 0 )
				{
					DataItem goals_Item;
					goals_Item.string = "null";
					benchmarkLogObject.addLogDataItem(goals_Item);
				}
				else
				{
					for (int p=0; p < _agentGoals.size(); p++)
					{
						agent_goals << "(" << _agentGoals.at(p).x << "," <<
								_agentGoals.at(p).z << ")";
						if ( p != (_agentGoals.size() - 1))
						{
							agent_goals << ";";
						}
					}
					// agent_goals << ")";

	#ifdef _DEBUG1
					std::cout << "agent Goals points: " << agent_goals.str() << std::endl;
	#endif
					DataItem goals_Item;
					goals_Item.string = agent_goals.str();
					benchmarkLogObject.addLogDataItem( goals_Item);
				}

				std::stringstream agent_initial_disks;
				// agent_initial_disks << "(";
				if ( _agentInitialDisks.size() == 0 )
				{
					DataItem goals_Item2;
					goals_Item2.string = "null";
					benchmarkLogObject.addLogDataItem(goals_Item2);
				}
				else
				{
					for (int p=0; p < _agentInitialDisks.size(); p++)
					{
						agent_initial_disks << "((" << _agentInitialDisks.at(p).x << "," <<
								_agentInitialDisks.at(p).z << ")" << _agentInitialDisks.at(p).Radius() << ")";
						if ( p != (_agentInitialDisks.size() - 1))
						{
							agent_initial_disks << ";";
						}
					}

	#ifdef _DEBUG1
					std::cout << "agent intial disks: " << agent_initial_disks.str() << std::endl;
	#endif
					DataItem goals_Item2;
					goals_Item2.string = agent_initial_disks.str();
					benchmarkLogObject.addLogDataItem( goals_Item2);
				}
				// agent_initial_disks << ")";

				std::stringstream agent_initial_forward_direction;
				// agent_initial_forward_direction << "(";
				if ( _agentInitialForwardDirections.size() == 0 )
				{
					DataItem goals_Item3;
					goals_Item3.string = "null";
					benchmarkLogObject.addLogDataItem( goals_Item3);
				}
				else
				{
					for (int p=0; p < _agentInitialForwardDirections.size(); p++)
					{
						agent_initial_forward_direction << "(" << _agentInitialForwardDirections.at(p).x << "," <<
								_agentInitialForwardDirections.at(p).z << ")" ;
						if ( p != (_agentInitialForwardDirections.size() - 1))
						{
							agent_initial_forward_direction << ";";
						}
					}
	#ifdef _DEBUG1
					std::cout << "agent initial forward direction: " << agent_initial_disks.str() << std::endl;
	#endif
					DataItem goals_Item3;
					goals_Item3.string = agent_initial_forward_direction.str();
					benchmarkLogObject.addLogDataItem( goals_Item3);
				}
				// agent_initial_forward_direction << ")";

				std::stringstream agent_Paths;
				// agent_initial_forward_direction << "(";
				if ( agentPaths.size() == 0 )
				{
					DataItem string_data;
					string_data.string = "null";
					benchmarkLogObject.addLogDataItem( string_data);
				}
				else
				{
					for (int p=0; p < agentPaths.size(); p++)
					{
						agent_Paths << "(";
						for (int p2 = 0; p2 < agentPaths.at(p).size(); p2++)
						{
							agent_Paths << "(" << agentPaths.at(p).at(p2).x << "," <<
									agentPaths.at(p).at(p2).z << ")" ;
							if ( p2 != (agentPaths.at(p).size() - 1))
							{
								agent_Paths << ",";
							}
						}
						agent_Paths << ")";
						if ( p != ( agentPaths.size() - 1))
						{
							agent_Paths << ";";
						}
					}
	#ifdef _DEBUG1
					std::cout << "agent initial forward direction: " << agent_initial_disks.str() << std::endl;
	#endif
					DataItem goals_Item3;
					goals_Item3.string = agent_Paths.str();
					// std::cout << agent_Paths.str() << std::endl;
					benchmarkLogObject.addLogDataItem( goals_Item3);
				}
#ifdef _DEBUG1
				std::cout << "The egocentric path is: " << agentPaths.at(0).at(1) << std::endl;
				std::cout << "There are " << agentPaths.size() << " agent paths " << std::endl;
#endif

				DataItem static_interaction;
				static_interaction.string = constructBenchDataString(_agentStaticPathIntersections);
				benchmarkLogObject.addLogDataItem( static_interaction);

				DataItem static_interaction2;
				static_interaction2.string = constructBenchDataString(_refAgentStaticPathIntersections);
				benchmarkLogObject.addLogDataItem( static_interaction2);
			}

			/*
			float v = 0.0f;
			float a = 3.0f;
			float optimalAvgKE = 0.0f;
			float optimalIntegralKE = 0.0f;
			for (float t = 0.0f ; t < optimalTime; t = t + dt)
			{
				float instantKE = 0.5f * v * v;
				optimalIntegralKE += 2.0f * instantKE *  dt;

				v = v + a * dt;
				if ( v > DESIRED_SPEED ) v = DESIRED_SPEED;

			}

			optimalAvgKE = optimalIntegralKE / optimalTime * 0.5f;

			float avgKE = scores[scores.size()-1];

			benchmarkLogObject.addLogData(optimalAvgKE);
			benchmarkLogObject.addLogData(avgKE);
			*/


			if ( _benchmarkTechnique == "compositePLE" )
			{

				// MUBBASIR PLE ENERGY -- CURRENTLY THIS IS ONLY FOR EGOCENTRIC AGENT -- WE NEED THIS TO BE FOR ALL AGENTS
				float pleEnergy = scores[9]; // updated because I added another field
				benchmarkLogObject.addLogData(pleEnergy);

				// computing pleEnergyOptimal = 2 m L sqrt (E_S * E_W )
				float pleEnergyOptimal = 2.0f * MASS * optimalPathLength * sqrt(E_S * E_W );
				benchmarkLogObject.addLogData(pleEnergyOptimal);

				float pleEnergyRatio = pleEnergyOptimal / pleEnergy;
				benchmarkLogObject.addLogData(pleEnergyRatio);

				#ifdef _DEBUG1
				std::cout << " ple calculations: " << numCollisions << " " << pleEnergy << " " << pleEnergyOptimal << " " << pleEnergyRatio << "\n";
				#endif
				agent_metrics.clear();
				std::vector<float> agent_ples;
				for (unsigned int a=0; a < gEngine->getAgents().size(); a++)
				{
					_benchmarkModule->getAgentScore(a,agent_metrics);
					agent_ples.push_back(agent_metrics.at(9));
					agent_metrics.clear();
				}

				DataItem ples_thing;
				ples_thing.string = constructBenchDataString(agent_ples);
				benchmarkLogObject.addLogDataItem(ples_thing);

			}
			else if (_benchmarkTechnique == "compositeEntropy" )
			{
				std::cout << "Entropy value? " << _benchmarkModule->getTotalBenchmarkScore() << std::endl;
			}

			#ifdef _DEBUG1
			std::cout << "logging benchmark data ... " << benchmarkLogObject.getRecordSize()  << " " << _benchmarkLogger->getNumberOfFields() << "\n";
			#endif

			// Testing out Pretty write for more human readable output
			if ( _prettyLog && ! _benchmarkLog.empty() )
			{
				_benchmarkLogger->writeLogObjectPretty(benchmarkLogObject);
			}
			else
			{
				if ( ! _benchmarkLog.empty())
				{
					_benchmarkLogger->writeLogObject(benchmarkLogObject);
				}

				// std::cout << _benchmarkLogger->logObjectToString(benchmarkLogObject) << std::endl;
				_data = _data + _benchmarkLogger->logObjectToString(benchmarkLogObject);
				// fresh pointers need to be saved so the data is not erased when this program is finished.
				_logData.push_back(benchmarkLogObject.copy());
			}
		}

		// clearScenario ();

		/*
		// in case we are using RVOPlusAI
		if ( _aiModuleName.compare("RVOPlusAI") == 0)
		{
			std::cout << "resetting rvo simulator\n";
			dynamic_cast<RVOPlusAIModule *>(_aiModule)->resetRVOSimulator ();
		}
		*/

#ifdef _DEBUG1
std::cout << "after clearing scenario \n";
#endif
		// resetting clock 
		gEngine->getClock().reset();

		_currentScenario++;

		// _frameSaver reset 
		if ( _dumpFrames )
		{
			_frameSaver->m_frameCount = 0;
		}

		// writing rec file
		if ( _recFileBaseName != "" )
		{
			_recorderModule->finishRecording();
		}


		if ( _currentScenario < _numScenarios)
		{
			#ifdef _DEBUG1
			std::cout << " about to generate scenario \n";
			#endif
			clearScenario();

			//loadCurrentScenario();

			_aiModule->cleanupSimulation();
#ifdef _DEBUG1
std::cout << "after aiModuile cleanup Scenario \n";
#endif

			if( runScenarioSet )
			{

				loadScenario(scenarioSetPath, _currentScenario);
			}
			else if (_subspace != NULL)
			{
				generateNextSubSpace();
			}
			else
			{
				generateScenario(_currentScenario);
			}

			// resetting benchmark module 
			if ( _useBenchmark ) 
				_benchmarkModule->resetBenchmarkModule ();

			// starting up a new rec file
			if ( _recFileBaseName != "" )
			{
				std::stringstream st1;
				if ( _recFileFolder != "" )
				{
					st1 << _recFileFolder << "/";
				}

				st1 << _recFileBaseName;
				st1 << ((int)_currentScenario + (int)scenarioSetInitId);
				st1 << ".rec";
				std::string tempS = st1.str();
				_recorderModule->initializeRecorder(tempS);
			}

			// refresh path planner due to environment change.
			// gEngine->getPathPlanner()->refresh();


		}
		else 
		{
			// we have just simulated last scenario
			#ifdef _DEBUG1
			std::cout << "\nAll Scenarios processed. \n";
			#endif
			_aiModule->cleanupSimulation(); // TODO this will cause a bug in steerstats where the last scenario will be skipped
			// perform module cleanup 
			if ( _useBenchmark )
			{
				// _benchmarkModule->cleanupSimulation ();

				if ( _useBenchmark && ! _benchmarkLog.empty() )
				{
					// cleaning up log data 
					_benchmarkLogger->closeLog();
				}
			}
			_aiModule->finish();
			//printf("} exiting 1\n");
			// exit(0);
			gEngine->stop();

		}
	}
	//printf("}\n");

}


std::string constructBenchDataString(std::vector<Util::Point> points)
{
	std::stringstream strout;
	if ( points.size() == 0 )
	{
		strout << "null";
	}
	else
	{
		strout << "(";
		for (int p=0; p < points.size(); p++)
		{
			strout << "(" << std::fixed << points.at(p).x << "," << std::fixed <<
					points.at(p).z << ")";
			if ( p != (points.size() - 1))
			{
				strout << ",";
			}
		}
		strout << ")";
	}
	return strout.str();
}

std::string constructBenchDataString(std::vector<int> points)
{
	std::stringstream strout;
	if ( points.size() == 0 )
	{
		strout << "null";
	}
	else
	{
		strout << "(";
		for (int p=0; p < points.size(); p++)
		{
			strout << std::fixed << points.at(p);
			if ( p != (points.size() - 1))
			{
				strout << ",";
			}
		}
		strout << ")";
	}
	return strout.str();
}

std::string constructBenchDataString(std::vector<float> points)
{
	std::stringstream strout;
	if ( points.size() == 0 )
	{
		strout << "null";
	}
	else
	{
		strout << "(";
		for (int p=0; p < points.size(); p++)
		{
			strout << std::fixed << points.at(p);
			if ( p != (points.size() - 1))
			{
				strout << ",";
			}
		}
		strout << ")";
	}
	return strout.str();
}

void ScenarioModule::clearScenario ()
{
	// cleaning up agents 


	unsigned int numAgentsDisabled = 0;
	for (unsigned int i=0; i < gEngine->getAgents().size(); i++)
	{
		if ( gEngine->getAgents()[i]->enabled() == false)
		{
			numAgentsDisabled ++;
		}
		else
		{
			gEngine->getAgents()[i]->disable() ;
			numAgentsDisabled ++;
		}
	}
	gEngine->destroyAllAgentsFromModule(_aiModule);

	// cleaning up obstacles
	/*
	std::set<SteerLib::SpatialDatabaseItemPtr> _neighbors;
	for (unsigned int i=0; i<_obstacles.size(); i++)
	{
		gEngine->getSpatialDatabase()->getItemsInRange(_neighbors,_obstacles[i]->createObstacle()->getBounds().xmin,
						_obstacles[i]->createObstacle()->getBounds().xmax, _obstacles[i]->createObstacle()->getBounds().zmin, _obstacles[i]->createObstacle()->getBounds().zmax, NULL);
		if ( _neighbors.size() > 0);
		{
			gEngine->getSpatialDatabase()->removeObject( _obstacles[i]->createObstacle(), _obstacles[i]->createObstacle()->getBounds());
		}
		gEngine->removeObstacle(_obstacles[i]->createObstacle());
	}
	 */
	std::set<SteerLib::SpatialDatabaseItemPtr> _neighbors;
	for (std::set<SteerLib::ObstacleInterface*>::iterator neighbor = gEngine->getObstacles().begin();  neighbor != gEngine->getObstacles().end();  neighbor++)
	{
		SteerLib::ObstacleInterface * obstacle;
		obstacle = dynamic_cast<SteerLib::ObstacleInterface *>(*neighbor);
		// gEngine->getSpatialDatabase()->getItemsInRange(_neighbors, obstacle->getBounds().xmin,
			// 	obstacle->getBounds().xmax, obstacle->getBounds().zmin, obstacle->getBounds().zmax, NULL);

		// for (std::set<SteerLib::SpatialDatabaseItemPtr>::iterator neighbor = _neighbors.begin();  neighbor != _neighbors.end();  neighbor++)
		gEngine->getSpatialDatabase()->removeObject( obstacle, obstacle->getBounds());
	}
	gEngine->removeAllObstacles(); // remove the obstacles from the simulation
	gEngine->getSpatialDatabase()->clearDatabase();
	// Clear data structures.
	_obstacles.clear ();
	_agents.clear ();
	agentPaths.clear();
	_refAgentIntersectPoints.clear();
	_agentIntersectionPoints.clear();
	_agentGoals.clear();
	_agentInitialDisks.clear();
	_agentInitialForwardDirections.clear();

	clearGraphData();
}

void 
ScenarioModule::processKeyboardInput(int key, int action )
{
	if ( key == 83  && (action==KEY_PRESSED) ) // s
	{
		#ifdef _DEBUG1
		std::cout << "Dumping scenario " << _randomSeed << " " << _currentNumberOfRandomCalls << "\n";
		#endif
		_frameSaver->DumpPPM(windowSizeX,windowSizeY);
	}
	if ( key == 46  && (action==KEY_PRESSED) ) // .
	{
		for (unsigned int i=0; i < gEngine->getAgents().size(); i++)
		{
			if ( gEngine->getAgents()[i]->enabled() == true )
			{
				gEngine->getAgents()[i]->disable() ;
			}
		}
	}

}


/// Records the header for all scenarios
void 
ScenarioModule::recordHeaderForScenarios(std::fstream &outStream) 
{
	if( gEngine == NULL ) 
	{
		std::cerr << "gEngine is null\n" ;
		return ;
	}
	
	outStream << "ScenarioId\tRandCalls\tRandomSeed" ;
	outStream << "\tNumAgents\tNumOsctacles" ;
	for( int i = 0 ; i < (int) _agents.size() ; i++)
	{
		outStream << "\t" << "Agent[" << i << "]-PosX" << "\t" << "Agent[" << i << "]-PosZ" ;
		outStream << "\t" << "Agent[" << i << "]-TargetX" << "\t" << "Agent[" << i << "]-TargetZ" ;
	}

	for( int i = 0 ; i < (int) _obstacles.size() ; i++ )
	{
		outStream << "\t" << "box["<< i << "].xmin" << "\t" << "box[" << i << "].xmax";
	}
	outStream << "\tAreaRatio" ;
	outStream << "\tNumIntersections" ;
	outStream << "\n" ;
}

/// Records certain elements of a scenario in a previously opened stream, for SCA 2011
void 
ScenarioModule::recordScenario(std::ofstream &outStream) 
{
	std::cerr << "Warning: I will APPEND the scenario to an existing file!!\n" ;
	if( gEngine == NULL ) 
	{
		std::cerr << "gEngine is null\n" ;
		return ;
	}
	outStream << (int) _currentScenario 
		  << "\t"  << (long long) _currentNumberOfRandomCalls 
		  << "\t" << (int) _randomSeed ;
	outStream << "\t" << _agents.size() ;
	outStream << "\t"  << _obstacles.size() ;
	#ifdef _DEBUG1
	std::cout << "NumAgents = " << _agents.size() ;
	std::cout<< " NumObstacles = "  << _obstacles.size() << "\n" ;
	#endif
	int numPredictedInters = 0 ;
	for( int i = 0 ; i < (int) _agents.size() ; i++)
	{
		outStream << "\t" << _agents[i].position.x << "\t" << _agents[i].position.z ;
		outStream << "\t" << _agents[i].goals[0].targetLocation.x << "\t" << _agents[i].goals[0].targetLocation.z ;

		if( i != 0 ) numPredictedInters += predictIntersection(
			_agents[0].position.x, _agents[0].position.z,
			_agents[0].goals[0].targetLocation.x, _agents[0].goals[0].targetLocation.z,
			_agents[i].position.x, _agents[i].position.z,
			_agents[i].goals[0].targetLocation.x, _agents[i].goals[0].targetLocation.z) ;
	}

	float sizeX = gEngine->getSpatialDatabase()->getGridSizeX() ;
	float sizeZ = gEngine->getSpatialDatabase()->getGridSizeZ() ;
	float area = 0 ;
	for( int i = 0 ; i < (int) _obstacles.size() ; i++ )
	{
		Util::AxisAlignedBox box = _obstacles[i]->createObstacle()->getBounds() ;
		outStream << "\t" << box.xmin << "\t" << box.xmax ;
		outStream << "\t" << box.zmin << "\t" << box.zmax  ;
		area += fabs((box.xmax - box.xmin) * (box.zmax - box.zmin));
	}
	outStream << "\t" << area/(sizeX*sizeZ) ;
	outStream << "\t" << numPredictedInters ;
	outStream << "\n" ;
}

void 
ScenarioModule::draw()
{
#ifdef ENABLE_GUI

	//Util::DrawLib::glColor(Util::gBlack);
	// Draw static paths
	Util::Color c = Util::gYellow;
	Util::Color refpathColor = Util::gWhite;
#ifdef DrawPathsRef
	for (int j=0; j < agentPaths[0].size ()-1; j++)
	{

#ifdef DrawPathInteractionCirclesRef
			Util::DrawLib::drawCircle(agentPaths[0][j], Color(0,0.8f,0.6f), _interactionDistance, 20);
#endif
			// Util::DrawLib::drawCylinder(agentPaths[0][j], 0.2, 0.0, 0.1, refpathColor);


		Util::DrawLib::drawLine(agentPaths[0][j],agentPaths[0][j+1],c, 0.15);
		Util::DrawLib::drawCylinder(agentPaths[0][j], 0.2, 0.0, 0.1);
	}
#endif

#ifdef DrawPaths
	// Util::Color
	unsigned int j;
	for (unsigned int i=0; i < agentPaths.size(); i++)
	{
#ifdef _DEBUG12
	std::cout << "i = " << i << "\n";
#endif
		for ( j=0; j < agentPaths[i].size ()-1; j++)
		{

			if ( i == 0)
			{
#ifdef DrawPathInteractionCirclesRef
				Util::DrawLib::drawCircle(agentPaths[i][j], Color(0,0.8f,0.6f), _interactionDistance, 20);
#endif
				// Util::DrawLib::drawCylinder(agentPaths[i][j], 0.2, 0.0, 0.1, refpathColor);
			}

#ifdef _DEBUG12
	std::cout << "j = " << j << ", &agentpaths[i][j] = " << &agentPaths[i][j] <<
			", &agentpaths[i][j+1] = " << &agentPaths[i][j+1] << "\n";
#endif
			Util::DrawLib::drawLine(agentPaths[i][j],agentPaths[i][j+1],c, PathLineThickness);
			// Util::DrawLib::drawCylinder(agentPaths[i][j], 0.2, 0.0, 0.1);
#ifdef DrawPathInteractionCircles
			Util::DrawLib::drawCircle(agentPaths[i][j], Color(0,0.8f,0.6f), _interactionDistance, 20);
#endif
		}
#ifdef DrawPathInteractionCircles
			Util::DrawLib::drawCircle(agentPaths[i][j], Color(0,0.8f,0.6f), _interactionDistance, 20);
#endif
	}
#endif



#ifdef DrawIntersections
/*	for (int i = 0; i < _refAgentIntersectPoints.size(); i++)
	{
		Util::DrawLib::drawStar(_refAgentIntersectPoints.at(i), Util::Vector(1,0,0), 0.34f, gDarkGreen);
	}*/
	for (int z = 0; z < _agentIntersectionPoints.size(); z++)
	{
		Util::DrawLib::drawStar( _agentIntersectionPoints.at(z), Util::Vector(1,0,0), 0.34f, gDarkGreen);
	}
#endif

#ifdef DrawPathInteractionsRef
	for (int i = 0; i < _refAgentStaticPathIntersections.size(); i++)
	{
		Util::DrawLib::drawStar( _refAgentStaticPathIntersections.at(i), Util::Vector(1,0,0), 0.34f, gOrange);
#ifdef DrawInteractionDifficultyCirclesRef
		Util::DrawLib::drawCircle(_refAgentStaticPathIntersections.at(i), Color(0,0.8f,0.6f), _checkAgentInteractionRadius, 20);
#endif
	}
#endif
#ifdef DrawPathInteractions
	for (int i = 0; i < _agentStaticPathIntersections.size(); i++)
	{
		Util::DrawLib::drawStar( _agentStaticPathIntersections.at(i), Util::Vector(1,0,0), 0.34f, gDarkBlue);
#ifdef DrawInteractionDifficultyCircles
		Util::DrawLib::drawCircle(_agentStaticPathIntersections.at(i), Color(0,0.8f,0.6f), _checkAgentInteractionRadius, 20);
#endif
	}
#endif

#ifdef DrawGoals
	std::vector<SteerLib::AgentInterface*> agents = gEngine->getAgents();
	for (int p = 0; p < agents.size(); p++)
	{
		// std::cout << agents.at(p)->currentGoal().targetLocation << std::endl;
		Util::DrawLib::drawStar( agents.at(p)->currentGoal().targetLocation, Util::Vector(1,0,0), 0.34f, Color(0.65f,0.8f,0));
		Util::DrawLib::drawFlag( agents.at(p)->currentGoal().targetLocation, Color(0.5f,0.8f,0), 2);
		// Util::DrawLib::drawCircle(agents.at(p)->currentGoal().targetLocation, Color(0.5f,0.8f,0), 3, 30);
	}
#endif

#endif


}


void 
ScenarioModule::generateFailureSet(std::string &logFileName, std::set<long long> & failureSet, int numAlgos )
{
	std::ifstream f1 ( logFileName );
	
	long long randCalls;
	int success;
	bool allSuccess; 

	failureSet.clear ();

	while ( f1 >> randCalls )
	{
		allSuccess = false; 
		for (int i=0; i < numAlgos; i++)
		{
			f1 >> success;
			if ( success == 1 ) // atleast 1 succeeded
				allSuccess = true;
		}

		//std::cout << " rand : " << randCalls << " succ " << success << "\n";
		if ( allSuccess == false ) // all failed  
			failureSet.insert ( randCalls);
	}

	f1.close ();

}


void 
ScenarioModule::writeFailureSet ( std::string &logFileName, std::set<long long> & failureSet)
{
	std::ofstream f1 ( logFileName );

	std::set<long long>::iterator i1;
	for(i1 = failureSet.begin(); i1 != failureSet.end(); i1++)
	{
		f1 << *i1 << "\n";
	}

	f1.close ();
}


void ScenarioModule::readFailureSet ( std::string &logFileName )
{
	std::ifstream f1 ( logFileName );
	long long randCalls; 

	_failureSet.clear ();

	while ( f1 >> randCalls )
	{
		_failureSet.push_back(randCalls);

	}

	f1.close ();

}

void ScenarioModule::incrementRandomNumberGenerator ( long long numberOfRandomCalls)
{
	#ifdef _DEBUG1
	std::cout << " increment \n";
	#endif

	for (long long i=0; i < numberOfRandomCalls; i++)
		randGen->rand ();
}

