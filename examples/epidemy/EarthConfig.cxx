
#include <EarthConfig.hxx>

namespace Examples
{

EarthConfig::EarthConfig( const std::string & xmlFile ) : Config(xmlFile), _scale(1), _numCases(0), _firstCase(0,0), _demName("no loaded"), _populationName("no loaded")
{
}

EarthConfig::~EarthConfig()
{
}

void EarthConfig::loadParams()
{
    _demName = getParamStr("inputData", "dem");
    _populationName = getParamStr("inputData", "population");
    _scale = getParamInt("inputData", "scale");

    _numCases = getParamInt("outbreak", "initInfected");
    _firstCase._x = getParamInt("outbreak", "x");
    _firstCase._y = getParamInt("outbreak", "y");
    _virulence = getParamFloat("outbreak", "virulence");

}

} // namespace Examples

