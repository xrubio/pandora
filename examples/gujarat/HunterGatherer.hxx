
#ifndef __HunterGatherer_hxx__
#define __HunterGatherer_hxx__

#include <GujaratAgent.hxx>

namespace Gujarat
{
class Sector;
class ForageAction;

class HunterGatherer : public GujaratAgent
{	
	int _homeRange; // MpiBasicAttribute
	int _numSectors; // MpiBasicAttribute

	void updateKnowledge();

	// this method checks whether P1 and P2 are in the same side of the line that goes from B to C
	bool sameSide( Engine::Point2D<int> P1, Engine::Point2D<int> P2, Engine::Point2D<int> A, Engine::Point2D<int> B );
	bool insideTriangle( Engine::Point2D<int> p,  Engine::Point2D<int> b, Engine::Point2D<int> c ); 
	
	void selectActions();
	
	GujaratAgent * createNewAgent();

	std::vector<Sector *> _sectors;

public:
	HunterGatherer( const std::string & id );
	virtual ~HunterGatherer();

	void setHomeRange( int v ) { _homeRange = v; }
	int  getHomeRange() const { return _homeRange; }
	void setNumSectors( int v ) { _numSectors = v; }
	int  getNumSectors() const { return _numSectors; }
	const std::vector<Sector *> & getSectors() const { return _sectors; }

	void updateResources( int v ) { _collectedResources += v; }
	void updateKnowledge( const Engine::Point2D<int>& agentPos, const Engine::DynamicRaster& dataRaster, std::vector< Sector* >& sectors )const;
	
	void serialize();
	void registerAttributes();

	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
	HunterGatherer( void * );
	void * fillPackage();
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

};

} // namespace Gujarat

#endif // __HunterGatherer_hxx__

