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
	retrieveAttributeMandatory(element, "size", _size);

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

	element2 = element->FirstChildElement("initPanic");
	retrieveAttributeMandatory(element2, "x", _initPanic._x);
	retrieveAttributeMandatory(element2, "y", _initPanic._y);
	retrieveAttributeMandatory(element2, "initRadius", _initPanicRadius);
	retrieveAttributeMandatory(element2, "contagion", _contagion);

	element2 = element->FirstChildElement("initAgentsDistribution");
	retrieveAttributeMandatory(element2, "type", _initAgentsDistributionType);
	if(_initAgentsDistributionType.compare("raster")==0)
	{
		retrieveAttributeMandatory(element2, "file", _initAgentsDistributionFile);
	}

	// agents
	element = root->FirstChildElement("agents");
	retrieveAttributeMandatory(element, "num", _numAgents);
	
	// compression
	element = root->FirstChildElement("compression");
	retrieveAttributeMandatory(element, "agentWeight", _agentCompressionWeight);
	retrieveAttributeMandatory(element, "wallWeight", _wallCompressionWeight);
	retrieveAttributeMandatory(element, "threshold", _compressionThreshold);
}

int ScenarioConfig::getSize() const
{
	return _size;
}

} // namespace Panic
