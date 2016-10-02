/*!
*
* \author VaHiD AzIzI
*
*/

#ifndef __COLLISION_AGENT__
#define __COLLISION_AGENT__

/// @file CollisionAgent.h
/// @brief Declares the CollisionAgent class.

#include <queue>
#include "SteerLib.h"
// #include "CollisionAgent.h"
#include "CollisionAIModule.h"

/**
 * @brief An example agent with very basic AI, that is part of the collisionAI plugin.
 *
 * This agent performs extremely collision AI using forces and Euler integration, simply
 * steering towards static goals without avoiding any other agents or static obstacles.
 * Agents that are "selected" in the GUI will have some collision annotations that
 * show how the spatial database and engine interface can be used.
 *
 * This class is instantiated when the engine calls CollisionAIModule::createAgent().
 *
 */
class CollisionAgent : public SteerLib::AgentInterface
{
public:
    CollisionAgent();
    ~CollisionAgent();
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
    void addGoal(const SteerLib::AgentGoalInfo & newGoal) { throw Util::GenericException("addGoals() not implemented yet for CollisionAgent"); }
    void clearGoals() { throw Util::GenericException("clearGoals() not implemented yet for CollisionAgent"); }

	void insertAgentNeighbor(const SteerLib::AgentInterface *agent, float &rangeSq) { throw Util::GenericException("insertAgentNeighbor not implemented yet for BenchmarkAgent"); }
	void setParameters(SteerLib::Behaviour behave)
	{
		throw Util::GenericException("setParameters() not implemented yet for this Agent");
	}
	/// @name The SteerLib::SpatialDatabaseItemInterface
	/// @brief These functions are required so that the agent can be used by the SteerLib::GridDatabase2D spatial database;
	/// The Util namespace helper functions do the job nicely for basic circular agents.
	//@{
    bool intersects(const Util::Ray &r, float &t) { return false; }
    bool overlaps(const Util::Point & p, float radius) { return false; }
    float computePenetration(const Util::Point & p, float radius) { return false; }
	//@}


protected:
	/// Updates position, velocity, and orientation of the agent, given the force and dt time step.
	void _doEulerStep(const Util::Vector & steeringDecisionForce, float dt);

	bool _enabled;
	Util::Point __position;
	Util::Vector _velocity;
	Util::Vector _forward; // normalized version of velocity
	float _radius;
	std::queue<SteerLib::AgentGoalInfo> _goalQueue;

	virtual SteerLib::EngineInterface * getSimulationEngine();
};

#endif
