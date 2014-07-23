#include <ScenarioConfig.hxx>

namespace Panic
{

ScenarioConfig::ScenarioConfig( const std::string & xmlFile ) : Config(xmlFile), _obstacleFile("not loaded"), _initAgentsDistributionFile("not loaded")
{
}

ScenarioConfig::~ScenarioConfig()
{
}

void ScenarioConfig::loadParams()
{
	//environment
    _obstacleFile = getParamStr("environment/rasters/obstacles", "file");
    // TODO multiple support and/or panic events, issue #95
    _initAgentsDistributionType = getParamStr("environment/initAgentsDistribution", "type");
    if(_initAgentsDistributionType=="raster")
    {
        _initAgentsDistributionFile = getParamStr("environment/initAgentsDistribution","file"); 
    }

    // TODO multiple support and/or panic events, issue #95
    PanicEvent event;
    event._position._x = getParamInt("panicEvents/event", "x");
    event._position._y = getParamInt("panicEvents/event", "y");
    event._radius = getParamInt("panicEvents/event", "initRadius");
    event._obstacleRadius = getParamInt("panicEvents/event", "obstacleRadius");
    event._step = getParamInt("panicEvents/event", "step");
	_panicEvents.push_back(event);

    _numAgents = getParamInt("agents", "num");
    _agentCompressionWeight = getParamInt("agents/compression", "agentWeight");
    _wallCompressionWeight = getParamInt("agents/compression", "wallWeight");

	// compression
    _compressionThreshold = getParamInt("agents/physicalParameters", "threshold");
    _bodiesToObstacle = getParamInt("agents/physicalParameters", "obstacle");
    _contagion = getParamInt("agents/physicalParameters", "contagion");
    _knowledge = getParamInt("agents/physicalParameters", "knowledge");
}

} // namespace Panic

