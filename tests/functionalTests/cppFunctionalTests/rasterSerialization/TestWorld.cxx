
#include <TestWorld.hxx>

#include <Config.hxx>
#include <DynamicRaster.hxx>
#include <Point2D.hxx>

namespace Test 
{

TestWorld::TestWorld(Engine::Config * config, Engine::Scheduler * scheduler ) : World(config, scheduler, false)
{
}

TestWorld::~TestWorld()
{
}

void TestWorld::createRasters()
{
	registerDynamicRaster("value", true);
	getDynamicRaster("value").setInitValues(0, _config->getSize()._width*_config->getSize()._height, 0);

	for(auto index : getBoundaries())
	{
		getDynamicRaster("value").setMaxValue(index, index._x*index._y);
	}
}

void TestWorld::createAgents()
{
}

} // namespace Test

