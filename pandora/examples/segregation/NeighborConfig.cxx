#include "NeighborConfig.hxx"
#include <sstream>
#include "Exceptions.hxx"

namespace Segregation 
{

NeighborConfig::NeighborConfig() : _size(0), _friendlyPercentage(0.0f), _populationDensity(0.0f), _dimePercentage(0.0f), _neighborDistance(1.0f), _maxMovingDistance(1.0f)
{
}
  
NeighborConfig::NeighborConfig(const std::string & filename) : _size(0), _friendlyPercentage(0.0f), _populationDensity(0.0f), _dimePercentage(0.0f), _neighborDistance(1.0f), _maxMovingDistance(1.0f)
{     
}

NeighborConfig::~NeighborConfig()
{
}

void NeighborConfig::extractParticularAttribs(TiXmlElement * root)
{
	TiXmlElement * element = root->FirstChildElement("size");
	_size = atoi(element->Attribute("value"));
	
	element = root->FirstChildElement("friendlyPercentage");
	_friendlyPercentage = atof(element->Attribute("value"));

	element = root->FirstChildElement("populationDensity");
	_populationDensity = atof(element->Attribute("value"));

	element = root->FirstChildElement("dimePercentage");
	_dimePercentage = atof(element->Attribute("value"));

	element = root->FirstChildElement("neighborDistance");
	_neighborDistance = atof(element->Attribute("value"));
	
	element = root->FirstChildElement("maxMovingDistance");
	_maxMovingDistance = atof(element->Attribute("value"));
}
	
const int & NeighborConfig::getSize()
{
	return _size;
}

} // namespace Segregation

