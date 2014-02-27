
#ifndef __TestWorld_hxx__
#define __TestWorld_hxx__

#include <World.hxx>

namespace Engine
{
	class Simulation;
}

namespace Test 
{

class TestWorld : public Engine::World
{
	void createRasters();
	void createAgents();

public:
	// worldOfLastTest must be true for the last world in test in order to clean up MPI state
	TestWorld( Engine::Simulation & simulation, bool worldOfLastTest = false );
	virtual ~TestWorld();
};

} // namespace Test 

#endif // __TestWorld_hxx

