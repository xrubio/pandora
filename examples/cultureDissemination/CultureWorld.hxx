
#ifndef __CultureWorld_hxx
#define __CultureWorld_hxx

#include <World.hxx>

namespace Examples 
{

class CultureConfig;

class CultureWorld : public Engine::World
{
	void createAgents();
public:
	CultureWorld(Engine::Config * config, Engine::Scheduler * scheduler = 0);
	virtual ~CultureWorld();
};

} // namespace Examples 

#endif // __CultureWorld_hxx

