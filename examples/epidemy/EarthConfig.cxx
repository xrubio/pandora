
#include <EarthConfig.hxx>

namespace Examples
{

EarthConfig::EarthConfig() : _scale(1), _numCases(0), _firstCase(0,0)
{
	_gisData._size._x = 0;
	_gisData._size._y = 0;
	_gisData._resolution = 0;
	_gisData._demName = "no loaded";
	_gisData._populationName = "no loaded";
}

EarthConfig::~EarthConfig()
{
}

void EarthConfig::extractParticularAttribs(TiXmlElement * root)
{
	TiXmlElement * element = root->FirstChildElement("gis");
	retrieveAttributeMandatory( element, "width", _gisData._size._x);
	retrieveAttributeMandatory( element, "height", _gisData._size._y);
	retrieveAttributeMandatory( element, "resolution", _gisData._resolution);

	TiXmlElement * element2 = element->FirstChildElement("dem");
	retrieveAttributeMandatory( element2, "name", _gisData._demName);

	element2 = element->FirstChildElement("population");
	retrieveAttributeMandatory( element2, "name", _gisData._populationName);

	element = root->FirstChildElement("scale");
	retrieveAttributeMandatory(element, "value", _scale);	
	
	element = root->FirstChildElement("firstCase");
	retrieveAttributeMandatory(element, "num", _numCases);
	retrieveAttributeMandatory(element, "x", _firstCase._x);
	retrieveAttributeMandatory(element, "y", _firstCase._y);
}
	
const Engine::Point2D<int> & EarthConfig::getSize() const
{
	return _gisData._size;
}

} // namespace Examples

