
#ifndef __Neighbor_hxx__
#define __Neighbor_hxx__

#include "Agent.hxx"

#include <vector>
#include <list>

namespace Segregation 
{

class Neighbor : public Engine::Agent
{
	float _friendlyPercentage;
	bool _needsToMove;
	float _neighborDistance;
	float _maxMovingDistance;

	void updateKnowledge();
	void moveHome();

public:
	Neighbor( const std::string & id );
	virtual ~Neighbor();

	void * createPackage();

	void step();
	void serialize();

	void setFriendlyPercentage( const float & friendlyPercentage );
	void setNeighborDistance( const float & neighborDistance );
	void setMaxMovingDistance( const float & maxMovingDistance );
};

} // namespace Segregation 

#endif // __Neighbor_hxx__

