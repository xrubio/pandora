
#ifndef __NeighborConfig_hxx__
#define __NeighborConfig_hxx__

#include <Config.hxx>
#include <Size.hxx>
#include <sstream>

namespace Segregation 
{
    class NeighborHood;

class NeighborConfig : public Engine::Config
{ 
    Engine::Size<int> _size;
	float _friendlyPercentage;
	float _density;
	float _dimePercentage;
	float _neighborDistance;
	float _maxMovingDistance;
public:
	NeighborConfig();  
	virtual ~NeighborConfig();
    
	void extractParticularAttribs(TiXmlElement *pRoot);
	friend class Neighborhood;
	const Engine::Size<int> & getSize();
};

} // namespace Segregation 

#endif // __NeighborConfig_hxx__

