
#include "TestWorld.hxx"
#include "TestAgent.hxx"
#include <iostream>
#include <Simulation.hxx>

namespace Test 
{

TestWorld::TestWorld( Engine::Simulation & simulation, bool worldOfLastTest ) : World(simulation, 1, false, "data/results.h5")
{
	if(!worldOfLastTest)
	{
		setFinalize(false);
	}
}

TestWorld::~TestWorld()
{
}

void TestWorld::createRasters()
{
}

void TestWorld::createAgents()
{
}

} // namespace Test 

