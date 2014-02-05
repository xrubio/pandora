
#include <NeolithicConfig.hxx>

namespace Examples
{

NeolithicConfig::NeolithicConfig() : _size(0,0), _mountains(false), _heightThreshold(0), _seaTravel(false), _seaTravelDistance(0), _saturationDensity(0.0f), _reproductiveRate(0.0f), _persistence(0.0f), _demFile(""), _initPopulationFile("")
{
}
  
NeolithicConfig::~NeolithicConfig()
{
}

void NeolithicConfig::extractParticularAttribs(TiXmlElement * root)
{
	TiXmlElement * element = root->FirstChildElement("location");
	float resolution = 0.0f;
	retrieveAttributeMandatory(element, "resolution", resolution);
	retrieveAttributeMandatory(element, "width", _size._x);
	retrieveAttributeMandatory(element, "height", _size._y);
	retrieveAttributeMandatory(element, "demFile", _demFile);
	retrieveAttributeMandatory(element, "initPopulationFile", _initPopulationFile);

	element = root->FirstChildElement("mountains");
	retrieveAttributeMandatory(element, "active", _mountains);
	retrieveAttributeMandatory(element, "threshold", _heightThreshold);

	element = root->FirstChildElement("seaTravel");
	retrieveAttributeMandatory(element, "active", _seaTravel);
	retrieveAttributeMandatory(element, "distance", _seaTravelDistance);
	_seaTravelDistance /= resolution;

	element = root->FirstChildElement("dispersion");
	float saturationDensityConstant = 0.0f;
	retrieveAttributeMandatory(element, "saturationDensityConstant", saturationDensityConstant);
	retrieveAttributeMandatory(element, "reproductiveRate", _reproductiveRate);
	retrieveAttributeMandatory(element, "persistence", _persistence);
	
	_saturationDensity = resolution * resolution * saturationDensityConstant;
}
	
const Engine::Point2D<int> & NeolithicConfig::getSize() const
{
	return _size;
}
  
} // namespace Examples

