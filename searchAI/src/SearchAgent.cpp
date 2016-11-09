//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#include "SteerLib.h"
#include "SearchAgent.h"
#include "SearchAIModule.h"
#include "util/Color.h"

/// @file SearchAgent.cpp
/// @brief Implements the SearchAgent class.

#define MAX_FORCE_MAGNITUDE 3.0f
#define MAX_SPEED 0.13f
#define AGENT_MASS 1.0f
#define GOAL_REGION 0.1f
#define DURATION 15

SearchAgent::SearchAgent()
{
	_enabled = false;
}

SearchAgent::~SearchAgent()
{
	if (_enabled) {
		Util::AxisAlignedBox bounds(__position.x-_radius, __position.x+_radius, 0.0f, 0.0f, __position.z-_radius, __position.z+_radius);
		gSpatialDatabase->removeObject( this, bounds);
	}
}

void SearchAgent::disable()
{
	Util::AxisAlignedBox bounds(__position.x-_radius, __position.x+_radius, 0.0f, 0.0f, __position.z-_radius, __position.z+_radius);
	gSpatialDatabase->removeObject( this, bounds);
	_enabled = false;
}

void SearchAgent::reset(const SteerLib::AgentInitialConditions & initialConditions, SteerLib::EngineInterface * engineInfo)
{
	// compute the "old" bounding box of the agent before it is reset.  its OK that it will be invalid if the agent was previously disabled
	// because the value is not used in that case.
	std::cout<<"Reset is called";
	Util::AxisAlignedBox oldBounds(__position.x-_radius, __position.x+_radius, 0.0f, 0.0f, __position.z-_radius, __position.z+_radius);

	// initialize the agent based on the initial conditions
	__position = initialConditions.position;
	_forward = initialConditions.direction;
	_radius = initialConditions.radius;
	_velocity = initialConditions.speed * Util::normalize(initialConditions.direction);

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
			_goalQueue.push(initialConditions.goals[i]);
			if (initialConditions.goals[i].targetIsRandom) {
				// if the goal is random, we must randomly generate the goal.
				_goalQueue.back().targetLocation = gSpatialDatabase->randomPositionWithoutCollisions(1.0f, true);
			}
		}
		else {
			throw Util::GenericException("Unsupported goal type; SearchAgent only supports GOAL_TYPE_SEEK_STATIC_TARGET.");
		}
	}

	assert(_forward.length()!=0.0f);
	assert(_goalQueue.size() != 0);
	assert(_radius != 0.0f);
}


void SearchAgent::computePlan()
{
	std::cout<<"\nComputing agent plan ";
	if (!_goalQueue.empty())
	{
		Util::Point global_goal = _goalQueue.front().targetLocation;
		if (astar.computePath(__path, __position, _goalQueue.front().targetLocation, gSpatialDatabase))
		{

			while (!_goalQueue.empty())
				_goalQueue.pop();

			for (int i = 0; i < __path.size(); ++i)
			{
				SteerLib::AgentGoalInfo goal_path_pt;
				goal_path_pt.targetLocation = __path[i];
				_goalQueue.push(goal_path_pt);
			}
			SteerLib::AgentGoalInfo goal_path_pt;
			goal_path_pt.targetLocation = global_goal;
			_goalQueue.push(goal_path_pt);
		}
		// else
		// {
		// 	for(int i = 0;i<20;++i)
		// 		_goalQueue.push(_goalQueue.front());
		// }
	}


}


void SearchAgent::updateAI(float timeStamp, float dt, unsigned int frameNumber)
{
	Util::AutomaticFunctionProfiler profileThisFunction( &SearchAIGlobals::gPhaseProfilers->aiProfiler );

	
	double steps = (DURATION/(double)__path.size());
	if(timeStamp*dt > last_waypoint*steps)
	{	
		if(!_goalQueue.empty())
		{
			__position = _goalQueue.front().targetLocation;
			std::cout<<"Waypoint: "<< __position;
			_goalQueue.pop();
			last_waypoint++;
		}
	}
}


void SearchAgent::draw()
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
			Util::DrawLib::drawAgentDisc(__position, _forward, _radius, Util::gBlue);
		}
		else {
			Util::DrawLib::drawAgentDisc(__position, _forward, _radius);
		}
	}
	else {
		Util::DrawLib::drawAgentDisc(__position, _forward, _radius, Util::gGray40);
	}
	if (_goalQueue.front().goalType == SteerLib::GOAL_TYPE_SEEK_STATIC_TARGET) {
		Util::DrawLib::drawFlag(_goalQueue.front().targetLocation);
	}
	
	if(__path.size()>0)
	{
		for(int i = 1; i<__path.size(); ++i)
			Util::DrawLib::drawLine(__path[i-1], __path[i], Util::Color(1.0f, 0.0f, 0.0f), 2);
		Util::DrawLib::drawCircle(__path[__path.size()-1], Util::Color(0.0f, 1.0f, 0.0f));
	}
#endif
}
