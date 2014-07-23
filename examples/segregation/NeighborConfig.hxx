
#ifndef __NeighborConfig_hxx__
#define __NeighborConfig_hxx__

#include <Config.hxx>

namespace Segregation 
{
    class NeighborHood;

class NeighborConfig : public Engine::Config
{ 
	float _friendlyPercentage;
	float _density;
	float _dimePercentage;
	float _neighborDistance;
	float _maxMovingDistance;
public:
	NeighborConfig( const std::string & xmlFile );  
	virtual ~NeighborConfig();
    
	void loadParams(); 
	friend class Neighborhood;
};

} // namespace Segregation 

#endif // __NeighborConfig_hxx__

