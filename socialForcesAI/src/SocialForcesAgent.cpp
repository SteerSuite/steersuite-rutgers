//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//



#include "SocialForcesAgent.h"
#include "SocialForcesAIModule.h"
#include "SocialForces_Parameters.h"
// #include <math.h>

// #include "util/Geometry.h"

/// @file SocialForcesAgent.cpp
/// @brief Implements the SocialForcesAgent class.

#undef min
#undef max

#define AGENT_MASS 1.0f
#define AGENT_ACCELERATION_PERCENTAGE .5f
#define INDIV_BEHAVIOUR_PERCENTAGE .5f

using namespace Util;
using namespace SocialForcesGlobals;
using namespace SteerLib;

// #define _DEBUG_ENTROPY 1

SocialForcesAgent::SocialForcesAgent()
{
	_SocialForcesParams.sf_acceleration = sf_acceleration;
	_SocialForcesParams.sf_personal_space_threshold = sf_personal_space_threshold;
	_SocialForcesParams.sf_agent_repulsion_importance = sf_agent_repulsion_importance;
	_SocialForcesParams.sf_query_radius = sf_query_radius;
	_SocialForcesParams.sf_body_force = sf_body_force;
	_SocialForcesParams.sf_agent_body_force = sf_agent_body_force;
	_SocialForcesParams.sf_sliding_friction_force = sf_sliding_friction_force;
	_SocialForcesParams.sf_agent_b = sf_agent_b;
	_SocialForcesParams.sf_agent_a = sf_agent_a;
	_SocialForcesParams.sf_wall_b = sf_wall_b;
	_SocialForcesParams.sf_wall_a = sf_wall_a;
	_SocialForcesParams.sf_max_speed = sf_max_speed;

	_enabled = false;
}

SocialForcesAgent::~SocialForcesAgent()
{
	// std::cout << this << " is being deleted" << std::endl;
	/*
	if (this->enabled())
	{
	Util::AxisAlignedBox bounds(_position.x-_radius, _position.x+_radius, 0.0f, 0.0f, _position.z-_radius, _position.z+_radius);
	// getSimulationEngine()->getSpatialDatabase()->removeObject( this, bounds);
	}*/
	// std::cout << "Someone is removing an agent " << std::endl;
}

SteerLib::EngineInterface * SocialForcesAgent::getSimulationEngine()
{
	return _gEngine;
}

void SocialForcesAgent::setParameters(Behaviour behave)
{
	this->_SocialForcesParams.setParameters(behave);
}

void SocialForcesAgent::disable()
{
	// DO nothing for now
	// if we tried to disable a second time, most likely we accidentally ignored that it was disabled, and should catch that error.
	// std::cout << "this agent is being disabled " << this << std::endl;
	assert(_enabled == true);


	//  1. remove from database
	AxisAlignedBox b = AxisAlignedBox(_position.x - _radius, _position.x + _radius, 0.0f, 0.0f, _position.z - _radius, _position.z + _radius);
	getSimulationEngine()->getSpatialDatabase()->removeObject(dynamic_cast<SpatialDatabaseItemPtr>(this), b);

	//  2. set enabled = false
	_enabled = false;


}

void SocialForcesAgent::reset(const SteerLib::AgentInitialConditions & initialConditions, SteerLib::EngineInterface * engineInfo)
{
	// compute the "old" bounding box of the agent before it is reset.  its OK that it will be invalid if the agent was previously disabled
	// because the value is not used in that case.
	// std::cout << "resetting agent " << this << std::endl;
	_waypoints.clear();
	_midTermPath.clear();

	Util::AxisAlignedBox oldBounds(_position.x - _radius, _position.x + _radius, 0.0f, 0.5f, _position.z - _radius, _position.z + _radius);


	// initialize the agent based on the initial conditions
	/*
	position_ = Vector2(initialConditions.position.x, initialConditions.position.z);
	radius_ = initialConditions.radius;
	velocity_ = normalize(Vector2(initialConditions.direction.x, initialConditions.direction.z));
	velocity_ = velocity_ * initialConditions.speed;
	*/
	// initialize the agent based on the initial conditions
	_position = initialConditions.position;
	_forward = normalize(initialConditions.direction);
	_radius = initialConditions.radius;
	_velocity = initialConditions.speed * _forward;
	// std::cout << "inital colour of agent " << initialConditions.color << std::endl;
	if (initialConditions.colorSet == true)
	{
		this->_color = initialConditions.color;
	}
	else
	{
		this->_color = Util::gBlue;
	}

	// compute the "new" bounding box of the agent
	Util::AxisAlignedBox newBounds(_position.x - _radius, _position.x + _radius, 0.0f, 0.5f, _position.z - _radius, _position.z + _radius);

	if (!_enabled) {
		// if the agent was not enabled, then it does not already exist in the database, so add it.
		// std::cout
		getSimulationEngine()->getSpatialDatabase()->addObject(dynamic_cast<SpatialDatabaseItemPtr>(this), newBounds);
	}
	else {
		// if the agent was enabled, then the agent already existed in the database, so update it instead of adding it.
		// std::cout << "new position is " << _position << std::endl;
		// std::cout << "new bounds are " << newBounds << std::endl;
		// std::cout << "reset update " << this << std::endl;
		getSimulationEngine()->getSpatialDatabase()->updateObject(dynamic_cast<SpatialDatabaseItemPtr>(this), oldBounds, newBounds);
		// engineInfo->getSpatialDatabase()->updateObject( this, oldBounds, newBounds);
	}

	_enabled = true;

	if (initialConditions.goals.size() == 0)
	{
		throw Util::GenericException("No goals were specified!\n");
	}

	SeekTarget.clear();
	FleeTarget.clear();
	agentName = initialConditions.name;

	while (!_goalQueue.empty())
	{
		_goalQueue.pop();
	}

	// iterate over the sequence of goals specified by the initial conditions.
	for (unsigned int i = 0; i<initialConditions.goals.size(); i++) {
		if (initialConditions.goals[i].goalType == SteerLib::GOAL_TYPE_SEEK_STATIC_TARGET ||
			initialConditions.goals[i].goalType == GOAL_TYPE_AXIS_ALIGNED_BOX_GOAL)
		{
			if (initialConditions.goals[i].targetIsRandom)
			{
				// if the goal is random, we must randomly generate the goal.
				// std::cout << "assigning random goal" << std::endl;
				SteerLib::AgentGoalInfo _goal;
				_goal.targetLocation = getSimulationEngine()->getSpatialDatabase()->randomPositionWithoutCollisions(1.0f, true);
				_goalQueue.push(_goal);
				_currentGoal.targetLocation = _goal.targetLocation;
			}
			else
			{
				_goalQueue.push(initialConditions.goals[i]);
			}
		}
		else if (initialConditions.goals[i].goalType == GOAL_TYPE_SEEK_DYNAMIC_TARGET)
			SeekTarget.insert(initialConditions.goals[i].targetName);
		else if (initialConditions.goals[i].goalType == GOAL_TYPE_FLEE_DYNAMIC_TARGET)
			FleeTarget.insert(initialConditions.goals[i].targetName);

		else {
			throw Util::GenericException("Unsupported goal type; SocialForcesAgent only supports GOAL_TYPE_SEEK_STATIC_TARGET and GOAL_TYPE_AXIS_ALIGNED_BOX_GOAL.");
		}
	}

	if (_goalQueue.size() > 0)
		runLongTermPlanning(_goalQueue.front().targetLocation, dont_plan);

	// std::cout << "first waypoint: " << _waypoints.front() << " agents position: " << position() << std::endl;
	/*
	* Must make sure that _waypoints.front() != position(). If they are equal the agent will crash.
	* And that _waypoints is not empty
	*/
	Util::Vector goalDirection;
	if (_goalQueue.size() > 0)
	{
		if (!_midTermPath.empty())
		{
			this->updateLocalTarget();
			goalDirection = normalize(this->_currentLocalTarget - position());
		}
		else
		{
			goalDirection = normalize(_goalQueue.front().targetLocation - position());
		}

		_prefVelocity =
			(
			(
			(
			Util::Vector(goalDirection.x, 0.0f, goalDirection.z) *
			PERFERED_SPEED
			)
			- velocity()
			)
			/
			_SocialForcesParams.sf_acceleration
			)
			*
			MASS;

		// _velocity = _prefVelocity;
#ifdef _DEBUG_ENTROPY
		std::cout << "goal direction is: " << goalDirection << " prefvelocity is: " << prefVelocity_ <<
			" and current velocity is: " << velocity_ << std::endl;
#endif
	}

	// std::cout << "Parameter spec: " << _SocialForcesParams << std::endl;
	// _gEngine->addAgent(this, rvoModule);
	assert(_forward.length() != 0.0f);
	assert(_goalQueue.size() + SeekTarget.size() + FleeTarget.size() != 0);
	assert(_radius != 0.0f);
}


void SocialForcesAgent::calcNextStep(float dt)
{

}

std::pair<float, Util::Point> minimum_distance(Util::Point l1, Util::Point l2, Util::Point p)
{
	// Return minimum distance between line segment vw and point p
	float lSq = (l1 - l2).lengthSquared();  // i.e. |l2-l1|^2 -  avoid a sqrt
	if (lSq == 0.0)
		return std::make_pair((p - l2).length(), l1);   // l1 == l2 case
	// Consider the line extending the segment, parameterized as l1 + t (l2 - l1).
	// We find projection of point p onto the line.
	// It falls where t = [(p-l1) . (l2-l1)] / |l2-l1|^2
	const float t = dot(p - l1, l2 - l1) / lSq;
	if (t < 0.0)
	{
		return std::make_pair((p - l1).length(), l1);       // Beyond the 'l1' end of the segment
	}
	else if (t > 1.0)
	{
		return std::make_pair((p - l2).length(), l2);  // Beyond the 'l2' end of the segment
	}
	const Util::Point projection = l1 + t * (l2 - l1);  // Projection falls on the segment
	return std::make_pair((p - projection).length(), projection);
}


Util::Vector SocialForcesAgent::calcProximityForce(float dt)
{
	std::set<SteerLib::SpatialDatabaseItemPtr> _neighbors;
	getSimulationEngine()->getSpatialDatabase()->getItemsInRange(_neighbors,
		_position.x - (this->_radius + _SocialForcesParams.sf_query_radius),
		_position.x + (this->_radius + _SocialForcesParams.sf_query_radius),
		_position.z - (this->_radius + _SocialForcesParams.sf_query_radius),
		_position.z + (this->_radius + _SocialForcesParams.sf_query_radius),
		dynamic_cast<SteerLib::SpatialDatabaseItemPtr>(this));

	SteerLib::AgentInterface * tmp_agent;
	SteerLib::ObstacleInterface * tmp_ob;
	Util::Vector away = Util::Vector(0, 0, 0);
	Util::Vector away_obs = Util::Vector(0, 0, 0);

	for (std::set<SteerLib::SpatialDatabaseItemPtr>::iterator neighbour = _neighbors.begin(); neighbour != _neighbors.end(); neighbour++)
		// for (int a =0; a < tmp_agents.size(); a++)
	{
		if ((*neighbour)->isAgent())
		{
			tmp_agent = dynamic_cast<SteerLib::AgentInterface *>(*neighbour);

			// direction away from other agent
			Util::Vector away_tmp = normalize(position() - tmp_agent->position());
			// std::cout << "away_agent_tmp vec" << away_tmp << std::endl;
			// Scale force
			// std::cout << "the exp of agent distance is " << exp((radius() + tmp_agent->radius()) -
			//	(position() - tmp_agent->position()).length()) << std::endl;


			// away = away + (away_tmp * ( radius() / ((position() - tmp_agent->position()).length() * B) ));
			away = away +
				(
				away_tmp
				*
				(
				_SocialForcesParams.sf_agent_a
				*
				exp(
				(
				(
				(
				this->radius()
				+
				tmp_agent->radius()
				)
				-
				(
				this->position()
				-
				tmp_agent->position()
				).length()
				)
				/
				_SocialForcesParams.sf_agent_b
				)
				)


				)
				*
				dt
				);
			/*
			std::cout << "agent " << this->id() << " away this far " << away <<
			" distance " << exp(
			(
			(
			(
			radius()
			+
			tmp_agent->radius()
			)
			-
			(
			position()
			-
			tmp_agent->position()
			).length()
			)
			/
			_SocialForcesParams.sf_agent_b
			)
			) << std::endl;
			*/
		}
		else
		{
			// It is an obstacle
			tmp_ob = dynamic_cast<SteerLib::ObstacleInterface *>(*neighbour);
			CircleObstacle * obs_cir = dynamic_cast<SteerLib::CircleObstacle *>(tmp_ob);
			if (obs_cir != NULL && USE_CIRCLES)
			{
				// std::cout << "Found circle obstacle" << std::endl;
				Util::Vector away_tmp = normalize(position() - obs_cir->position());
				away = away +
					(
					away_tmp
					*
					(
					_SocialForcesParams.sf_wall_a
					*
					exp(
					(
					(
					(
					this->radius()
					+
					obs_cir->radius()
					)
					-
					(
					this->position()
					-
					obs_cir->position()
					).length()
					)
					/
					_SocialForcesParams.sf_wall_b
					)
					)


					)
					*
					dt
					);
			}
			else
			{
				Util::Vector wall_normal = calcWallNormal(tmp_ob);
				std::pair<Util::Point, Util::Point> line = calcWallPointsFromNormal(tmp_ob, wall_normal);
				// Util::Point midpoint = Util::Point((line.first.x+line.second.x)/2, ((line.first.y+line.second.y)/2)+1,
				// 	(line.first.z+line.second.z)/2);
				std::pair<float, Util::Point> min_stuff = minimum_distance(line.first, line.second, position());
				// wall distance

				Util::Vector away_obs_tmp = normalize(position() - min_stuff.second);
				// std::cout << "away_obs_tmp vec" << away_obs_tmp << std::endl;
				// away_obs = away_obs + ( away_obs_tmp * ( radius() / ((position() - min_stuff.second).length() * B ) ) );
				away_obs = away_obs +
					(
					away_obs_tmp
					*
					(
					_SocialForcesParams.sf_wall_a
					*
					exp(
					(
					(
					(this->radius()) -
					(
					this->position()
					-
					min_stuff.second
					).length()
					)
					/
					_SocialForcesParams.sf_wall_b
					)
					)
					)
					*
					dt
					);
			}
		}

	}
	return away + away_obs;
}

Util::Vector SocialForcesAgent::calcRepulsionForce(float dt)
{
#ifdef _DEBUG_
	std::cout << "wall repulsion; " << calcWallRepulsionForce(dt) << " agent repulsion " <<
		(_SocialForcesParams.sf_agent_repulsion_importance * calcAgentRepulsionForce(dt)) << std::endl;
#endif
//	return calcWallRepulsionForce(dt);
	return calcWallRepulsionForce(dt) + (_SocialForcesParams.sf_agent_repulsion_importance * calcAgentRepulsionForce(dt));
}

Util::Vector SocialForcesAgent::calcAgentRepulsionForce(float dt)
{

	Util::Vector agent_repulsion_force = Util::Vector(0, 0, 0);

	std::set<SteerLib::SpatialDatabaseItemPtr> _neighbors;
	getSimulationEngine()->getSpatialDatabase()->getItemsInRange(_neighbors,
		_position.x - (this->_radius + _SocialForcesParams.sf_query_radius),
		_position.x + (this->_radius + _SocialForcesParams.sf_query_radius),
		_position.z - (this->_radius + _SocialForcesParams.sf_query_radius),
		_position.z + (this->_radius + _SocialForcesParams.sf_query_radius),
		(this));

	SteerLib::AgentInterface * tmp_agent;

	for (std::set<SteerLib::SpatialDatabaseItemPtr>::iterator neighbour = _neighbors.begin(); neighbour != _neighbors.end(); neighbour++)
		// for (int a =0; a < tmp_agents.size(); a++)
	{
		if ((*neighbour)->isAgent())
		{
			tmp_agent = dynamic_cast<SteerLib::AgentInterface *>(*neighbour);
		}
		else
		{
			continue;
		}
		if ((id() != tmp_agent->id()) &&
			(tmp_agent->computePenetration(this->position(), this->radius()) > 0.000001)
			)
		{
			agent_repulsion_force = agent_repulsion_force +
				(tmp_agent->computePenetration(this->position(), this->radius()) * _SocialForcesParams.sf_agent_body_force * dt) *
				normalize(position() - tmp_agent->position());
			// normalized tangential force
			/*
			agent_repulsion_force = agent_repulsion_force +
			(
			(
			(-1*position()) - tmp_agent->position()
			)
			/
			(
			(-1*position()) - tmp_agent->position()
			).length()

			)*0.2;
			*/
			//TODO this can have some funny behaviour is velocity == 0
			Util::Vector tangent = cross(cross(tmp_agent->position() - position(), velocity()),
				tmp_agent->position() - position());
			tangent = tangent / tangent.length();
			float  tanget_v_diff = dot(tmp_agent->velocity() - velocity(), tangent);
			// std::cout << "Velocity diff is " << tanget_v_diff << " tangent is " << tangent <<
			//	" velocity is " << velocity() << std::endl;
			agent_repulsion_force = agent_repulsion_force +
				(_SocialForcesParams.sf_sliding_friction_force * dt *
				(
				tmp_agent->computePenetration(this->position(), this->radius())
				) * tangent * tanget_v_diff

				);
		}

	}
	return agent_repulsion_force;
}

Util::Vector SocialForcesAgent::calcWallRepulsionForce(float dt)
{

	Util::Vector wall_repulsion_force = Util::Vector(0, 0, 0);


	std::set<SteerLib::SpatialDatabaseItemPtr> _neighbors;
	getSimulationEngine()->getSpatialDatabase()->getItemsInRange(_neighbors,
		_position.x - (this->_radius + _SocialForcesParams.sf_query_radius),
		_position.x + (this->_radius + _SocialForcesParams.sf_query_radius),
		_position.z - (this->_radius + _SocialForcesParams.sf_query_radius),
		_position.z + (this->_radius + _SocialForcesParams.sf_query_radius),
		dynamic_cast<SteerLib::SpatialDatabaseItemPtr>(this));

	SteerLib::ObstacleInterface * tmp_ob;

	for (std::set<SteerLib::SpatialDatabaseItemPtr>::iterator neighbour = _neighbors.begin(); neighbour != _neighbors.end(); neighbour++)
		// for (std::set<SteerLib::ObstacleInterface * >::iterator tmp_o = _neighbors.begin();  tmp_o != _neighbors.end();  tmp_o++)
	{
		if (!(*neighbour)->isAgent())
		{
			tmp_ob = dynamic_cast<SteerLib::ObstacleInterface *>(*neighbour);
		}
		else
		{
			continue;
		}
		if (tmp_ob->computePenetration(this->position(), this->radius()) > 0.000001)
		{
			CircleObstacle * cir_obs = dynamic_cast<SteerLib::CircleObstacle *>(tmp_ob);
			if (cir_obs != NULL && USE_CIRCLES)
			{
				// std::cout << "Intersected circle obstacle" << std::endl;
				Util::Vector wall_normal = position() - cir_obs->position();
				// wall distance
				float distance = wall_normal.length() - cir_obs->radius();

				wall_normal = normalize(wall_normal);
				wall_repulsion_force = wall_repulsion_force +
					((
					(
					(
					wall_normal
					)
					*
					(
					radius() +
					_SocialForcesParams.sf_personal_space_threshold -
					(
					distance
					)
					)
					)
					/
					distance
					)* _SocialForcesParams.sf_body_force * dt);

				// tangential force
				// std::cout << "wall tangent " << rightSideInXZPlane(wall_normal) <<
				// 	" dot is " << dot(forward(),  rightSideInXZPlane(wall_normal)) <<
				// std::endl;
				wall_repulsion_force = wall_repulsion_force +
					(
					dot(forward(), rightSideInXZPlane(wall_normal))
					*
					rightSideInXZPlane(wall_normal)
					*
					cir_obs->computePenetration(this->position(), this->radius())
					)* _SocialForcesParams.sf_sliding_friction_force * dt;

			}
			else
			{
				Util::Vector wall_normal = calcWallNormal(tmp_ob);
				std::pair<Util::Point, Util::Point> line = calcWallPointsFromNormal(tmp_ob, wall_normal);
				// Util::Point midpoint = Util::Point((line.first.x+line.second.x)/2, ((line.first.y+line.second.y)/2)+1,
				// 	(line.first.z+line.second.z)/2);
				std::pair<float, Util::Point> min_stuff = minimum_distance(line.first, line.second, position());
				// wall distance
				wall_repulsion_force = wall_repulsion_force +
					((
					(
					(
					wall_normal
					)
					*
					(
					radius() +
					_SocialForcesParams.sf_personal_space_threshold -
					(
					min_stuff.first
					)
					)
					)
					/
					min_stuff.first
					)* _SocialForcesParams.sf_body_force * dt);
				// tangential force
				// std::cout << "wall tangent " << rightSideInXZPlane(wall_normal) <<
				// 	" dot is " << dot(forward(),  rightSideInXZPlane(wall_normal)) <<
				// std::endl;
				wall_repulsion_force = wall_repulsion_force +
					(
					dot(forward(), rightSideInXZPlane(wall_normal))
					*
					rightSideInXZPlane(wall_normal)
					*
					tmp_ob->computePenetration(this->position(), this->radius())
					)* _SocialForcesParams.sf_sliding_friction_force * dt;
			}
		}

	}
	return wall_repulsion_force;
}

std::pair<Util::Point, Util::Point> SocialForcesAgent::calcWallPointsFromNormal(SteerLib::ObstacleInterface* obs, Util::Vector normal)
{
	Util::AxisAlignedBox box = obs->getBounds();
	if (normal.z == 1)
	{
		return std::make_pair(Util::Point(box.xmin, 0, box.zmax), Util::Point(box.xmax, 0, box.zmax));
		// Ended here;
	}
	else if (normal.z == -1)
	{
		return std::make_pair(Util::Point(box.xmin, 0, box.zmin), Util::Point(box.xmax, 0, box.zmin));
	}
	else if (normal.x == 1)
	{
		return std::make_pair(Util::Point(box.xmax, 0, box.zmin), Util::Point(box.xmax, 0, box.zmax));
	}
	else // normal.x == -1
	{
		return std::make_pair(Util::Point(box.xmin, 0, box.zmin), Util::Point(box.xmin, 0, box.zmax));
	}
}

/**
* Basically What side of the obstacle is the agent on use that as the normal
* DOES NOT SUPPORT non-axis-aligned boxes
*
*
* 			   \		   /
* 				\		  /
* 				 \	 a	 /
*				  \		/
* 					 _
* 			a		| |       a
* 					 -
* 				  /     \
* 				 /   a   \
* 				/	      \
* 			   /	       \
*
*
*/
Util::Vector SocialForcesAgent::calcWallNormal(SteerLib::ObstacleInterface* obs)
{
	Util::AxisAlignedBox box = obs->getBounds();
	if (position().x > box.xmax)
	{
		if (position().z > box.zmax)
		{
			if (fabs(position().z - box.zmax) >
				fabs(position().x - box.xmax))
			{
				return Util::Vector(0, 0, 1);
			}
			else
			{
				return Util::Vector(1, 0, 0);
			}

		}
		else if (position().z < box.zmin)
		{
			if (fabs(position().z - box.zmin) >
				fabs(position().x - box.xmax))
			{
				return Util::Vector(0, 0, -1);
			}
			else
			{
				return Util::Vector(1, 0, 0);
			}

		}
		else
		{ // in between zmin and zmax
			return Util::Vector(1, 0, 0);
		}

	}
	else if (position().x < box.xmin)
	{
		if (position().z > box.zmax)
		{
			if (fabs(position().z - box.zmax) >
				fabs(position().x - box.xmin))
			{
				return Util::Vector(0, 0, 1);
			}
			else
			{
				return Util::Vector(-1, 0, 0);
			}

		}
		else if (position().z < box.zmin)
		{
			if (fabs(position().z - box.zmin) >
				fabs(position().x - box.xmin))
			{
				return Util::Vector(0, 0, -1);
			}
			else
			{
				return Util::Vector(-1, 0, 0);
			}

		}
		else
		{ // in between zmin and zmax
			return Util::Vector(-1, 0, 0);
		}
	}
	else // between xmin and xmax
	{
		if (position().z > box.zmax)
		{
			return Util::Vector(0, 0, 1);
		}
		else if (position().z < box.zmin)
		{
			return Util::Vector(0, 0, -1);
		}
		else
		{ // What do we do if the agent is inside the wall?? Lazy Normal
			return calcObsNormal(obs);
		}
	}

}

/**
* Treats Obstacles as a circle and calculates normal
*/
Util::Vector SocialForcesAgent::calcObsNormal(SteerLib::ObstacleInterface* obs)
{
	Util::AxisAlignedBox box = obs->getBounds();
	Util::Point obs_centre = Util::Point((box.xmax + box.xmin) / 2, (box.ymax + box.ymin) / 2,
		(box.zmax + box.zmin) / 2);
	return normalize(position() - obs_centre);
}

/*
void SocialForcesAgent::computeNeighbors()
{
agentNeighbors_.clear();

if (_SocialForcesParams.rvo_max_neighbors > 0) {
// std::cout << "About to segfault" << std::endl;
dynamic_cast<SocialForcesAIModule *>(rvoModule)->kdTree_->computeAgentNeighbors(this, _SocialForcesParams.rvo_neighbor_distance * _SocialForcesParams.rvo_neighbor_distance);
// std::cout << "Made it past segfault" << std::endl;
}
}*/

void SocialForcesAgent::pursueAndEvade_acceleration(float dt, Util::Vector &_acceleration)
{

	Util::Vector pursue_accel, evade_accel;
	std::set<SteerLib::SpatialDatabaseItemPtr> _neighbors;
	SocialForcesAgent * tmp_agent;
	double targetNum = 0;
	getSimulationEngine()->getSpatialDatabase()->getItemsInRange(_neighbors,
		_position.x - (this->_radius + _SocialForcesParams.sf_query_radius),
		_position.x + (this->_radius + _SocialForcesParams.sf_query_radius),
		_position.z - (this->_radius + _SocialForcesParams.sf_query_radius),
		_position.z + (this->_radius + _SocialForcesParams.sf_query_radius),
		dynamic_cast<SteerLib::SpatialDatabaseItemPtr>(this));
	pursue_accel.zero();
	evade_accel.zero();

	for (std::set<SteerLib::SpatialDatabaseItemPtr>::iterator neighbor =
		_neighbors.begin(); neighbor != _neighbors.end(); neighbor++)
	{

		if ((*neighbor)->isAgent())
		{
			tmp_agent = dynamic_cast<SocialForcesAgent*>(*neighbor);
			Util::Vector future_pos;
			future_pos.x = (tmp_agent->position()).x;
			future_pos.y = (tmp_agent->position()).y;
			future_pos.z = (tmp_agent->position()).z;
			future_pos = future_pos + dt * (tmp_agent->velocity());
			std::string neighbor_name = tmp_agent->agentName;


			if (FleeTarget.find(neighbor_name) != FleeTarget.end())
			{
				Util::Vector future_v;
				future_v.x = future_pos.x - (position()).x;
				future_v.y = future_pos.y - (position()).y;
				future_v.z = future_pos.z - (position()).z;
				future_v = sf_max_speed * normalize(future_v);
				evade_accel = evade_accel + (future_v - velocity());
				targetNum++;
			}
			if (SeekTarget.find(neighbor_name) != SeekTarget.end())
			{
				Util::Vector future_v;
				future_v.x = future_pos.x - (position()).x;
				future_v.y = future_pos.y - (position()).y;
				future_v.z = future_pos.z - (position()).z;
				future_v = sf_max_speed * normalize(future_v);
				pursue_accel = pursue_accel + (future_v - velocity());
				targetNum++;
			}
		}
	}

	if (targetNum > 0)
	{
		pursue_accel = (1 / targetNum) * pursue_accel;
		evade_accel = -(1 / targetNum) * evade_accel;
	}
	_acceleration = pursue_accel + evade_accel;
}

Util::Vector SocialForcesAgent::seek_acceleration(Util::Vector target)
{
	Util::Vector seek_acceleration;
	Util::Vector future_v;
	future_v.x = target.x - (position()).x;
	future_v.y = target.y - (position()).y;
	future_v.z = target.z - (position()).z;

	future_v = sf_max_speed * normalize(future_v);
	seek_acceleration = (future_v - velocity());

	return seek_acceleration;
}

void SocialForcesAgent::Wallfollower(float dt, Util::Vector &_acceleration)
{

	std::set<SteerLib::SpatialDatabaseItemPtr> _neighbors;
	getSimulationEngine()->getSpatialDatabase()->getItemsInRange(_neighbors,
		_position.x - (this->_radius + _SocialForcesParams.sf_query_radius),
		_position.x + (this->_radius + _SocialForcesParams.sf_query_radius),
		_position.z - (this->_radius + _SocialForcesParams.sf_query_radius),
		_position.z + (this->_radius + _SocialForcesParams.sf_query_radius),
		dynamic_cast<SteerLib::SpatialDatabaseItemPtr>(this));

	SteerLib::ObstacleInterface * tmp_ob;
	Util::Vector normal;
	std::pair<float, Util::Point> min_stuff;
	float minimum_dis;
	Util::Point projection;

	Util::Point future_pos;
	//    future_pos.x = (position()).x + dt * (velocity()).x;
	//    future_pos.y = (position()).y + dt * (velocity()).y;
	//    future_pos.z = (position()).z + dt * (velocity()).z;
	future_pos = position() + velocity();

	bool tag = false;
	for (std::set<SteerLib::SpatialDatabaseItemPtr>::iterator neighbour = _neighbors.begin(); neighbour != _neighbors.end(); neighbour++)
		// for (std::set<SteerLib::ObstacleInterface * >::iterator tmp_o = _neighbors.begin();  tmp_o != _neighbors.end();  tmp_o++)
	{
		if (!(*neighbour)->isAgent())
		{
			tmp_ob = dynamic_cast<SteerLib::ObstacleInterface *>(*neighbour);
		}
		else
		{
			continue;
		}
		Util::Vector wall_normal = calcWallNormal(tmp_ob);
		std::pair<Util::Point, Util::Point> line = calcWallPointsFromNormal(tmp_ob, wall_normal);
		min_stuff = minimum_distance(line.first, line.second, future_pos);// wall distance
		if (!tag) {
			minimum_dis = min_stuff.first;
			projection = min_stuff.second;
			normal = wall_normal;
			tag = true;
		}
		if (min_stuff.first < minimum_dis) {
			minimum_dis = min_stuff.first;
			projection = min_stuff.second;
			normal = wall_normal;
		}
	}

	Util::Point target_pos;
	//    target_pos.x = projection.x + 2 * normal.x;
	//    target_pos.y = projection.y + 2 * normal.y;
	//    target_pos.z = projection.z + 2 * normal.z;
	target_pos = projection + 2 * normal;

	Util::Vector desire_v;
	//    desire_v.x = target_pos.x - (position()).x;
	//    desire_v.y = target_pos.y - (position()).y;
	//    desire_v.z = target_pos.z - (position()).z;
	desire_v = target_pos - position();
	desire_v = sf_max_speed * normalize(desire_v);
	_acceleration = desire_v - velocity();
}

void SocialForcesAgent::Queueing(Util::Vector &_acceleration)
{
	_acceleration.zero();
	std::set<SteerLib::SpatialDatabaseItemPtr> _neighbors;
	getSimulationEngine()->getSpatialDatabase()->getItemsInRange(_neighbors,
		_position.x - (this->_radius + _SocialForcesParams.sf_query_radius),
		_position.x + (this->_radius + _SocialForcesParams.sf_query_radius),
		_position.z - (this->_radius + _SocialForcesParams.sf_query_radius),
		_position.z + (this->_radius + _SocialForcesParams.sf_query_radius),
		dynamic_cast<SteerLib::SpatialDatabaseItemPtr>(this));

	SocialForcesAgent * tmp_agent;
	for (std::set<SteerLib::SpatialDatabaseItemPtr>::iterator neighbour = _neighbors.begin(); neighbour != _neighbors.end(); neighbour++)
		// for (std::set<SteerLib::ObstacleInterface * >::iterator tmp_o = _neighbors.begin();  tmp_o != _neighbors.end();  tmp_o++)
	{
		if ((*neighbour)->isAgent())
		{
			tmp_agent = dynamic_cast<SocialForcesAgent*>(*neighbour);
		}
		else
		{
			continue;
		}
		Util::Point tmp_pos = tmp_agent->position();
		float distance = distanceBetween(tmp_pos, position());
		float frontjudge = (tmp_pos - position()) * forward() / (tmp_pos - position()).length();
		if (distance < 1.5 && frontjudge > 0 && tmp_agent->velocity().length() < velocity().length()) {
			_acceleration += (position() - tmp_pos) / distance;
		}
		else
		{
			continue;
		}
	}
	_acceleration = clamp(_acceleration, velocity().length());
}


void SocialForcesAgent::updateAI(float timeStamp, float dt, unsigned int frameNumber)
{
	// std::cout << "_SocialForcesParams.rvo_max_speed " << _SocialForcesParams._SocialForcesParams.rvo_max_speed << std::endl;
	Util::AutomaticFunctionProfiler profileThisFunction(&SocialForcesGlobals::gPhaseProfilers->aiProfiler);
	Util::Vector goalDirection;
	Util::Vector _acceleration, _acceleration_1;
	Util::Vector prefForce;
	if (!enabled())
	{
		return;
	}

	Util::AxisAlignedBox oldBounds(_position.x - _radius, _position.x + _radius, 0.0f, 0.0f, _position.z - _radius, _position.z + _radius);

	if (_goalQueue.size() > 0)
	{
		SteerLib::AgentGoalInfo goalInfo = _goalQueue.front();
		// std::cout << "midtermpath empty: " << _midTermPath.empty() << std::endl;
		if (!_midTermPath.empty() && (!this->hasLineOfSightTo(goalInfo.targetLocation)))
		{
			if (reachedCurrentWaypoint())
			{
				this->updateMidTermPath();
			}

			this->updateLocalTarget();

			goalDirection = normalize(_currentLocalTarget - position());

		}
		else
		{
			goalDirection = normalize(goalInfo.targetLocation - position());
		}
		// _prefVelocity = goalDirection * PERFERED_SPEED;
		prefForce = (((goalDirection * 1.3) - velocity()) / (_SocialForcesParams.sf_acceleration / dt)); //assumption here
	}
	Util::Vector repulsionForce = calcRepulsionForce(dt);
	if (repulsionForce.x != repulsionForce.x)
	{
		std::cout << "Found some nan" << std::endl;
		repulsionForce = velocity();
		// throw GenericException("SocialForces numerical issue");
	}
	Util::Vector proximityForce = calcProximityForce(dt);

	//SocialForcesAgent::pursueAndEvade_acceleration(dt, _acceleration_1);
	//SocialForcesAgent::Wallfollower(dt, _acceleration_1);
	SocialForcesAgent::Queueing(_acceleration_1);
	_acceleration = ((prefForce + repulsionForce + proximityForce) / MASS * AGENT_ACCELERATION_PERCENTAGE) + (_acceleration_1 *INDIV_BEHAVIOUR_PERCENTAGE);

	_velocity = velocity() + _acceleration;
	_velocity = clamp(velocity(), _SocialForcesParams.sf_max_speed);
	_velocity.y = 0.0f;
	_position = position() + (velocity() * dt);
	/****/
	//prefForce = prefForce + velocity();
	/****/
	// _velocity = prefForce;


	// #define _DEBUG_ 1
	//#ifdef _DEBUG_
	//	std::cout << "agent" << id() << " repulsion force " << repulsionForce << std::endl;
	//	std::cout << "agent" << id() << " proximity force " << proximityForce << std::endl;
	//	std::cout << "agent" << id() << " pref force " << prefForce << std::endl;
	//#endif
	// _velocity = _newVelocity;
	//	int alpha=1;
	//	if ( repulsionForce.length() > 0.0)
	//	{
	//		alpha=0;
	//	}
	//
	//	_velocity = (prefForce) + repulsionForce + proximityForce;
	//	// _velocity = (prefForce);
	//	// _velocity = velocity() + repulsionForce + proximityForce;
	//
	//	_velocity = clamp(velocity(), _SocialForcesParams.sf_max_speed);
	//	_velocity.y=0.0f;
	//#ifdef _DEBUG_
	//	std::cout << "agent" << id() << " speed is " << velocity().length() << std::endl;
	//#endif
	//	_position = position() + (velocity() * dt);

	// A grid database update should always be done right after the new position of the agent is calculated
	/*
	* Or when the agent is removed for example its true location will not reflect its location in the grid database.
	* Not only that but this error will appear random depending on how well the agent lines up with the grid database
	* boundaries when removed.
	*/
	// std::cout << "Updating agent" << this->id() << " at " << this->position() << std::endl;
	Util::AxisAlignedBox newBounds(_position.x - _radius, _position.x + _radius, 0.0f, 0.0f, _position.z - _radius, _position.z + _radius);
	getSimulationEngine()->getSpatialDatabase()->updateObject(this, oldBounds, newBounds);

	/*
	if ( ( !_waypoints.empty() ) && (_waypoints.front() - position()).length() < radius()*WAYPOINT_THRESHOLD_MULTIPLIER)
	{
	_waypoints.erase(_waypoints.begin());
	}
	*/
	/*
	* Now do the conversion from SocialForcesAgent into the SteerSuite coordinates
	*/
	// _velocity.y = 0.0f;

	if (_goalQueue.size() <= 0)
		return;

	AgentGoalInfo goalInfo = _goalQueue.front();
	if ((goalInfo.targetLocation - position()).length() < radius()*GOAL_THRESHOLD_MULTIPLIER ||
		(goalInfo.goalType == GOAL_TYPE_AXIS_ALIGNED_BOX_GOAL &&
		Util::boxOverlapsCircle2D(goalInfo.targetRegion.xmin, goalInfo.targetRegion.xmax,
		goalInfo.targetRegion.zmin, goalInfo.targetRegion.zmax, this->position(), this->radius())))
	{
		_goalQueue.pop();
		// std::cout << "Made it to a goal" << std::endl;
		if (_goalQueue.size() != 0)
		{
			// in this case, there are still more goals, so start steering to the next goal.
			goalDirection = _goalQueue.front().targetLocation - _position;
			_prefVelocity = Util::Vector(goalDirection.x, 0.0f, goalDirection.z);
		}
		else
		{
			// in this case, there are no more goals, so disable the agent and remove it from the spatial database.
			disable();
			return;
		}
	}

	// Hear the 2D solution from RVO is converted into the 3D used by SteerSuite
	// _velocity = Vector(velocity().x, 0.0f, velocity().z);
	if (velocity().lengthSquared() > 0.0)
	{
		// Only assign forward direction if agent is moving
		// Otherwise keep last forward
		_forward = normalize(_velocity);
	}
	// _position = _position + (_velocity * dt);

}


void SocialForcesAgent::draw()
{
#ifdef ENABLE_GUI
	AgentInterface::draw();
	// if the agent is selected, do some annotations just for demonstration

#ifdef DRAW_COLLISIONS
	std::set<SteerLib::SpatialDatabaseItemPtr> _neighbors;
	getSimulationEngine()->getSpatialDatabase()->getItemsInRange(_neighbors, _position.x - (this->_radius * 3), _position.x + (this->_radius * 3),
		_position.z - (this->_radius * 3), _position.z + (this->_radius * 3), dynamic_cast<SteerLib::SpatialDatabaseItemPtr>(this));

	for (std::set<SteerLib::SpatialDatabaseItemPtr>::iterator neighbor = _neighbors.begin(); neighbor != _neighbors.end(); neighbor++)
	{
		if ((*neighbor)->isAgent() && (*neighbor)->computePenetration(this->position(), this->_radius) > 0.00001f)
		{
			Util::DrawLib::drawStar(
				this->position()
				+
				(
				(
				dynamic_cast<AgentInterface*>(*neighbor)->position()
				-
				this->position()
				)
				/ 2), Util::Vector(1, 0, 0), 0.8f, gRed);
			// Util::DrawLib::drawStar(this->position(), Util::Vector(1,0,0), 1.14f, gRed);
		}
	}
#endif
#ifdef DRAW_HISTORIES
	__oldPositions.push_back(position());
	int points = 0;
	float mostPoints = 100.0f;
	while (__oldPositions.size() > mostPoints)
	{
		__oldPositions.pop_front();
	}
	for (int q = __oldPositions.size() - 1; q > 0 && __oldPositions.size() > 1; q--)
	{
		DrawLib::drawLineAlpha(__oldPositions.at(q), __oldPositions.at(q - 1), gBlack, q / (float)__oldPositions.size());
	}

#endif

#ifdef DRAW_ANNOTATIONS

	for (int i = 0; (_waypoints.size() > 1) && (i < (_waypoints.size() - 1)); i++)
	{
		if (_gEngine->isAgentSelected(this))
		{
			DrawLib::drawLine(_waypoints.at(i), _waypoints.at(i + 1), gYellow);
		}
		else
		{
			//DrawLib::drawLine(_waypoints.at(i), _waypoints.at(i+1), gBlack);
		}
	}

	for (int i = 0; i < (_waypoints.size()); i++)
	{
		DrawLib::drawStar(_waypoints.at(i), Util::Vector(1, 0, 0), 0.34f, gBlue);
	}

	for (int i = 0; (_midTermPath.size() > 1) && (i < (_midTermPath.size() - 1)); i++)
	{
		if (_gEngine->isAgentSelected(this))
		{
			DrawLib::drawLine(_midTermPath.at(i), _midTermPath.at(i + 1), gMagenta);
		}
		else
		{
			// DrawLib::drawLine(_midTermPath.at(i), _midTermPath.at(i+1), gCyan);
		}
	}

	DrawLib::drawLine(position(), this->_currentLocalTarget, gGray10);
	DrawLib::drawStar(this->_currentLocalTarget + Util::Vector(0, 0.001, 0), Util::Vector(1, 0, 0), 0.24f, gGray10);

	/*
	// draw normals and closest points on walls
	std::set<SteerLib::ObstacleInterface * > tmp_obs = gEngine->getObstacles();

	for (std::set<SteerLib::ObstacleInterface * >::iterator tmp_o = tmp_obs.begin();  tmp_o != tmp_obs.end();  tmp_o++)
	{
	Util::Vector normal = calcWallNormal( *tmp_o );
	std::pair<Util::Point,Util::Point> line = calcWallPointsFromNormal(* tmp_o, normal);
	Util::Point midpoint = Util::Point((line.first.x+line.second.x)/2, ((line.first.y+line.second.y)/2)+1,
	(line.first.z+line.second.z)/2);
	DrawLib::drawLine(midpoint, midpoint+normal, gGreen);

	// Draw the closes point as well
	std::pair<float, Util::Point> min_stuff = minimum_distance(line.first, line.second, position());
	DrawLib::drawStar(min_stuff.second, Util::Vector(1,0,0), 0.34f, gGreen);
	}
	*/

#endif

#endif
}

