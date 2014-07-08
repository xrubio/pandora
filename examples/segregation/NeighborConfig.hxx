
#ifndef __NeighborConfig_hxx__
#define __NeighborConfig_hxx__

#include <Config.hxx>
#include <tinyxml.h>
#include <string>
#include <sstream>

namespace Segregation 
{
    class NeighborHood;

class NeighborConfig : public Engine::Config
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
	friend class Neighborhood;
	const int & getSize();
};

} // namespace Segregation 

#endif // __NeighborConfig_hxx__

