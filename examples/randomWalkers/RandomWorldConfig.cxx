
#include <RandomWorldConfig.hxx>

namespace Examples
{

RandomWorldConfig::RandomWorldConfig() : _numAgents(0), _size(0, 0)
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
	retrieveAttributeMandatory( element, "width", _size._width);
	retrieveAttributeMandatory( element, "height", _size._height);
}
	
const Engine::Size<int> & RandomWorldConfig::getSize() const
{
	return _size;
}

} // namespace Examples

