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
	retrieveAttributeMandatory(element, "size", _size);
	
	// agents
	element = root->FirstChildElement("agents");
	retrieveAttributeMandatory(element, "num", _numAgents);
	retrieveAttributeMandatory(element, "neededResources", _neededResources);

	//mdp
	TiXmlElement * child = element->FirstChildElement("mdp");
	retrieveAttributeMandatory(child, "horizon", _horizon);
	retrieveAttributeMandatory(child, "width", _width);	
	retrieveAttributeMandatory(child, "explorationBonus", _explorationBonus);	
}

int QuantumWorldConfig::getSize() const
{
	return _size;
}

} // namespace QuantumExperiment
