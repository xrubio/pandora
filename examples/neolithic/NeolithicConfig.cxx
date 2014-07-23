
#include <NeolithicConfig.hxx>

namespace Examples
{

NeolithicConfig::NeolithicConfig( const std::string & xmlFile ) : Config(xmlFile), _mountains(false), _heightThreshold(0), _seaTravel(false), _seaTravelDistance(0), _saturationDensity(0.0f), _reproductiveRate(0.0f), _persistence(0.0f), _demFile(""), _initPopulationFile("")
{
}
  
NeolithicConfig::~NeolithicConfig()
{
}

void NeolithicConfig::loadParams()
{

    _demFile = getParamStr("location", "demFile");
    _initPopulationFile = getParamStr("location", "initPopulationFile");

    _mountains = getParamBool("mountains", "active");
    _heightThreshold = getParamInt("mountains", "threshold");

    _seaTravel = getParamBool("seaTravel", "active");
    _seaTravelDistance = getParamInt("seaTravel", "distance");
    float resolution = getParamFloat("location", "resolution");
	_seaTravelDistance /= resolution;

    float saturationDensityConstant = getParamFloat("dispersion", "saturationDensityConstant");
    _reproductiveRate = getParamFloat("dispersion", "reproductiveRate");
    _persistence = getParamFloat("dispersion", "persistence");
	_saturationDensity = resolution * resolution * saturationDensityConstant;
}
	
} // namespace Examples

