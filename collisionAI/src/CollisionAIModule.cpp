/*!
*
* \author VaHiD AzIzI
*
*/

/// @file CollisionAIModule.cpp
/// @brief Implements the CollisionAIModule plugin.


#include "SteerLib.h"
#include "SimulationPlugin.h"
#include "CollisionAIModule.h"
#include "CollisionAgent.h"

#include "LogObject.h"
#include "LogManager.h"

#include "obstacles/GJK_EPA.h"


// globally accessible to the CollisionAI plugin
SteerLib::EngineInterface*  gEngine;
SteerLib::SpatialDataBaseInterface*   gSpatialDatabase;

namespace CollisionAIGlobals
{
	unsigned int gLongTermPlanningPhaseInterval;
	unsigned int gMidTermPlanningPhaseInterval;
	unsigned int gShortTermPlanningPhaseInterval;
	unsigned int gPredictivePhaseInterval;
	unsigned int gReactivePhaseInterval;
	unsigned int gPerceptivePhaseInterval;
	bool gUseDynamicPhaseScheduling;
	bool gShowStats;
	bool gShowAllStats;

	PhaseProfilers * gPhaseProfilers;
}

using namespace CollisionAIGlobals;

PLUGIN_API SteerLib::ModuleInterface * createModule()
{
    return new CollisionAIModule;
}

PLUGIN_API void destroyModule( SteerLib::ModuleInterface*  module )
{
	delete module;
}


void CollisionAIModule::init( const SteerLib::OptionDictionary & options, SteerLib::EngineInterface * engineInfo )
{
	gEngine = engineInfo;
	gSpatialDatabase = engineInfo->getSpatialDatabase();

	gUseDynamicPhaseScheduling = false;
	gShowStats = false;
	logStats = false;
	gShowAllStats = false;
    logFilename = "CollisionAI.log";

	SteerLib::OptionDictionary::const_iterator optionIter;
	for (optionIter = options.begin(); optionIter != options.end(); ++optionIter) {
		std::stringstream value((*optionIter).second);
		if ((*optionIter).first == "")
		{
			value >> gLongTermPlanningPhaseInterval;
		}
		else if ((*optionIter).first == "ailogFileName")
		{
			logFilename = value.str();
			logStats = true;
		}
		else if ((*optionIter).first == "stats")
		{
			gShowStats = Util::getBoolFromString(value.str());
		}
		else if ((*optionIter).first == "allstats")
		{
			gShowAllStats = Util::getBoolFromString(value.str());
		}
		else
		{
			// throw Util::GenericException("unrecognized option \"" + Util::toString((*optionIter).first) + "\" given to PPR AI module.");
		}
	}

	if( logStats )
	{

		_logger = LogManager::getInstance()->createLogger(logFilename,LoggerType::BASIC_WRITE);

		_logger->addDataField("number_of_times_executed",DataType::LongLong );
		_logger->addDataField("total_ticks_accumulated",DataType::LongLong );
		_logger->addDataField("shortest_execution",DataType::LongLong );
		_logger->addDataField("longest_execution",DataType::LongLong );
		_logger->addDataField("fastest_execution", DataType::Float);
		_logger->addDataField("slowest_execution", DataType::Float);
		_logger->addDataField("average_time_per_call", DataType::Float);
		_logger->addDataField("total_time_of_all_calls", DataType::Float);
		_logger->addDataField("tick_frequency", DataType::Float);

		// LETS TRY TO WRITE THE LABELS OF EACH FIELD
		std::stringstream labelStream;
		unsigned int i;
		for (i=0; i < _logger->getNumberOfFields() - 1; i++)
			labelStream << _logger->getFieldName(i) << " ";
		labelStream << _logger->getFieldName(i);

		_logger->writeData(labelStream.str());

	}
}

void CollisionAIModule::initializeSimulation()
{
	//
	// initialize the performance profilers
	//
	gPhaseProfilers = new PhaseProfilers;
	gPhaseProfilers->aiProfiler.reset();
	gPhaseProfilers->longTermPhaseProfiler.reset();
	gPhaseProfilers->midTermPhaseProfiler.reset();
	gPhaseProfilers->shortTermPhaseProfiler.reset();
	gPhaseProfilers->perceptivePhaseProfiler.reset();
	gPhaseProfilers->predictivePhaseProfiler.reset();
	gPhaseProfilers->reactivePhaseProfiler.reset();
	gPhaseProfilers->steeringPhaseProfiler.reset();

}

void CollisionAIModule::preprocessSimulation()
{
    std::set<SteerLib::ObstacleInterface*> _obstacles = gEngine->getObstacles();

    std::vector<std::vector<Util::Vector>> polyVects;
    std::vector<Util::Point> points;
	std::vector<Util::Vector> vects;

    std::set<SteerLib::ObstacleInterface*>::iterator obstacleIter;

    for ( obstacleIter = _obstacles.begin(); obstacleIter != _obstacles.end(); ++obstacleIter )
    {
		 points = (*obstacleIter)->get2DStaticGeometry();

		 for (size_t i = 0; i < points.size(); ++i)
		 {
			 vects.push_back(points[i].vector());
		 }

         polyVects.push_back( vects );
         vects.clear();
    }

    for ( size_t i = 0; i < polyVects.size(); ++i )
    {
        for ( size_t j = i + 1; j < polyVects.size(); ++j )
        {
            Util::Vector penetration_vector;
			float penetration_depth;
            if ( SteerLib::GJK_EPA::intersect( penetration_depth, penetration_vector, polyVects[i], polyVects[j] ) )
                std::cerr<<" Collision detected between polygon No."<<i<<" and No."<<j<< " with a penetration depth of "<< penetration_depth << " and penetration vector of "<<penetration_vector<<std::endl;
            else
                std::cerr<<" NO collision detected between polygon No."<<i<<" and No."<<j<<std::endl;
        }
    }
}

void CollisionAIModule::preprocessFrame(float timeStamp, float dt, unsigned int frameNumber)
{
	//TODO does nothing for now
}

void CollisionAIModule::postprocessFrame(float timeStamp, float dt, unsigned int frameNumber)
{
	//TODO does nothing for now
}

void CollisionAIModule::cleanupSimulation()
{

	if ( logStats )
	{
		LogObject logObject;

		logObject.addLogData(gPhaseProfilers->aiProfiler.getNumTimesExecuted());
		logObject.addLogData(gPhaseProfilers->aiProfiler.getTotalTicksAccumulated());
		logObject.addLogData(gPhaseProfilers->aiProfiler.getMinTicks());
		logObject.addLogData(gPhaseProfilers->aiProfiler.getMaxTicks());
		logObject.addLogData(gPhaseProfilers->aiProfiler.getMinExecutionTimeMills());
		logObject.addLogData(gPhaseProfilers->aiProfiler.getMaxExecutionTimeMills());
		logObject.addLogData(gPhaseProfilers->aiProfiler.getAverageExecutionTimeMills());
		logObject.addLogData(gPhaseProfilers->aiProfiler.getTotalTime());
		logObject.addLogData(gPhaseProfilers->aiProfiler.getTickFrequency());

		_logger->writeLogObject(logObject);

		// cleanup profileing metrics for next simulation/scenario
		gPhaseProfilers->aiProfiler.reset();
		gPhaseProfilers->longTermPhaseProfiler.reset();
		gPhaseProfilers->midTermPhaseProfiler.reset();
		gPhaseProfilers->shortTermPhaseProfiler.reset();
		gPhaseProfilers->perceptivePhaseProfiler.reset();
		gPhaseProfilers->predictivePhaseProfiler.reset();
		gPhaseProfilers->reactivePhaseProfiler.reset();
		gPhaseProfilers->steeringPhaseProfiler.reset();
	}

	// kdTree_->deleteObstacleTree(kdTree_->obstacleTree_);
}

void CollisionAIModule::finish()
{
	// nothing to do here
}

SteerLib::AgentInterface * CollisionAIModule::createAgent()
{
    return new CollisionAgent;
}

void CollisionAIModule::destroyAgent( SteerLib::AgentInterface * agent )
{
	delete agent;
}
