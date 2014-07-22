
#ifndef __RandomWorld_hxx
#define __RandomWorld_hxx

#include <World.hxx>

namespace Examples 
{

class RandomWorldConfig;

class RandomWorld : public Engine::World
{
	void createRasters();
	void createAgents();
public:
	RandomWorld(Engine::Config * config, Engine::Scheduler * scheduler = 0);
	virtual ~RandomWorld();
};

} // namespace Examples 

#endif // __RandomWorld_hxx

