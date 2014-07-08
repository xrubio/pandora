
#ifndef __RemoveBulletWorld_hxx
#define __RemoveBulletWorld_hxx

#include "World.hxx"
#include "SimulationRecord.hxx"

#include <mpi.h>

namespace BattleSim
{

class RemoveBulletWorld : public Engine::World
{
	Engine::SimulationRecord _record;
	// percentage of bullets removed from battlefield
	int _percentage;

	void createRasters();
	void createAgents();
	
	void registerTypes();

	void * createPackage( const std::string & type );
	Engine::Agent * createAgentFromPackage( const std::string & type, void * package );
public:
	RemoveBulletWorld( const std::string & path, const std::string & battlefieldFile, int percentage );
	virtual ~RemoveBulletWorld();
};

} // namespace BattleSim 


#endif // __RemoveBulletWorld_hxx

