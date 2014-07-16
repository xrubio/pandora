
#ifndef __CustomWorld_hxx
#define __CustomWorld_hxx

#include <World.hxx>

namespace Engine
{
	class Simulation;
}

namespace Test 
{

class CustomWorld : public Engine::World
{
	void createAgents();

public:
	CustomWorld( const Engine::Simulation & simulation, Engine::Scheduler * scheduler = 0);
	virtual ~CustomWorld();
};

} // namespace Test 

#endif // __CustomWorld_hxx

