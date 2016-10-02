/*!
*
* \author VaHiD AzIzI
*
*/


#include "SteerLib.h"
#include "CollisionAgent.h"
#include "CollisionAIModule.h"

/// @file CollisionAgent.cpp
/// @brief Implements the CollisionAgent class.

#define MAX_FORCE_MAGNITUDE 3.0f
#define MAX_SPEED 1.3f
#define AGENT_MASS 1.0f

CollisionAgent::CollisionAgent()
{
	_enabled = false;
}

CollisionAgent::~CollisionAgent()
{
	if (_enabled) {
		Util::AxisAlignedBox bounds(__position.x-_radius, __position.x+_radius, 0.0f, 0.0f, __position.z-_radius, __position.z+_radius);
		gSpatialDatabase->removeObject( this, bounds);
	}
}

void CollisionAgent::disable()
{
	Util::AxisAlignedBox bounds(__position.x-_radius, __position.x+_radius, 0.0f, 0.0f, __position.z-_radius, __position.z+_radius);
	gSpatialDatabase->removeObject( this, bounds);
	_enabled = false;
}

void CollisionAgent::reset(const SteerLib::AgentInitialConditions & initialConditions, SteerLib::EngineInterface * engineInfo)
{
    // nothing to do here
}


void CollisionAgent::updateAI(float timeStamp, float dt, unsigned int frameNumber)
{
    // nothing to do here
}


void CollisionAgent::draw()
{
    // nothing to do here
}


void CollisionAgent::_doEulerStep(const Util::Vector & steeringDecisionForce, float dt)
{
    // nothing to do here
}

SteerLib::EngineInterface * CollisionAgent::getSimulationEngine()
{
	return gEngine;
}