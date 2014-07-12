#include <QuantumWorldConfig.hxx>

namespace QuantumExperiment
{

QuantumWorldConfig::QuantumWorldConfig()
{
}

QuantumWorldConfig::~QuantumWorldConfig()
{
}

void QuantumWorldConfig::extractParticularAttribs(TiXmlElement * root)
{
	//environment
	TiXmlElement * element = root->FirstChildElement("environment");
	retrieveAttributeMandatory(element, "width", _size._width);
	retrieveAttributeMandatory(element, "height", _size._height);
	
	// agents
	element = root->FirstChildElement("agents");
	retrieveAttributeMandatory(element, "num", _numAgents);
	retrieveAttributeMandatory(element, "neededResources", _neededResources);
	retrieveAttributeMandatory(element, "perfectInformation", _perfectInformation);
	retrieveAttributeMandatory(element, "riskAversion", _riskAversion);

	//mdp
	TiXmlElement * child = element->FirstChildElement("mdp");
	retrieveAttributeMandatory(child, "horizon", _horizon);
	retrieveAttributeMandatory(child, "width", _width);	
	retrieveAttributeMandatory(child, "explorationBonus", _explorationBonus);	
}

const Engine::Size<int> & QuantumWorldConfig::getSize() const
{
	return _size;
}

} // namespace QuantumExperiment
