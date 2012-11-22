
#ifndef __Neighborhood_hxx__
#define __Neighborhood_hxx__

#include "World.hxx"
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
	
	void createRasters();
	void createAgents();

	// MPI
	MPI_Datatype * createTypeNeighbor();
	void registerTypes();
	void * createPackage( const std::string & type );
	Engine::Agent * createAgentFromPackage( const std::string & type, void * package );
	
	void stepEnvironment();
public:
	Neighborhood( Engine::Simulation & simulation, const NeighborConfig & config );
	virtual ~Neighborhood();
}; // class Neighborhood

} // namespace Segregation 


#endif // __Neighborhood_hxx__

