//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
// Copyright (c) 2015 Mahyar Khayatkhoei
//

#ifndef __CURVE_AGENT__
#define __CURVE_AGENT__

/// @file CurveAgent.h
/// @brief Declares the CurveAgent class.

#include <queue>
#include <vector>
#include "SteerLib.h"
#include "CurveAIModule.h"

class CurveAgent : public SteerLib::AgentInterface
{
public:
	CurveAgent();
	~CurveAgent();
	void reset(const SteerLib::AgentInitialConditions & initialConditions, SteerLib::EngineInterface * engineInfo);
	void updateAI(float timeStamp, float dt, unsigned int frameNumber);
	void disable();
	void draw();

	bool enabled() const { return _enabled; }
	Util::Point position() const { return __position; }
	Util::Vector forward() const { return _forward; }
	Util::Vector velocity() const { throw Util::GenericException("velocity() not implemented yet"); }
	float radius() const { return _radius; }
	const SteerLib::AgentGoalInfo & currentGoal() const { return _goalQueue.front(); }
	size_t id() const { return 0;}
	const std::queue<SteerLib::AgentGoalInfo> & agentGoals() const { throw Util::GenericException("agentGoals() not implemented yet"); }
	void addGoal(const SteerLib::AgentGoalInfo & newGoal) { throw Util::GenericException("addGoals() not implemented yet for CurveAgent"); }
	void clearGoals() { throw Util::GenericException("clearGoals() not implemented yet for CurveAgent"); }

	void insertAgentNeighbor(const SteerLib::AgentInterface *agent, float &rangeSq) { throw Util::GenericException("insertAgentNeighbor not implemented yet for BenchmarkAgent"); }
	void setParameters(SteerLib::Behaviour behave)
	{
		throw Util::GenericException("setParameters() not implemented yet for this Agent");
	}
	/// @name The SteerLib::SpatialDatabaseItemInterface
	/// @brief These functions are required so that the agent can be used by the SteerLib::GridDatabase2D spatial database;
	/// The Util namespace helper functions do the job nicely for basic circular agents.
	//@{
	bool intersects(const Util::Ray &r, float &t) { return Util::rayIntersectsCircle2D(__position, _radius, r, t); }
	bool overlaps(const Util::Point & p, float radius) { return Util::circleOverlapsCircle2D( __position, _radius, p, radius); }
	float computePenetration(const Util::Point & p, float radius) { return Util::computeCircleCirclePenetration2D( __position, _radius, p, radius); }
	//@}


protected:
	/// Updates position, velocity, and orientation of the agent, given the force and dt time step.
	void _doEulerStep(const Util::Vector & steeringDecisionForce, float dt);
	bool _enabled;
	Util::Point __startPosition;
	Util::Point __position;
	Util::Vector _velocity;
	Util::Vector _forward; // normalized version of velocity
	float _radius;
	std::vector<SteerLib::AgentGoalInfo> _goalQueue;
	Util::Curve curve;
	Util::Color agentColor;
	virtual SteerLib::EngineInterface * getSimulationEngine();
};

#endif