
#include <RandomWorld.hxx>

#include <RandomWorldConfig.hxx>
#include <RandomAgent.hxx>
#include <Simulation.hxx>
#include <DynamicRaster.hxx>
#include <Point2D.hxx>
#include <GeneralState.hxx>

namespace Examples 
{

RandomWorld::RandomWorld(const RandomWorldConfig & config, Engine::Simulation & simulation, Engine::Scheduler * scheduler ) : World(simulation, scheduler, false), _config(config)
{
}

RandomWorld::~RandomWorld()
{
}

void RandomWorld::createRasters()
{
	registerDynamicRaster("resources", true);
	getDynamicRaster("resources").setInitValues(0, 5, 0);

	for(auto index:getBoundaries())
	{
		int value = Engine::GeneralState::statistics().getUniformDistValue(0,5);
		getDynamicRaster("resources").setMaxValue(index, value);
	}
	updateRasterToMaxValues("resources");
}

void RandomWorld::createAgents()
{
	for(int i=0; i<_config._numAgents; i++)
	{
		if((i%getNumTasks())==getId())
		{
			std::ostringstream oss;
			oss << "RandomAgent_" << i;
			RandomAgent * agent = new RandomAgent(oss.str());
			addAgent(agent);
			agent->setRandomPosition();
		}
	}
}

} // namespace Examples

