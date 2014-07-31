
#include "Neighborhood.hxx"

#include <DynamicRaster.hxx>
#include <Exception.hxx>
#include "Neighbor.hxx"
#include "NeighborConfig.hxx"

#include <limits>

namespace Segregation 
{

Neighborhood::Neighborhood( NeighborConfig * config, Engine::Scheduler * scheduler ) : World(config, scheduler, false)
{
}

Neighborhood::~Neighborhood()
{
}    

void Neighborhood::createAgents()
{
    const NeighborConfig & neighborConfig = (const NeighborConfig &)getConfig();
	int numAgents = neighborConfig._size._width * neighborConfig._size._height * neighborConfig._density;
	int numDimes = numAgents * neighborConfig._dimePercentage;

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
			agent->setFriendlyPercentage( neighborConfig._friendlyPercentage );
			agent->setNeighborDistance( neighborConfig._neighborDistance );
			agent->setMaxMovingDistance( neighborConfig._maxMovingDistance);
			agent->setPosition(getRandomPosition());
		}
	}
}

} // namespace Segregation 

