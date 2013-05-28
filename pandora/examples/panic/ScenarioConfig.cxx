#include <ScenarioConfig.hxx>

namespace Panic
{

ScenarioConfig::ScenarioConfig() : _obstacleFile("not loaded")
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
	retrieveAttributeMandatory(element, "fileName", _obstacleFile);

	TiXmlElement * element2 = element->FirstChildElement("initPanic");
	retrieveAttributeMandatory(element2, "x", _initPanic._x);
	retrieveAttributeMandatory(element2, "y", _initPanic._y);
	retrieveAttributeMandatory(element2, "initRadius", _initPanicRadius);
	retrieveAttributeMandatory(element2, "contagion", _contagion);

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
