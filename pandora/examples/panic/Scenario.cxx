
#include "Scenario.hxx"
#include "PanicAgent.hxx"

namespace Panic
{

Scenario::Scenario(Engine::Simulation &simulation, ScenarioConfig &config) : World(simulation, 1, true, config._resultsFile)
{
	_config = config;
}

Scenario::~Scenario()
{
}

void Scenario::createAgents()
{
	for(int i=0; i<_config._numAgents; i++)
	{
		std::ostringstream oss;
		oss << "PanicAgent_"<<i;
		PanicAgent * agent = new PanicAgent(oss.str());
		addAgent(agent);
	
		agent->setRandomPosition();
		while(getDynamicRaster(eObstacles).getValue(agent->getPosition())==1)
		{
			agent->setRandomPosition();
		}
		//agent->setPosition(Engine::Point2D<int>(99,99));
	}
}

void Scenario::createRasters()
{	
	registerDynamicRaster("obstacles", false, eObstacles);
	getDynamicRaster(eObstacles).setInitValues(0, 0, 0);
	getDynamicRaster(eObstacles).setMaxValue(1);

	Engine::Point2D<int> index(0,0);
	int maxValue = _overlapBoundaries._size._x;
	for(index._x=0; index._x<_overlapBoundaries._size._x; index._x++)
	{
		for(index._y=0; index._y<_overlapBoundaries._size._y; index._y++)
		{
			if(index._x==0 || index._y==0)
			{
				getDynamicRaster(eObstacles).setMaxValue(index, 1);
			}
			if(index._x==_overlapBoundaries._size._x-1 || index._y==_overlapBoundaries._size._y-1)
			{
				getDynamicRaster(eObstacles).setMaxValue(index, 1);
			}
		}
	}

	// 1 exit at point 0,0
	registerDynamicRaster("exits", false, eExits);
	getDynamicRaster(eExits).setInitValues(0, 0, 0);
	getDynamicRaster(eExits).setMaxValue(1);

	getDynamicRaster(eExits).setMaxValue(Engine::Point2D<int>(0,0),1);
	getDynamicRaster(eObstacles).setMaxValue(Engine::Point2D<int>(0,0),0);

	updateRasterToMaxValues(eObstacles);
	updateRasterToMaxValues(eExits);
}

} // namespace Panic

