
#include "TestWorld.hxx"
#include "TestAgent.hxx"
#include <iostream>
#include <Simulation.hxx>

namespace Test 
{

TestWorld::TestWorld( Engine::Simulation & simulation, Engine::Scheduler * scheduler) : World(simulation, scheduler, false)
{
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

