
#include <CustomWorld.hxx>
#include <CustomAgent.hxx>
#include <Config.hxx>

namespace Test 
{

CustomWorld::CustomWorld( Engine::Config * config, Engine::Scheduler * scheduler) : World(config, scheduler)
{
}

CustomWorld::~CustomWorld()
{
}

void CustomWorld::createAgents()
{
    // 100 agents with random values
	for(int i=0; i<100; i++)
	{
		std::ostringstream oss;
		oss << "CustomAgent_" << i;
		CustomAgent * newAgent = new CustomAgent(oss.str());
        addAgent(newAgent);
        newAgent->setRandomPosition();
        std::stringstream oss2;
        oss2 << "string attribute: " << i%10;
		newAgent->setStringAttribute(oss2.str());
		newAgent->setIntAttribute(i%10);
		newAgent->setFloatAttribute(float(i%10)/10.0f);
	}
}

} // namespace Test

