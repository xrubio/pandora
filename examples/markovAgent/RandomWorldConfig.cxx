
#include <RandomWorldConfig.hxx>

namespace Examples
{

RandomWorldConfig::RandomWorldConfig() : _numAgents(0), _size(0), _horizon(0), _width(0), _explorationBonus(0), _neededResources(0), _birthResources(0)
{
}

RandomWorldConfig::~RandomWorldConfig()
{
}

void RandomWorldConfig::extractParticularAttribs(TiXmlElement * root)
{
	TiXmlElement * element = root->FirstChildElement("size");
	retrieveAttributeMandatory( element, "value", _size);

	element = root->FirstChildElement("agents");
	retrieveAttributeMandatory( element, "num", _numAgents);
	retrieveAttributeMandatory( element, "neededResources", _neededResources);
	retrieveAttributeMandatory( element, "birthResources", _birthResources);

	TiXmlElement * mdp = element->FirstChildElement("mdp");
	retrieveAttributeMandatory( mdp, "horizon", _horizon);
	retrieveAttributeMandatory( mdp, "width", _width);
	retrieveAttributeMandatory( mdp, "explorationBonus", _explorationBonus);

}
	
int RandomWorldConfig::getSize() const
{
	return _size;
}

} // namespace Examples

