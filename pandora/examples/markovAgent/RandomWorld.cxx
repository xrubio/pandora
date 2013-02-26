
#include <RandomWorld.hxx>

#include <RandomWorldConfig.hxx>
#include <MDPAgent.hxx>
#include <Simulation.hxx>
#include <Raster.hxx>
#include <Point2D.hxx>
#include <GeneralState.hxx>

namespace Examples 
{

RandomWorld::RandomWorld( Engine::Simulation & simulation, const RandomWorldConfig & config ) : World(simulation, 1, true, config._resultsFile), _config(config)
{
}

RandomWorld::~RandomWorld()
{
}

void RandomWorld::createRasters()
{
	registerDynamicRaster("resources", true);
	getDynamicRasterStr("resources").setInitValues(0, 100, 0);
	
	Engine::Point2D<int> index(0,0);
	for(index._x=0; index._x<_overlapBoundaries._size._x; index._x++)
	{
		for(index._y=0; index._y<_overlapBoundaries._size._y; index._y++)
		{
			int value = Engine::GeneralState::statistics().getUniformDistValue(0,5);
			//getDynamicRasterStr("resources").setMaxValue(index, 1+index._x);
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
			oss << "MDPAgent_" << i;
			MDPAgent * agent = new MDPAgent(oss.str(), _config._horizon, _config._width, _config._explorationBonus);
			addAgent(agent);
			agent->setRandomPosition();
			//agent->setPosition(Engine::Point2D<int>(0,0));
		}
	}
}

} // namespace Examples

