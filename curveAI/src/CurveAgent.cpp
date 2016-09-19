//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
// Copyright (c) 2015 Mahyar Khayatkhoei
//

#include <random>
#include "SteerLib.h"
#include "CurveAgent.h"
#include "CurveAIModule.h"
#include <cmath>
#include <iostream>
#include <algorithm>
#include <vector>
#include <time.h>

/// @file CurveAgent.cpp
/// @brief Implements the CurveAgent class.

#define MAX_FORCE_MAGNITUDE 3.0f
#define MAX_SPEED 1.3f
#define AGENT_MASS 1.0f

CurveAgent::CurveAgent()
{
	_enabled = false;

	// Set curve type here
	curve.setType(Util::hermiteCurve);
	//curve.setType(Util::catmullCurve);
}

CurveAgent::~CurveAgent()
{
	if (_enabled) {
		Util::AxisAlignedBox bounds(__position.x-_radius, __position.x+_radius, 0.0f, 0.0f, __position.z-_radius, __position.z+_radius);
		gSpatialDatabase->removeObject( this, bounds);
	}
}

void CurveAgent::disable()
{
	Util::AxisAlignedBox bounds(__position.x-_radius, __position.x+_radius, 0.0f, 0.0f, __position.z-_radius, __position.z+_radius);
	gSpatialDatabase->removeObject( this, bounds);
	_enabled = false;
}

void CurveAgent::reset(const SteerLib::AgentInitialConditions & initialConditions, SteerLib::EngineInterface * engineInfo)
{
	// compute the "old" bounding box of the agent before it is reset.  its OK that it will be invalid if the agent was previously disabled
	// because the value is not used in that case.
	Util::AxisAlignedBox oldBounds(__position.x-_radius, __position.x+_radius, 0.0f, 0.0f, __position.z-_radius, __position.z+_radius);

	// initialize the agent based on the initial conditions
	__startPosition = initialConditions.position;
	__position = initialConditions.position;
	_forward = initialConditions.direction;
	_radius = initialConditions.radius;
	_velocity = initialConditions.speed * Util::normalize(initialConditions.direction);

	// Find random agent color
	if (initialConditions.colorSet)
		agentColor = initialConditions.color;
	else
	{
		srand(time(NULL));
		float r, g, b;
		r = (float)(rand() % 80) / 100.0f;
		g = (float)(rand() % 80) / 100.0f;
		b = (float)(rand() % 80) / 100.0f;
		agentColor = Util::Color(r, g, b);
		//std::random_device seed;
		//std::default_random_engine generator(seed());
		//std::uniform_real_distribution<float> distribution(0.0f, 0.8f);
		//agentColor = Util::Color(distribution(generator), distribution(generator), distribution(generator));
	}

	// compute the "new" bounding box of the agent
	Util::AxisAlignedBox newBounds(__position.x-_radius, __position.x+_radius, 0.0f, 0.0f, __position.z-_radius, __position.z+_radius);

	if (!_enabled) {
		// if the agent was not enabled, then it does not already exist in the database, so add it.
		gSpatialDatabase->addObject( this, newBounds);
	}
	else {
		// if the agent was enabled, then the agent already existed in the database, so update it instead of adding it.
		gSpatialDatabase->updateObject( this, oldBounds, newBounds);
	}

	_enabled = true;

	if (initialConditions.goals.size() == 0) {
		throw Util::GenericException("No goals were specified!\n");
	}

	// iterate over the sequence of goals specified by the initial conditions.
	for (unsigned int i=0; i<initialConditions.goals.size(); i++) {
		if (initialConditions.goals[i].goalType == SteerLib::GOAL_TYPE_SEEK_STATIC_TARGET) {
			_goalQueue.push_back(initialConditions.goals[i]);
			if (initialConditions.goals[i].targetIsRandom) {
				// if the goal is random, we must randomly generate the goal.
				_goalQueue.back().targetLocation = gSpatialDatabase->randomPositionWithoutCollisions(1.0f, true);
			}
		}
		else {
			throw Util::GenericException("Unsupported goal type; CurveAgent only supports GOAL_TYPE_SEEK_STATIC_TARGET.");
		}
	}

	// Add control points to curve
	std::vector<Util::CurvePoint> controlPoints;
	Util::Vector startTangent(0.f, 0.f, 0.f);
	controlPoints.push_back(Util::CurvePoint(__position, startTangent, 0.f));
	for (int i = 0; i < _goalQueue.size(); i++)
	{
		controlPoints.push_back(Util::CurvePoint(_goalQueue[i].targetLocation, _goalQueue[i].targetTangent, _goalQueue[i].targetTime));
	}
	curve.addControlPoints(controlPoints);

	assert(_forward.length()!=0.0f);
	assert(_goalQueue.size() != 0);
	assert(_radius != 0.0f);
}


void CurveAgent::updateAI(float timeStamp, float dt, unsigned int frameNumber)
{
	//For this function, we assume that all goals are of type GOAL_TYPE_SEEK_STATIC_TARGET.
	//The error check for this was performed in reset().
	Util::AutomaticFunctionProfiler profileThisFunction( &CurveAIGlobals::gPhaseProfilers->aiProfiler );
	Util::Point newPosition;

	//Move one step on hermiteCurve
	if (!curve.calculatePoint(newPosition, timeStamp+dt))
	{
		disable();
		return;
	}

	//Update the database with the new agent's setup
	Util::AxisAlignedBox oldBounds(__position.x - _radius, __position.x + _radius, 0.0f, 0.0f, __position.z - _radius, __position.z + _radius);
	Util::AxisAlignedBox newBounds(newPosition.x - _radius, newPosition.x + _radius, 0.0f, 0.0f, newPosition.z - _radius, newPosition.z + _radius);
	gSpatialDatabase->updateObject(this, oldBounds, newBounds);

	//Update current position
	__position = newPosition;
}

SteerLib::EngineInterface * CurveAgent::getSimulationEngine()
{
	return gEngine;
}

void CurveAgent::draw()
{
#ifdef ENABLE_GUI
	// if the agent is selected, do some annotations just for demonstration
	if (gEngine->isAgentSelected(this)) {
		Util::Ray ray;
		ray.initWithUnitInterval(__position, _forward);
		float t = 0.0f;
		SteerLib::SpatialDatabaseItem * objectFound;
		Util::DrawLib::drawLine(ray.pos, ray.eval(1.0f));
		if (gSpatialDatabase->trace(ray, t, objectFound, this, false)) {
			Util::DrawLib::drawAgentDisc(__position, _forward, _radius, Util::gOrange);
		}
		else {
			Util::DrawLib::drawAgentDisc(__position, _forward, _radius, Util::gDarkOrange);
		}
	}
	else {
		Util::DrawLib::drawAgentDisc(__position, _forward, _radius, agentColor);
	}

	// Draw flags for all goal targets
	for (int i = 0; i < _goalQueue.size(); i++)
	{
		if (_goalQueue[i].goalType == SteerLib::GOAL_TYPE_SEEK_STATIC_TARGET)
		{
			Util::DrawLib::drawFlag(_goalQueue[i].targetLocation, agentColor, 2);
		}
	}

	// Draw curve path
	curve.drawCurve(agentColor);

#endif
}


void CurveAgent::_doEulerStep(const Util::Vector & steeringDecisionForce, float dt)
{
	// compute acceleration, _velocity, and newPosition by a simple Euler step
	const Util::Vector clippedForce = Util::clamp(steeringDecisionForce, MAX_FORCE_MAGNITUDE);
	Util::Vector acceleration = (clippedForce / AGENT_MASS);
	_velocity = _velocity + (dt*acceleration);
	_velocity = clamp(_velocity, MAX_SPEED);  // clamp _velocity to the max speed
	const Util::Point newPosition = __position + (dt*_velocity);

	// For this simple agent, we just make the orientation point along the agent's current velocity.
	if (_velocity.lengthSquared() != 0.0f) {
		_forward = normalize(_velocity);
	}

	// update the database with the new agent's setup
	Util::AxisAlignedBox oldBounds(__position.x - _radius, __position.x + _radius, 0.0f, 0.0f, __position.z - _radius, __position.z + _radius);
	Util::AxisAlignedBox newBounds(newPosition.x - _radius, newPosition.x + _radius, 0.0f, 0.0f, newPosition.z - _radius, newPosition.z + _radius);
	gSpatialDatabase->updateObject( this, oldBounds, newBounds);

	__position = newPosition;
}