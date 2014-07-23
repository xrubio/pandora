
#include "NeighborConfig.hxx"
#include <sstream>
#include <Exception.hxx>

namespace Segregation 
{

NeighborConfig::NeighborConfig( const std::string & xmlFile ) : Config(xmlFile), _friendlyPercentage(0.0f), _density(0.0f), _dimePercentage(0.0f), _neighborDistance(1.0f), _maxMovingDistance(1.0f)
{
}
  
NeighborConfig::~NeighborConfig()
{
}

void NeighborConfig::loadParams()
{
    _friendlyPercentage = getParamFloat("populationParams", "friendlyPercentage");
    _density = getParamFloat("populationParams", "density");
    _dimePercentage = getParamFloat("populationParams", "dimePercentage");
    _neighborDistance = getParamFloat("populationParams", "neighborDistance");
    _maxMovingDistance = getParamFloat("populationParams", "maxMovingDistance");
}

} // namespace Segregation

