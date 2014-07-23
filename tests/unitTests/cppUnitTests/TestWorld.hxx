
#ifndef __TestWorld_hxx__
#define __TestWorld_hxx__

#include <World.hxx>

namespace Engine
{
	class Config;
}

namespace Test 
{

class TestWorld : public Engine::World
{
	void createRasters();
	void createAgents();

public:
	// worldOfLastTest must be true for the last world in test in order to clean up MPI state
	TestWorld( Engine::Config * config, Engine::Scheduler * scheduler = 0 );
	virtual ~TestWorld();
};

} // namespace Test 

#endif // __TestWorld_hxx

