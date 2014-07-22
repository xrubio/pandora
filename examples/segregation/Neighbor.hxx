
#ifndef __Neighbor_hxx__
#define __Neighbor_hxx__

#include <Agent.hxx>

namespace Segregation 
{

class Neighbor : public Engine::Agent
{
	float _friendlyPercentage; // MpiBasicAttribute	
	bool _needsToMove; // MpiBasicAttribute	
	float _neighborDistance; // MpiBasicAttribute	
	float _maxMovingDistance; // MpiBasicAttribute	
    int _friends; // MpiBasicAttribute
    int _neighbors; // MpiBasicAttribute

	void moveHome();

public:
	Neighbor( const std::string & id );
	virtual ~Neighbor();

	void updateKnowledge();
	void updateState();

	void setFriendlyPercentage( const float & friendlyPercentage );
	void setNeighborDistance( const float & neighborDistance );
	void setMaxMovingDistance( const float & maxMovingDistance );

    void registerAttributes();
    void serialize();

	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
	Neighbor( void * );
	void * fillPackage();
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

};

} // namespace Segregation 

#endif // __Neighbor_hxx__

