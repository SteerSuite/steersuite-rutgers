//
// Copyright (c) 2009-2015 Glen Berseth, Mubbasir Kapadia, Shawn Singh, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//


#ifndef __COMMAND_LINE_ENGINE_DRIVER_H__
#define __COMMAND_LINE_ENGINE_DRIVER_H__

/// @file CommandLineEngineDriver.h
/// @brief Declares the CommandLineEngineDriver class


#include "SteerLib.h"
#include "interfaces/EngineControllerInterface.h"
#include "simulation/SimulationEngine.h"

class STEERLIB_API CommandLineEngineDriver : public SteerLib::EngineControllerInterface
{
public:
	CommandLineEngineDriver();
	~CommandLineEngineDriver() {}
	void init(SteerLib::SimulationOptions * options);
	void finish();
	void run();
	const char * getData();
	LogData * getLogData();

	/// @name The EngineControllerInterface
	/// @brief The CommandLineEngineDriver does not support any of the engine controls.
	//@{
	virtual bool isStartupControlSupported() { return false; }
	virtual bool isPausingControlSupported() { return false; }
	virtual bool isPaused() { return false; }
	virtual void loadSimulation();
	virtual void unloadSimulation();
	virtual void startSimulation();
	virtual void stopSimulation();
	virtual void pauseSimulation() { throw Util::GenericException("CommandLineEngineDriver does not support pauseSimulation()."); }
	virtual void unpauseSimulation() { throw Util::GenericException("CommandLineEngineDriver does not support unpauseSimulation()."); }
	virtual void togglePausedState() { throw Util::GenericException("CommandLineEngineDriver does not support togglePausedState()."); }
	virtual void pauseAndStepOneFrame() { throw Util::GenericException("CommandLineEngineDriver does not support pauseAndStepOneFrame()."); }
	//@}

protected:
	bool _alreadyInitialized;
	SteerLib::SimulationEngine * _engine;

private:
	// These functions are kept here to protect us from mangling the instance.
	// Technically the CommandLineEngineDriver is not a singleton, though.
	CommandLineEngineDriver(const CommandLineEngineDriver & );  // not implemented, not copyable
	CommandLineEngineDriver& operator= (const CommandLineEngineDriver & );  // not implemented, not assignable
};

#endif
