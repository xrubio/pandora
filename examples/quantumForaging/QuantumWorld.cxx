#include <QuantumWorld.hxx>
#include "Forager.hxx"

namespace QuantumExperiment
{

QuantumWorld::QuantumWorld(const QuantumWorldConfig & config, Engine::Simulation & simulation, Engine::Scheduler * scheduler ) : World(simulation, scheduler, false), _config(config)
{
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

		forager->configureMDP(_config._horizon, _config._width, _config._explorationBonus);	
		addAgent(forager);
		forager->setRandomPosition();
		forager->createKnowledge();
	}
}

void QuantumWorld::createRasters()
{	
	registerDynamicRaster("resources", true, eResources);
	getDynamicRaster(eResources).setInitValues(0, std::numeric_limits<int>::max(), 0);

    for( auto index : getBoundaries())
	{
		// value ranging from 0 to 100
		int value = Engine::GeneralState::statistics().getUniformDistValue(0,100);
		getDynamicRaster(eResources).setMaxValue(index, value);
	}	
    
    for(int i=0; i<_config._numAgents; i++)
	{
		std::ostringstream oss;
		oss << "Forager_"<<i;
    
        std::ostringstream ossKnowledge;
        ossKnowledge<< oss.str() << "_knowledge";
	    std::ostringstream ossResources;
    	ossResources << oss.str() << "_resources";

    	registerDynamicRaster(ossKnowledge.str(), false);
    	registerDynamicRaster(ossResources.str(), false);
	    getDynamicRaster(ossKnowledge.str()).setInitValues(0, 10, 0);
	    getDynamicRaster(ossResources.str()).setInitValues(0, std::numeric_limits<int>::max(), 0);
    }

	updateRasterToMaxValues(eResources);	
}

void QuantumWorld::stepEnvironment()
{
}

}

