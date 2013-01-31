
#ifndef __Battlefield_hxx
#define __Battlefield_hxx

#include "World.hxx"
#include <mpi.h>

namespace BattleSim
{

class BattlefieldConfig;

class Battlefield : public Engine::World
{
	const BattlefieldConfig & _config;

	int _numSoldiers;
	void createRasters();
	void createAgents();
	
	void registerTypes();
	MPI_Datatype * registerSoldier();
	MPI_Datatype * registerBullet();

	void * createPackage( const std::string & type );
	Engine::Agent * createAgentFromPackage( const std::string & type, void * package );
	void stepEnvironment();
public:
	Battlefield( Engine::Simulation & simulation, const BattlefieldConfig & config );
	virtual ~Battlefield();
};

} // namespace BattleSim 


#endif // __Battlefield_hxx

