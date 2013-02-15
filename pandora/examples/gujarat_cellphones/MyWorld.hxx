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
	std::vector<MyVillage> _villages;

	void createRasters();
	void createVillages();
	void createAgents();
	void initVillage(int id, int x, int y);
	MyVillage getVillage(int id);
	int _agentsCounter;

public:
	Examples::MyWorldConfig _config;
	MyWorld(Engine::Simulation &simulation, Examples::MyWorldConfig &config);
	virtual ~MyWorld();
	std::string createAgent(int idVillage);
	double getMaximumAvgCellsSharedPerCall();
};

} // namespace Tutorial 

#endif // __MyWorld_hxx
