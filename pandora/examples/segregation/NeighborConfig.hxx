
#ifndef __NeighborConfig_hxx__
#define __NeighborConfig_hxx__

#include "Config.hxx"
#include <tinyxml.h>
#include <string>
#include <sstream>

namespace Segregation 
{
    class NeighborHood;

class NeighborConfig : public Config
{ 
	int _size;
	float _friendlyPercentage;
	float _populationDensity;
	float _dimePercentage;
	float _neighborDistance;
	float _maxMovingDistance;
public:
	NeighborConfig();  
	NeighborConfig(const std::string & filename);  
	virtual ~NeighborConfig();
    
	void extractParticularAttribs(TiXmlElement *pRoot);
	friend std::ostream & operator<<( std::ostream & stream, const NeighborConfig & config )
	{
		return stream << "Config(" << "path:" << config._path << "," << "size:" << config._size << "," << "steps:" << config._numSteps << " friendly percentage: " << config._friendlyPercentage<< " population density: " << config._populationDensity << " dime percentage: " << config._dimePercentage << " and neighbor distance: " << config._neighborDistance << ")";
	}  
	friend class Neighborhood;
	const int & getSize();
};

} // namespace Segregation 

#endif // __NeighborConfig_hxx__

