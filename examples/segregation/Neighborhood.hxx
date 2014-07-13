
#ifndef __Neighborhood_hxx__
#define __Neighborhood_hxx__

#include <World.hxx>
#include <string>

namespace Engine
{
	class Simulation;
}

namespace Segregation 
{

class NeighborConfig;

class Neighborhood : public Engine::World
{
	const NeighborConfig & _config;

public:
	Neighborhood( const NeighborConfig & config, Engine::Simulation & simulation, Engine::Scheduler * scheduler = 0);
	virtual ~Neighborhood();
	
	void createAgents();

}; // class Neighborhood

} // namespace Segregation 


#endif // __Neighborhood_hxx__

