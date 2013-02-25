#ifndef __MyWorld_hxx
#define __MyWorld_hxx

#include <World.hxx>
#include "MyWorldConfig.hxx"
#include "MyVillage.hxx"
#include <GeneralState.hxx>

namespace Engine
{
	class Simulation;
}

namespace Tutorial 
{

class MyWorld : public Engine::World
{
	int _agentsCounter;
	std::vector<std::string> _idsDeletedAgents;
	std::vector<MyVillage> _villages;

	void createAgents();
	void createRasters();
	void createVillages();
	MyVillage getVillage(int id);
	void initSocialNetwork();
	void initVillage(int id, int x, int y);

public:
	Examples::MyWorldConfig _config;

	MyWorld(Engine::Simulation &simulation, Examples::MyWorldConfig &config);
	virtual ~MyWorld();
	void addDeletedAgent(std::string id);
	bool agentHasBeenDeleted(std::string id);
	std::string createAgent(int idVillage, bool initialAgent);
	int getAgentsCounter();
	std::vector<std::string> getIdsAgentsInCell(int x, int y);
	std::vector<std::string> getIdsExistingAgents();
	double getMaximumAvgCellsSharedPerCall();
};

} // namespace Tutorial 

#endif // __MyWorld_hxx
