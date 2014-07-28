
#ifndef __PandoraTypedefs_hxx__
#define __PandoraTypedefs_hxx__

#include <list>
#include <vector>
#include <memory>

namespace Engine
{

class Agent;

typedef std::shared_ptr<Agent> AgentPtr;
typedef std::list< AgentPtr > AgentsList;
typedef std::vector< AgentPtr > AgentsVector;

//! List of Mpi messages sent by the scheduler
enum MpiMessageType
{
	eNumAgents = 1,
	eAgent = 2,
	eNumGhostAgents = 3,
	eGhostAgent = 4,
	eRasterData = 5,
	eRasterMaxData = 6,
	eVectorAttribute = 7, 	
	eSizeVector = 8,
	eNumModifiedAgents = 9,
	eModifiedAgent = 10
};


} // namespace Engine

#endif // __PandoraTypedefs_hxx__

