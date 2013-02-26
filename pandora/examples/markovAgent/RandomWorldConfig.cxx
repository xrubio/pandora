
#include <RandomWorldConfig.hxx>

namespace Examples
{

RandomWorldConfig::RandomWorldConfig() : _numAgents(0), _size(0), _horizon(0), _width(0), _explorationBonus(0)
{
}

RandomWorldConfig::~RandomWorldConfig()
{
}

void RandomWorldConfig::extractParticularAttribs(TiXmlElement * root)
{
	TiXmlElement * element = root->FirstChildElement("numAgents");
	retrieveAttributeMandatory( element, "value", _numAgents);
	element = root->FirstChildElement("size");
	retrieveAttributeMandatory( element, "value", _size);

	element = root->FirstChildElement("mdp");
	retrieveAttributeMandatory( element, "horizon", _horizon);
	retrieveAttributeMandatory( element, "width", _width);
	retrieveAttributeMandatory( element, "explorationBonus", _explorationBonus);

}
	
int RandomWorldConfig::getSize() const
{
	return _size;
}

} // namespace Examples

