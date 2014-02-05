
#include "MyWorld.hxx"
#include "MyAgent.hxx"
#include <iostream>
#include <Simulation.hxx>

namespace Tutorial 
{

MyWorld::MyWorld( Engine::Simulation & simulation) : World(simulation, 1, false, "data/results.h5")
{
}

MyWorld::~MyWorld()
{
}

void MyWorld::createRasters()
{
	std::cout << "create rasters" << std::endl;
	registerDynamicRaster("resources", true);
	getDynamicRaster("resources").setInitValues(0, 10, 0);
}

void MyWorld::createAgents()
{
	std::cout << "create agents" << std::endl;	
	for(int i=0; i<10; i++)
	{
		if((i%_simulation.getNumTasks())==_simulation.getId())
		{
			std::ostringstream oss;
			oss << "MyAgent_" << i;
			MyAgent * agent = new MyAgent(oss.str());
			addAgent(agent);
			agent->setRandomPosition();
		}
	}
}

} // namespace Tutorial 

