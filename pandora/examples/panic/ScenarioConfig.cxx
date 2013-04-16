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
	
	// agents
	element = root->FirstChildElement("agents");
	retrieveAttributeMandatory(element, "num", _numAgents);
}

int ScenarioConfig::getSize() const
{
	return _size;
}

} // namespace Panic
