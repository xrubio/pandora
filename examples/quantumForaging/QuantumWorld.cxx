#include "QuantumWorld.hxx"

#include "Forager.hxx"
#include "QuantumWorldConfig.hxx"
#include <GeneralState.hxx>
#include <Statistics.hxx>

namespace QuantumExperiment
{

QuantumWorld::QuantumWorld( QuantumWorldConfig * config, Engine::Scheduler * scheduler ) : World(config, scheduler, false)
{
}

QuantumWorld::~QuantumWorld()
{
}

void QuantumWorld::createAgents()
{
    const QuantumWorldConfig & qConfig = (const QuantumWorldConfig &)getConfig();
	for(int i=0; i<qConfig._numAgents; i++)
	{
		std::ostringstream oss;
		oss << "Forager_"<<i;
		Forager * forager = 0;
		if(qConfig._riskAversion!=-1)
		{
			forager = new Forager(oss.str(), qConfig._neededResources, qConfig._perfectInformation, qConfig._riskAversion);
		}
		else
		{
			float riskAversion = Engine::GeneralState::statistics().getUniformDistValue();
			forager = new Forager(oss.str(), qConfig._neededResources, qConfig._perfectInformation, riskAversion);
		}

		forager->configureMDP(qConfig._horizon, qConfig._width, qConfig._explorationBonus);	
		addAgent(forager);
		forager->setRandomPosition();
		forager->createKnowledge();
	}
}

void QuantumWorld::createRasters()
{	
    const QuantumWorldConfig & qConfig = (const QuantumWorldConfig &)getConfig();
	registerDynamicRaster("resources", true, eResources);
	getDynamicRaster(eResources).setInitValues(0, std::numeric_limits<int>::max(), 0);

    for( auto index : getBoundaries())
	{
		// value ranging from 0 to 100
		int value = Engine::GeneralState::statistics().getUniformDistValue(0,100);
        setMaxValue(eResources, index, value);
	}	
    
    for(int i=0; i<qConfig._numAgents; i++)
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

