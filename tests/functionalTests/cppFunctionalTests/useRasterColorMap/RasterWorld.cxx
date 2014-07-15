
#include <RasterWorld.hxx>

#include <RasterLoader.hxx>
#include <GeneralState.hxx>
#include <Simulation.hxx>

namespace Test 
{

RasterWorld::RasterWorld( const Engine::Simulation & simulation, Engine::Scheduler * scheduler, const std::string & rasterFile ) : World(simulation, scheduler), _rasterFile(rasterFile)
{
}

RasterWorld::~RasterWorld()
{
}

void RasterWorld::createRasters()
{
    registerStaticRaster("raster", true);
	Engine::GeneralState::rasterLoader().fillGDALRaster(getStaticRaster("raster"), _rasterFile);
}

void RasterWorld::createAgents()
{
}

} // namespace Test

