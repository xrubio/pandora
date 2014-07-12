#include <ScenarioConfig.hxx>

namespace Panic
{

ScenarioConfig::ScenarioConfig() : _obstacleFile("not loaded"), _initAgentsDistributionFile("not loaded")
{
}

ScenarioConfig::~ScenarioConfig()
{
}

void ScenarioConfig::extractParticularAttribs(TiXmlElement * root)
{
	//environment
	TiXmlElement * element = root->FirstChildElement("environment");
	retrieveAttributeMandatory(element, "width", _size._width);
	retrieveAttributeMandatory(element, "height", _size._height);

	TiXmlElement * elementRasters = element->FirstChildElement("rasters");

	TiXmlElement * element2 = elementRasters->FirstChildElement("obstacles");
	retrieveAttributeMandatory(element2, "file", _obstacleFile);

	element2 = elementRasters->FirstChildElement("support");
	while(element2)
	{
		std::string name;
		std::string file;
		retrieveAttributeMandatory(element2, "name", name);
		retrieveAttributeMandatory(element2, "file", file);
		_supportMaps.insert(make_pair(name, file));
		element2 = element2->NextSiblingElement("support");
	}

	element2 = element->FirstChildElement("initAgentsDistribution");
	retrieveAttributeMandatory(element2, "type", _initAgentsDistributionType);
	if(_initAgentsDistributionType.compare("raster")==0)
	{
		retrieveAttributeMandatory(element2, "file", _initAgentsDistributionFile);
	}

	// panic events
	element = root->FirstChildElement("panicEvents");	
	element2 = element->FirstChildElement("event");
	while(element2)
	{
		PanicEvent event;
		retrieveAttributeMandatory(element2, "x", event._position._x);
		retrieveAttributeMandatory(element2, "y", event._position._y);
		retrieveAttributeMandatory(element2, "initRadius", event._radius);
		retrieveAttributeMandatory(element2, "obstacleRadius", event._obstacleRadius);
		retrieveAttributeMandatory(element2, "step", event._step);
		_panicEvents.push_back(event);
		element2 = element2->NextSiblingElement("event");
	}
	// agents
	element = root->FirstChildElement("agents");
	retrieveAttributeMandatory(element, "num", _numAgents);
	
	element2 = element->FirstChildElement("compression");
	retrieveAttributeMandatory(element2, "agentWeight", _agentCompressionWeight);
	retrieveAttributeMandatory(element2, "wallWeight", _wallCompressionWeight);

	// compression
	element2 = element->FirstChildElement("physicalParameters");
	retrieveAttributeMandatory(element2, "threshold", _compressionThreshold);
	retrieveAttributeMandatory(element2, "obstacle", _bodiesToObstacle);
	retrieveAttributeMandatory(element2, "contagion", _contagion);
	retrieveAttributeMandatory(element2, "knowledge", _knowledge);
}

const Engine::Size<int> & ScenarioConfig::getSize() const
{
	return _size;
}

} // namespace Panic
