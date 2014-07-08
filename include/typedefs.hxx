
#ifndef __PandoraTypedefs_hxx__
#define __PandoraTypedefs_hxx__

#include <list>
#include <vector>
#include <memory>

namespace Engine
{

class Agent;

typedef std::list< std::shared_ptr<Agent> > AgentsList;
typedef std::vector< std::shared_ptr<Agent> > AgentsVector;

} // namespace Engine

#endif // __PandoraTypedefs_hxx__

