
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
	Neighborhood( Engine::Simulation & simulation, const NeighborConfig & config );
	virtual ~Neighborhood();
	
	void createRasters();
	void createAgents();

	void stepEnvironment();

}; // class Neighborhood

} // namespace Segregation 


#endif // __Neighborhood_hxx__

