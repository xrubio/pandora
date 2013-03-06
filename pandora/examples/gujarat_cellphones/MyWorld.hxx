#ifndef __MyWorld_hxx
#define __MyWorld_hxx

#include <World.hxx>
#include "GeneralState.hxx"
#include "MyVillage.hxx"
#include "MyWorldConfig.hxx"
#include "Climate.hxx"

namespace Engine
{
	class Simulation;
}

namespace GujaratCellphones {

enum Rasters
{
	// quality of soil, between 0 and 10 (being 5 the standard)
	eSoilQuality,
	eResources,
	eResourcesFraction
};

class MyAgent;

class MyWorld : public Engine::World
{
	int _agentsCounter;
	Climate _climate;
	std::vector<std::vector<int> > _distancesMatrix;
	std::vector<std::string> _idsDeletedAgents;
	std::vector<MyVillage> _villages;

	void createAgents();
	void createRasters();
	void createVillages();
	void generateDistancesMatrix(int size);
	MyVillage getVillage(int id);
	void initSocialNetwork();
	void initVillage(int id, int x, int y);
	void recomputeYearlyBiomass();
	void updateResources();

	// maximum resources for soil quality (from 0 to 10)
	std::vector<float> _maxResources;
	// daily decrease for soil quality (from 0 to 10)
	std::vector<float> _dailyDecrease;
public:
	MyWorldConfig _config;

	MyWorld(Engine::Simulation &simulation, MyWorldConfig &config);
	virtual ~MyWorld();
	void addDeletedAgent(std::string id);
	bool agentHasBeenDeleted(std::string id);
	MyAgent * createAgent(int idVillage, bool initialAgent);
	int getAgentsCounter();
	int getDistance(int x1, int y1, int x2, int y2);
	std::vector<std::string> getIdsAgentsInCell(int x, int y);
	std::vector<std::string> getIdsExistingAgents();
	double getMaximumAvgCellsSharedPerCall();
	void stepEnvironment();

	// the first step of a year simultes the entire wet season
	bool isWetSeason() const;
	// remaining days until next wet season
	int daysUntilWetSeason() const;
};

} // namespace Tutorial 

#endif // __MyWorld_hxx
