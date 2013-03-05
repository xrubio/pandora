#ifndef __MyWorld_hxx
#define __MyWorld_hxx

#include <World.hxx>
#include "GeneralState.hxx"
#include "MyVillage.hxx"
#include "MyWorldConfig.hxx"
#include "Climate.hxx"
#include "MyAgent.hxx"

namespace Engine
{
	class Simulation;
}

namespace GujaratCellphones {

enum Rasters
{
	eWeightWater,
	eResources,
	eResourcesFraction,
	eSoils
};

enum Soils
{
	WATER = 1,
	INTERDUNE = 2,
	DUNE = 3
};

class MyWorld : public Engine::World
{
	int _agentsCounter;
	std::vector<std::vector<Soils> > _cellSoils;
	Climate _climate;
	std::vector<float> _dailyDrySeasonBiomassDecrease;
	std::vector<float> _dailyRainSeasonBiomassIncrease;
	std::vector<std::vector<int> > _distancesMatrix;
	std::vector<std::string> _idsDeletedAgents;
	std::vector<float> _remainingBiomass;
	std::vector<MyVillage> _villages;
	std::vector<float> _yearlyBiomass;

	void createAgents();
	void createRasters();
	void createVillages();
	void generateDistancesMatrix(int size);
	float getBiomassVariation(bool wetSeason, Soils & cellSoil, const Engine::Point2D<int> & index) const;
	MyVillage getVillage(int id);
	void initCellSoils();
	void initSocialNetwork();
	void initVillage(int id, int x, int y);
	void recomputeYearlyBiomass();
	void updateRainfall();
	void updateResources();

public:
	MyWorldConfig _config;

	MyWorld(Engine::Simulation &simulation, MyWorldConfig &config);
	virtual ~MyWorld();
	void addDeletedAgent(std::string id);
	bool agentHasBeenDeleted(std::string id);
	std::string createAgent(int idVillage, bool initialAgent);
	int getAgentsCounter();
	Climate getClimate();
	int getDistance(int x1, int y1, int x2, int y2);
	std::vector<std::string> getIdsAgentsInCell(int x, int y);
	std::vector<std::string> getIdsExistingAgents();
	double getMaximumAvgCellsSharedPerCall();
	void stepEnvironment();
};

} // namespace Tutorial 

#endif // __MyWorld_hxx
