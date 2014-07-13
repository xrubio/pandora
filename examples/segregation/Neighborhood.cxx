
#include "Neighborhood.hxx"

#include <DynamicRaster.hxx>
#include <Exception.hxx>
#include "Neighbor.hxx"
#include "NeighborConfig.hxx"

#include <limits>

namespace Segregation 
{

Neighborhood::Neighborhood( const NeighborConfig & config, Engine::Simulation & simulation, Engine::Scheduler * scheduler ) : World(simulation, scheduler, false), _config(config)
{
}

Neighborhood::~Neighborhood()
{
}    

void Neighborhood::createAgents()
{
	int numAgents = _config._size._width * _config._size._height * _config._density;
	int numDimes = numAgents * _config._dimePercentage;
	std::cout << "creating : " << numAgents << " neighbors, dimes: " << numDimes << " and pennies: " << numAgents - numDimes  << std::endl;

	int indexDimes = 0;
	int indexPennies = 0;
	for(int i=0; i<numAgents; i++)
	{
		if((i%getNumTasks())==getId())
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

} // namespace Segregation 

