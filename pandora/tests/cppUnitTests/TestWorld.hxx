
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
	TestWorld( Engine::Simulation & simulation );
	virtual ~TestWorld();
};

} // namespace Test 

#endif // __TestWorld_hxx

