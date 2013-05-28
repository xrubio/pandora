
#ifndef __HunterGatherer_hxx__
#define __HunterGatherer_hxx__

#include <GujaratAgent.hxx>
#include <HGMind.hxx>

namespace Gujarat
{
class Sector;
//class ForageAction;
//class HGMind;

class HunterGatherer : public GujaratAgent
{	
protected:
	//int 					_surplusForReproductionThreshold;
	//int 					_surplusWanted;
	int 					_homeRange; // MpiBasicAttribute
	int 					_numSectors; // MpiBasicAttribute
	int						_lowResHomeRange; // MpiBasicAttribute
	HGMind * _myHGMind; 
	//void updateKnowledge();

	// this method checks whether P1 and P2 are in the same side of the line that goes from B to C
	bool sameSide( Engine::Point2D<int> P1, Engine::Point2D<int> P2, Engine::Point2D<int> A, Engine::Point2D<int> B );
	bool insideTriangle( Engine::Point2D<int> p,  Engine::Point2D<int> b, Engine::Point2D<int> c ); 
	
	void selectActions();
	
	GujaratAgent * createNewAgent();

	//std::vector<Sector *> _HRSectors;
	//std::vector<Sector *> _LRSectors;
	
	
public:
	HunterGatherer( const std::string & id );
	virtual ~HunterGatherer();

	void createMind();
	
	void setPosition(Engine::Point2D<int> p) { 	GujaratAgent::setPosition(p); _myHGMind->clearSectorKnowledge(); }
	
	void setHomeRange( int v ) { _homeRange = v; }
	int  getHomeRange() const { return _homeRange; }
	void setNumSectors( int v ) { _numSectors = v; }
	int  getNumSectors() const { return _numSectors; }
	void setLowResHomeRange(int v) { _lowResHomeRange = v; }
	int getLowResHomeRange() const { return _lowResHomeRange; }
	
	const std::vector<Sector *> & getHRSectors() const 
	{ return _myHGMind->getHRSectors(); }
	//{ return _HRSectors; }
	const std::vector<Sector *> & getLRSectors() const 
	{ return _myHGMind->getLRSectors(); }
	//{ return _LRSectors; }
	
	Sector * getHRSectors(int i) { return getHRSectors()[i]; }
	
	inline Engine::Raster & getLRResourcesRaster();
	
	//void setSurplusForReproductionThreshold( int v ) { _surplusForReproductionThreshold = v; }
	//void setSurplusWanted( int v ) { _surplusWanted = v; }
	bool cellValid( Engine::Point2D<int>& loc );
	bool cellRelevant( Engine::Point2D<int>& loc );
	
//	bool needsResources(int timeSteps);
//	bool needsResources();

	void updateResources( int v ) { _collectedResources += v; }
	
	void createHRSectors( 	const Engine::Point2D<int>& agentPos,
							std::vector< Sector* >& HRSectors) const;
	void createLRSectors( 	const Engine::Point2D<int>& agentPos, 
							std::vector< Sector* >& LRSectors ) const;
	void updateKnowledge();
	void updateKnowledge( 	const Engine::Point2D<int>& agentPos, const Engine::Raster& dataRaster, std::vector< Sector* >& HRSectors, std::vector< Sector* >& LRSectors  )const;
	//virtual void fillKnowledge() = 0;
	void updateKnowledgeFromFirstHandVisit(Sector * s){};	
	
	void executeActions();
	
	void clearSectorKnowledge();
	void clearSectorKnowledge(Engine::Point2D<int> LRHomeLocationOLD,Engine::Point2D<int> LRHomeLocationNEW){}
	
	void serialize();
	void registerAttributes();

	//*********** INFORMATION SHARING
	void doInformationSharing();
	void shareInformation( HunterGatherer * a);
	void putInformation(Sector *s, const Engine::Raster & r, const Engine::Raster & t);
	//********************************
	
	
	
	////////////////////////////////////////////////
	// This code has been automatically generated //
	/////// Please do not modify it ////////////////
	////////////////////////////////////////////////
#ifdef PANDORAMPI
	HunterGatherer( void * );
	void * fillPackage();
	void sendVectorAttributes(int);
	void receiveVectorAttributes(int);
#endif // PANDORAMPI
	////////////////////////////////////////////////
	//////// End of generated code /////////////////
	////////////////////////////////////////////////

	//friend class HGMind;
};

} // namespace Gujarat

#endif // __HunterGatherer_hxx__

