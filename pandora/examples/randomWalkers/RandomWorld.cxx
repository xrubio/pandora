
#include <RandomWorld.hxx>

#include <RandomWorldConfig.hxx>
#include <RandomAgent.hxx>
#include <Simulation.hxx>
#include <Raster.hxx>
#include <Point2D.hxx>
#include <GeneralState.hxx>

namespace Examples 
{

RandomWorld::RandomWorld( Engine::Simulation & simulation, const RandomWorldConfig & config ) : World(simulation, 1, false, config._resultsFile), _config(config)
{
}

RandomWorld::~RandomWorld()
{
}

void RandomWorld::createRasters()
{
	registerDynamicRaster("resources", true);
	getDynamicRaster("resources").setInitValues(0, 5, 0);
	
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
}

void RandomWorld::createAgents()
{
	for(int i=0; i<_config._numAgents; i++)
	{
		if((i%_simulation.getNumTasks())==_simulation.getId())
		{
			std::ostringstream oss;
			oss << "RandomAgent_" << i;
			RandomAgent * agent = new RandomAgent(oss.str());
			addAgent(agent);
			agent->setRandomPosition();
		}
	}
}

} // namespace Examples

