
#ifndef __Site_hxx
#define __Site_hxx

#include "World.hxx"
#include "SimulationRecord.hxx"

#include <mpi.h>

namespace BattleSim
{

class Site : public Engine::World
{
	Engine::SimulationRecord _record;
	int _distance;
	bool _horizontalMovement;

	void createRasters();
	void createAgents();
	
	void registerTypes();
	MPI_Datatype * registerArchaeologist();

	void * createPackage( const std::string & type );
	Engine::Agent * createAgentFromPackage( const std::string & type, void * package );
public:
	Site( const std::string & path, const std::string & battlefieldFile, int distance, bool horizontalMovement );
	virtual ~Site();
};

} // namespace BattleSim 


#endif // __Site_hxx

