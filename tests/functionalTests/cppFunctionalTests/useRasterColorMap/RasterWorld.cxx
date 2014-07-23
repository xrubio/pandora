
#include <RasterWorld.hxx>

#include <RasterLoader.hxx>
#include <GeneralState.hxx>
#include <Config.hxx>

namespace Test 
{

RasterWorld::RasterWorld( Engine::Config * config, Engine::Scheduler * scheduler, const std::string & rasterFile ) : World(config, scheduler), _rasterFile(rasterFile)
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

