
#include <TestWorld.hxx>

#include <Simulation.hxx>
#include <Raster.hxx>
#include <Point2D.hxx>

namespace Test 
{

TestWorld::TestWorld(Engine::Simulation & simulation, Engine::Scheduler * scheduler ) : World(simulation, scheduler, false)
{
}

TestWorld::~TestWorld()
{
}

void TestWorld::createRasters()
{
	registerDynamicRaster("value", true);
	getDynamicRaster("value").setInitValues(0, _simulation.getSize()._width*_simulation.getSize()._height, 0);

	for(auto index : getBoundaries())
	{
		getDynamicRaster("value").setMaxValue(index, index._x*index._y);
	}
}

void TestWorld::createAgents()
{
}

} // namespace Test

