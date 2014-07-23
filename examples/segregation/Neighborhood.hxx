
#ifndef __Neighborhood_hxx__
#define __Neighborhood_hxx__

#include <World.hxx>
#include <string>

namespace Segregation 
{

class NeighborConfig;

class Neighborhood : public Engine::World
{
public:
	Neighborhood( NeighborConfig * config, Engine::Scheduler * scheduler = 0);
	virtual ~Neighborhood();
	
	void createAgents();

}; // class Neighborhood

} // namespace Segregation 


#endif // __Neighborhood_hxx__

