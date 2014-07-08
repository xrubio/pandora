#include <QuantumWorld.hxx>
#include "Forager.hxx"

namespace QuantumExperiment
{

QuantumWorld::QuantumWorld(Engine::Simulation &simulation, QuantumWorldConfig &config) : World(simulation, 1, true, config._resultsFile)
{
	_config = config;
}

QuantumWorld::~QuantumWorld()
{
}

void QuantumWorld::createAgents()
{
	for(int i=0; i<_config._numAgents; i++)
	{
		std::ostringstream oss;
		oss << "Forager_"<<i;
		Forager * forager = 0;
		if(_config._riskAversion!=-1)
		{
			forager = new Forager(oss.str(), _config._neededResources, _config._perfectInformation, _config._riskAversion);
		}
		else
		{
			float riskAversion = (float)Engine::GeneralState::statistics().getUniformDistValue(0,100)/100.0f;
			forager = new Forager(oss.str(), _config._neededResources, _config._perfectInformation, riskAversion);
		}

		//int horizon = Engine::GeneralState::statistics().getUniformDistValue(1,_config._horizon);
		//int width = Engine::GeneralState::statistics().getUniformDistValue(2,_config._width);
		forager->configureMDP(_config._horizon, _config._width, _config._explorationBonus);	
		addAgent(forager);
		forager->setRandomPosition();
		//forager->setPosition(Engine::Point2D<int>(0,0));
		forager->createKnowledge();
	}
}

void QuantumWorld::createRasters()
{	
	registerDynamicRaster("resources", true, eResources);
	getDynamicRaster(eResources).setInitValues(0, std::numeric_limits<int>::max(), 0);

	Engine::Point2D<int> index(0,0);
	int maxValue = _overlapBoundaries._size._x;
	for(index._x=0; index._x<_overlapBoundaries._size._x; index._x++)
	{
		for(index._y=0; index._y<_overlapBoundaries._size._y; index._y++)
		{
			int value = Engine::GeneralState::statistics().getUniformDistValue(0,100);
			// value ranging from 0 to 100
			//int value = 100*index._x/maxValue;
			getDynamicRaster(eResources).setMaxValue(index, value);
		}
	}
	updateRasterToMaxValues(eResources);	
	/*
	registerDynamicRaster("gathered", true);
	getDynamicRasterStr("gathered").setInitValues(0, std::numeric_limits<int>::max(), 0);
	*/
}

void QuantumWorld::stepEnvironment()
{
}

}

