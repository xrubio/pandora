
#ifndef __Site_hxx__
#define __Site_hxx__

#include <Agent.hxx>
#include <vector>

namespace Examples
{

class Site : public Engine::Agent
{
    std::vector<int> _features; // MpiVectorAttribute
    float _traits; // MpiBasicAttribute

    void transmitFeature( Engine::AgentPtr agent );
    std::string getCultureString() const;

public:
	Site( const std::string & id, float numFeatures, float numTraits );
	virtual ~Site();

    float getSimilarity( Engine::AgentPtr agent );
    float getSimilarityNeighbors();
    int getFeature( size_t index ) const;
	
	void updateState();
	void registerAttributes();
	void serialize();

	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
	Site( void * );
	void * fillPackage();
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

};

} // namespace Examples

#endif // __Site_hxx__

