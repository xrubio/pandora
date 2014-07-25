#ifndef __QuantumWorld_hxx
#define __QuantumWorld_hxx

#include <World.hxx>

namespace QuantumExperiment 
{

class QuantumWorldConfig;

enum Rasters
{
	// quality of soil, between 0 and 10
	eResources,
	eKnowledge
};

class QuantumWorld: public Engine::World
{
	void createAgents();
	void createRasters();

public:
	QuantumWorld( QuantumWorldConfig * config, Engine::Scheduler * scheduler = 0);
	virtual ~QuantumWorld();
	void stepEnvironment();
};

} // namespace QuantumExperiment

#endif // __QuantumWorld_hxx
