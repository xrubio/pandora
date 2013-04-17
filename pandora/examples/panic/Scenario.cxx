
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
		//agent->setPosition(Engine::Point2D<int>(76,423));
		computeShortestExit(*agent);
	}
}

void Scenario::computeShortestExit(PanicAgent & agent )
{
	std::vector< Engine::Point2D<int> > possibleExits;
	float minDistance = std::numeric_limits<float>::max();

	for(ExitsList::const_iterator it=_exits.begin(); it!=_exits.end(); it++)
	{
		float distance = agent.getPosition().distance(*it);
		if(distance<minDistance)
		{
			possibleExits.clear();
			possibleExits.push_back(*it);
			minDistance = distance;
		}
		else if(distance==minDistance)
		{
			possibleExits.push_back(*it);
		}
	}
	std::random_shuffle(possibleExits.begin(), possibleExits.end());
	agent.setExit(possibleExits.at(0));
	std::cout << "agent: " << agent << " will go to: " << possibleExits.at(0) << std::endl;
}

void Scenario::createRasters()
{
	registerStaticRaster("orto5", true, eOrto);
	Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster(eOrto), "resources/orto5m.tiff", this);	

	registerStaticRaster("obstacles", true, eObstacles);
	Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster(eObstacles), _config._obstacleFile, this);	

	registerDynamicRaster("exits", true, eExits);
	getDynamicRaster(eExits).setInitValues(0, 0, 0);
	getDynamicRaster(eExits).setMaxValue(1);

	Engine::Point2D<int> index(0,0);
	int maxValue = _overlapBoundaries._size._x;
	for(index._x=0; index._x<_overlapBoundaries._size._x; index._x++)
	{
		for(index._y=0; index._y<_overlapBoundaries._size._y; index._y++)
		{
			if(getStaticRaster(eObstacles).getValue(index)!=0)
			{
				continue;
			}

			if(index._x==0 || index._y==0)
			{
				getDynamicRaster(eExits).setMaxValue(index, 1);
			}
			if(index._x==_overlapBoundaries._size._x-1 || index._y==_overlapBoundaries._size._y-1)
			{
				getDynamicRaster(eExits).setMaxValue(index, 1);
			}
		}
	}
	updateRasterToMaxValues(eExits);
	fillExitList();
}

void Scenario::fillExitList()
{
	Engine::Point2D<int> index;
	for(index._x=0; index._x<_overlapBoundaries._size._x; index._x++)
	{
		for(index._y=0; index._y<_overlapBoundaries._size._y; index._y++)
		{
			if(getDynamicRaster(eExits).getValue(index)==0)
			{
				continue;
			}
			_exits.push_back(index);
		}
	}
}

} // namespace Panic

