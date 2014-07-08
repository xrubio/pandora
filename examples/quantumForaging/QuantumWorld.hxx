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
	QuantumWorldConfig _config;

	void createAgents();
	void createRasters();

	// environment
//	void recomputeYearlyBiomass();
//	void updateResources();

public:
	QuantumWorld( Engine::Simulation &simulation, QuantumWorldConfig &config );
	virtual ~QuantumWorld();
	void stepEnvironment();
};

} // namespace QuantumExperiment

#endif // __QuantumWorld_hxx
