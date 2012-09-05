
#include "MyWorld.hxx"
#include "MyAgent.hxx"
#include <Simulation.hxx>
/*
#include <Raster.hxx>
#include <Point2D.hxx>
#include <GeneralState.hxx>
*/

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

	/*
	Engine::Point2D<int> index(0,0);
	for(index._x=0; index._x<_overlapBoundaries._size._x; index._x++)
	{
		for(index._y=0; index._y<_overlapBoundaries._size._y; index._y++)
		{
			int value = Engine::GeneralState::statistics().getUniformDistValue(0,5);
			getDynamicRaster("resources").setMaxValue(index, value);
		}
	}
	updateRasterToMaxValues("resources");
	*/
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

