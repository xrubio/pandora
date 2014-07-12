#include "NeighborConfig.hxx"
#include <sstream>
#include "Exception.hxx"

namespace Segregation 
{

NeighborConfig::NeighborConfig() : _friendlyPercentage(0.0f), _density(0.0f), _dimePercentage(0.0f), _neighborDistance(1.0f), _maxMovingDistance(1.0f)
{
}
  
NeighborConfig::~NeighborConfig()
{
}

void NeighborConfig::extractParticularAttribs(TiXmlElement * root)
{
	TiXmlElement * element = root->FirstChildElement("size");
	retrieveAttributeMandatory( element, "width", _size._width);
	retrieveAttributeMandatory( element, "height", _size._height);

	element = root->FirstChildElement("populationParams");
	retrieveAttributeMandatory( element, "friendlyPercentage", _friendlyPercentage);
	retrieveAttributeMandatory( element, "density", _density);
	retrieveAttributeMandatory( element, "dimePercentage", _dimePercentage);
	retrieveAttributeMandatory( element, "neighborDistance", _neighborDistance);
	retrieveAttributeMandatory( element, "maxMovingDistance", _maxMovingDistance);
}
	
const Engine::Size<int> & NeighborConfig::getSize()
{
	return _size;
}

} // namespace Segregation

