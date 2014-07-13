#ifndef __HerderWorld_hxx
#define __HerderWorld_hxx

#include <World.hxx>
#include <GeneralState.hxx>
#include "Village.hxx"
#include "HerderWorldConfig.hxx"
#include "Climate.hxx"

namespace Engine
{
	class Simulation;
}

namespace GujaratCellphones
{

enum Rasters
{
	// quality of soil, between 0 and 10 (being 5 the standard)
	eSoilQuality,
	eResources,
};

class Herder;

class HerderWorld : public Engine::World
{
	HerderWorldConfig _config;
	Climate _climate;
	// maximum resources for soil quality (from 0 to 10)
	std::vector<float> _maxResources;

	void createAgents();
	void createRasters();

	// environment
	void recomputeYearlyBiomass();
	void updateResources();
	// the first step of a year simulates the entire wet season
	bool isWetSeason() const;

public:
	HerderWorld( Engine::Simulation &simulation, HerderWorldConfig &config );
	virtual ~HerderWorld();
	void stepEnvironment();
	int daysUntilWetSeason() const; // remaining days until next wet season
	const HerderWorldConfig & getConfig() const;
};

} // namespace Tutorial 

#endif // __HerderWorld_hxx
