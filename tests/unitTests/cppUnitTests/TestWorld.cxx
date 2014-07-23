
#include "TestWorld.hxx"
#include "TestAgent.hxx"
#include <iostream>
#include <Config.hxx>

namespace Test 
{

TestWorld::TestWorld( Engine::Config * config, Engine::Scheduler * scheduler) : World(config, scheduler, false)
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

