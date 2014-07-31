
#include <CultureWorld.hxx>

#include <CultureConfig.hxx>
#include <Site.hxx>
#include <DynamicRaster.hxx>
#include <Point2D.hxx>
#include <Rectangle.hxx>

namespace Examples 
{

CultureWorld::CultureWorld(Engine::Config * config, Engine::Scheduler * scheduler ) : World(config, scheduler, false)
{
}

CultureWorld::~CultureWorld()
{
}

void CultureWorld::createAgents()
{
    const CultureConfig & cultureConfig = (const CultureConfig&)getConfig();
    for(auto index : getBoundaries())
    {
		std::ostringstream oss;
		oss << "Site_" << index._y*getBoundaries()._size._width+index._x;
		Site * site = new Site( oss.str(), cultureConfig._features, cultureConfig._traitsPerFeature);
		addAgent(site);
		site->setPosition(index);
	}
}

} // namespace Examples

