
#include <HelloWorld.hxx>
#include <HelloAgent.hxx>
#include <Simulation.hxx>
#include <DynamicRaster.hxx>
#include <Point2D.hxx>
#include <GeneralState.hxx>

HelloWorld::HelloWorld(Engine::Simulation & simulation) : World(simulation)
{
}

HelloWorld::~HelloWorld()
{
}

void HelloWorld::createRasters()
{
    std::cout << this << " creating rasters" << std::endl;
	registerDynamicRaster("values", true);
	getDynamicRaster("values").setInitValues(0, 0, 0);
}

void HelloWorld::createAgents()
{
    std::cout << this << " creating agents" << std::endl;
    HelloAgent * newAgent = new HelloAgent("HelloAgent_0");
    addAgent(newAgent);
    newAgent->setRandomPosition();
}

void HelloWorld::stepEnvironment()
{
    std::cout << this << " step of environment" << std::endl;
}

