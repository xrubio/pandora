
#include <EarthConfig.hxx>

namespace Examples
{

EarthConfig::EarthConfig() : _scale(1), _numCases(0), _firstCase(0,0), _size(0,0), _demName("no loaded"), _populationName("no loaded")
{
}

EarthConfig::~EarthConfig()
{
}

void EarthConfig::extractParticularAttribs(TiXmlElement * root)
{
	TiXmlElement * element = root->FirstChildElement("size");
	retrieveAttributeMandatory( element, "width", _size._width);
	retrieveAttributeMandatory( element, "height", _size._height);

	element = root->FirstChildElement("inputData");
	retrieveAttributeMandatory( element, "dem", _demName);
	retrieveAttributeMandatory( element, "population", _populationName);
	retrieveAttributeMandatory( element, "scale", _scale);
	
	element = root->FirstChildElement("firstCase");
	retrieveAttributeMandatory(element, "num", _numCases);
	retrieveAttributeMandatory(element, "x", _firstCase._x);
	retrieveAttributeMandatory(element, "y", _firstCase._y);
}
	
const Engine::Size<int> & EarthConfig::getSize() const
{
	return _size;
}

} // namespace Examples

