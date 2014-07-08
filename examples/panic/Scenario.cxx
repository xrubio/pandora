
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

bool Scenario::maxCapacity( const Engine::Point2D<int> & position )
{
	// if random no problem
	if(_config._initAgentsDistributionType.compare("random")==0)
	{
		return false;
	}
	if(getValue("initAgentsDistribution", position)<=getValue(eNumAgents, position))
	{
		return true;
	}

	int randomValue = Engine::GeneralState::statistics().getUniformDistValue(0,getStaticRaster("initAgentsDistribution").getMaxValue());
	if(randomValue<=getValue("initAgentsDistribution", position))
	{
		return false;
	}
	return true;
}

void Scenario::createAgents()
{
	for(int i=0; i<_config._numAgents; i++)
	{
		std::ostringstream oss;
		oss << "PanicAgent_"<<i;
		PanicAgent * agent = new PanicAgent(oss.str(), _config);
		addAgent(agent);

		// avoid agent in obstacle and initial dense crowd
		agent->setRandomPosition();
		while((getValue(eObstacles, agent->getPosition())==1) || maxCapacity(agent->getPosition()))
		{
			agent->setRandomPosition();
		}
		//agent->setPosition(Engine::Point2D<int>(76,423));
		computeShortestExit(*agent);
		setValue(eNumAgents, agent->getPosition(), getValue(eNumAgents, agent->getPosition())+1);
	}
}

void Scenario::computeShortestExit(PanicAgent & agent )
{
	std::vector< Engine::Point2D<int> > possibleExits;
	int randomValue = Engine::GeneralState::statistics().getUniformDistValue(0,9);
	// probability 20% of not knowing the exit
	if(randomValue>_config._knowledge)
	{
		int randomExitIndex = Engine::GeneralState::statistics().getUniformDistValue(0, _exits.size()-1);
		ExitsList::const_iterator it=_exits.begin();
		for(int i=0; i<randomExitIndex; i++)
		{
			it++;
		}
		possibleExits.push_back(*it);
	}
	else
	{
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
	}
	std::random_shuffle(possibleExits.begin(), possibleExits.end());
	agent.setExit(possibleExits.at(0));
//	std::cout << "agent: " << agent << " will go to: " << possibleExits.at(0) << std::endl;
}

void Scenario::createRasters()
{
	registerDynamicRaster("obstacles", false, eObstacles);
	Engine::GeneralState::rasterLoader().fillGDALRaster(getDynamicRaster(eObstacles), _config._obstacleFile, getBoundaries());	
	getDynamicRaster(eObstacles).setMaxValue(1);

	registerDynamicRaster("exits", false, eExits);
	getDynamicRaster(eExits).setInitValues(0, 0, 0);
	getDynamicRaster(eExits).setMaxValue(1);
	
	registerDynamicRaster("numAgents", false, eNumAgents);
	getDynamicRaster(eNumAgents).setInitValues(0, std::numeric_limits<int>::max(), 0);
	
	registerDynamicRaster("walls", false, eWalls);
	getDynamicRaster(eWalls).setInitValues(0, 8, 0);

	registerDynamicRaster("compression", false, eCompression);
	getDynamicRaster(eCompression).setInitValues(0, std::numeric_limits<int>::max(), 0);

	registerDynamicRaster("deaths", false, eDeaths);
	getDynamicRaster(eDeaths).setInitValues(0, std::numeric_limits<int>::max(), 0);

	registerDynamicRaster("panic", false, ePanic);
	getDynamicRaster(ePanic).setInitValues(0, 1, 0);
	
	if(_config._initAgentsDistributionType.compare("raster")==0)
	{
		registerStaticRaster("initAgentsDistribution", true);
		Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("initAgentsDistribution"), _config._initAgentsDistributionFile, getBoundaries());
	}


	// compute exit cells
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

	// compute number of adjacent walls
	for(index._x=0; index._x<_overlapBoundaries._size._x; index._x++)
	{
		for(index._y=0; index._y<_overlapBoundaries._size._y; index._y++)
		{
			if(getValue(eObstacles, index)==1)
			{
				continue;
			}
			Engine::Point2D<int> neighbor(0,0);
			int adjacentWalls = 0;	
			for(neighbor._x=index._x-1; neighbor._x<=index._x+1; neighbor._x++)
			{
				for(neighbor._y=index._y-1; neighbor._y<=index._y+1; neighbor._y++)
				{
					if(neighbor._x==index._x && neighbor._y==index._y)
					{
						continue;
					}
					if(!checkPosition(neighbor))
					{
						continue;
					}
					if(getValue(eObstacles, neighbor)==1)
					{
						adjacentWalls++;
					}
				}
			}
			setValue(eWalls, index, adjacentWalls);
		}
	}
	
	for(ScenarioConfig::SupportRastersMap::iterator it=_config._supportMaps.begin(); it!=_config._supportMaps.end(); it++)
	{
		std::string name = it->first;
		std::string fileName = it->second;
		registerStaticRaster(name, true);
		Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster(name), fileName, getBoundaries());
	}


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

void Scenario::checkPanicEvents()
{
	ScenarioConfig::PanicEventsList::iterator it=_config._panicEvents.begin();
	while(it!=_config._panicEvents.end())
	{
		PanicEvent & event = *it;
		if(event._step!=getCurrentStep())
		{
			it++;
			continue;
		}
		// new event
		// activate panic in those zones
		Engine::Point2D<int> index;
		for(index._x=event._position._x-event._radius; index._x<=event._position._x+event._radius; index._x++)
		{
			for(index._y=event._position._y-event._radius; index._y<=event._position._y+event._radius; index._y++)
			{
				if(getValue(eObstacles, index)==1)
				{
					continue;
				}
				if(index.distance(event._position)<event._radius)
				{
					setValue(ePanic, index, 1);
				}
				
				if(index.distance(event._position)<event._obstacleRadius)
				{
					setMaxValue(eObstacles, index, 1);
					setValue(eObstacles, index, 1);
					AgentsVector agents = getAgent(index);
					for(int i=0; i<agents.size(); i++)
					{
						removeAgent(agents.at(i));
						setValue(eDeaths, index, getValue(eDeaths, index)+1);		
						setValue(eNumAgents, index, getValue(eNumAgents, index)-1);		
						setValue(eCompression, index, 0);		
					}
				}
			}
		}

		it = _config._panicEvents.erase(it);
	}

}

void Scenario::stepEnvironment()
{
	checkPanicEvents();
	/*
	// update body compression
	Engine::Point2D<int> index;
	for(index._x=0; index._x<_overlapBoundaries._size._x; index._x++)
	{
		for(index._y=0; index._y<_overlapBoundaries._size._y; index._y++)
		{
			if(getValue(eObstacles, index)==1)
			{
				continue;
			}
	
			// 4 deaths = not passable
			if(getValue(eDeaths, index)>_config._bodiesToObstacle)
			{
				setMaxValue(eObstacles, index, 1);
				setValue(eObstacles, index, 1);
				// all agents are moved to neighbors
				Engine::AgentsVector agents = getAgent(index);
				for(int i=0; i<agents.size(); i++)
				{

				}
			}
		}
	}
	*/
}

} // namespace Panic

