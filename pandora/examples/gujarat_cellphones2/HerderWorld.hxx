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
	eResourcesFraction
};

class Herder;

class HerderWorld : public Engine::World
{
	HerderWorldConfig _config;
	Climate _climate;
//	int _agentsCounter;

	//std::vector<std::vector<int> > _distancesMatrix;
	//std::vector<std::string> _idsDeletedAgents;
	//std::vector<Village *> _villages;

	void createAgents();
	void createRasters();
//	void createVillages();
//	void generateDistancesMatrix(int size);
//	Village getVillage(int id);
//	void initSocialNetwork();
//	void initVillage(int id, int x, int y);

	// environment
	// maximum resources for soil quality (from 0 to 10)
	std::vector<float> _maxResources;
	// daily decrease for soil quality (from 0 to 10)
	std::vector<float> _dailyDecrease;
	void recomputeYearlyBiomass();
	void updateResources();

	// the first step of a year simulates the entire wet season
	bool isWetSeason() const;

public:
	HerderWorld(Engine::Simulation &simulation, HerderWorldConfig &config);
	virtual ~HerderWorld();
	//void addDeletedAgent(std::string id);
	//bool agentHasBeenDeleted(std::string id);
	//Herder * createAgent(int idVillage, bool initialAgent);
	//int getAgentsCounter();
	//int getDistance(int x1, int y1, int x2, int y2);
	//std::vector<std::string> getIdsAgentsInCell(int x, int y);
	//std::vector<std::string> getIdsExistingAgents();
	//double getMaximumAvgCellsSharedPerCall();
	void stepEnvironment();

	// remaining days until next wet season
	int daysUntilWetSeason() const;
};

} // namespace Tutorial 

#endif // __HerderWorld_hxx
