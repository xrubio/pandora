#ifndef __QuantumWorld_hxx
#define __QuantumWorld_hxx

#include <World.hxx>
#include <GeneralState.hxx>
#include "QuantumWorldConfig.hxx"

namespace Engine
{
	class Simulation;
}

namespace QuantumExperiment 
{

enum Rasters
{
	// quality of soil, between 0 and 10
	eResources,
	eKnowledge
};

class QuantumWorld: public Engine::World
{
	const QuantumWorldConfig & _config;

	void createAgents();
	void createRasters();

public:
	QuantumWorld( const QuantumWorldConfig &config, Engine::Simulation & simulation, Engine::Scheduler * scheduler = 0);
	virtual ~QuantumWorld();
	void stepEnvironment();
};

} // namespace QuantumExperiment

#endif // __QuantumWorld_hxx
