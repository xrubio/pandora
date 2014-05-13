
#include "Neighborhood.hxx"

#include <DynamicRaster.hxx>
#include <Exception.hxx>
#include "Neighbor.hxx"
#include "NeighborConfig.hxx"

#include <limits>

namespace Segregation 
{

Neighborhood::Neighborhood( Engine::Simulation & simulation, const NeighborConfig & config ) : World(simulation, std::max(config._neighborDistance, config._maxMovingDistance), false, config._resultsFile), _config(config)					
{
}

Neighborhood::~Neighborhood()
{
}    

void Neighborhood::createRasters()
{
	registerDynamicRaster("base", true);
	getDynamicRaster("base").setInitValues(0, std::numeric_limits<int>::max(), 0);
}

void Neighborhood::createAgents()
{
	int numAgents = _config._size * _config._size * _config._populationDensity;
	int numDimes = numAgents * _config._dimePercentage;
	std::cout << "creating : " << numAgents << " neighbors, dimes: " << numDimes << " and pennies: " << numAgents - numDimes  << std::endl;

	int indexDimes = 0;
	int indexPennies = 0;
	for(int i=0; i<numAgents; i++)
	{
		if((i%_simulation.getNumTasks())==_simulation.getId())
		{
			std::ostringstream oss;
			if(indexDimes<numDimes)
			{
 				oss << "dime_" << indexDimes;
				indexDimes++;

			}
			else
			{
				oss << "pennies_" << indexPennies;
				indexPennies++;
			}
			Neighbor * agent = new Neighbor(oss.str());
			addAgent(agent);
			agent->setFriendlyPercentage( _config._friendlyPercentage );
			agent->setNeighborDistance( _config._neighborDistance );
			agent->setMaxMovingDistance( _config._maxMovingDistance);
			agent->setPosition(getRandomPosition());
		}
	}
}
void Neighborhood::stepEnvironment()
{
}

} // namespace Segregation 

