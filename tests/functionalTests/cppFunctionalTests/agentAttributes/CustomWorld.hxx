
#ifndef __CustomWorld_hxx
#define __CustomWorld_hxx

#include <World.hxx>

namespace Engine
{
	class Config;
}

namespace Test 
{

class CustomWorld : public Engine::World
{
	void createAgents();

public:
	CustomWorld( Engine::Config * config, Engine::Scheduler * scheduler = 0);
	virtual ~CustomWorld();
};

} // namespace Test 

#endif // __CustomWorld_hxx

