#ifndef __HunterGathererMDPState_hxx__
#define __HunterGathererMDPState_hxx__

#include <Logger.hxx>

#include <Point2D.hxx>
#include <IncrementalRaster.hxx>
#include <HashTable.hxx>
#include <MDPAction.hxx>
#include <engine/problem.h>

#include <map>
#include <omp.h>
#include <sstream>

#include "Sector.hxx"

#include "GujaratWorld.hxx"
#include "HunterGatherer.hxx"
#include "GujaratConfig.hxx"
#include "HunterGathererMDPConfig.hxx"
#include "MDPAction.hxx"
#include "ForageAction.hxx"
#include "MoveHomeAction.hxx"
#include "DoNothingAction.hxx"

namespace Gujarat
{

class HunterGathererMDPState
{
public:
	HunterGathererMDPState(	HunterGatherer * agentRef
			, HunterGathererMDPConfig * config
			, const Engine::Point2D<int> loc
			, int initialOnHand
			, Engine::Raster& resourcesRaster
			, int maxResources
			, int divider
			, std::vector< Sector* > * HRActionSectors
			, std::vector< Sector* > * LRActionSectors
			, std::vector< Engine::Point2D<int> > * HRCellPool
			, std::vector< Engine::Point2D<int> > * LRCellPool
			, std::vector< bool > ownsItems
			, std::map<unsigned long,long> * objectUseCounter
			, omp_lock_t * mapLock
			, const std::vector<MDPAction *>&  actionList);
	
	HunterGathererMDPState( const HunterGathererMDPState& s );
	
	HunterGathererMDPState( const HunterGathererMDPState& s
							, const Engine::Point2D<int> loc
							, std::vector< Sector* > * HRActionSectors
							, std::vector< Sector* > * LRActionSectors
							, std::vector< Engine::Point2D<int> > * HRCellPool
							, std::vector< Engine::Point2D<int> > * LRCellPool
							, std::vector< bool > ownsItems
			                , const std::vector<MDPAction *>& actionList);
		
	~HunterGathererMDPState();

	
	bool equalActions(MDPAction *a, MDPAction *b) const;
	
	bool equalIncRastersWithReduct(const Engine::IncrementalRaster  & other)const;

	
	static int reductionResourcesToCategory(long r)
	{
		/*
			r = #calories or #resources.
			r/2000 specifies aprox needs per HG per day
		 */		
		r = r/2000;
		if (r < 2) return 0;
		if (r < 9) return 1;
		if (r < 21) return 2;
		if (r < 40) return 3;
		return 4;		
		
		//return GujaratAgent::reductionResourcesToCategory(r);
	}		
	
	unsigned	hash() const;
	bool		operator==( const HunterGathererMDPState& s ) const;
	bool		operator!=( const HunterGathererMDPState& s ) const;
	
	const HunterGathererMDPState&	operator=(const HunterGathererMDPState& s );	
	
	void		print( std::ostream& os ) const;

	void		increaseTimeIndex() { _timeIndex++; }
	unsigned	getTimeIndex() const { return _timeIndex; }
	int		getOnHandResources() const { return _onHandResources; }
	void addResources( int amt ) { _onHandResources += amt; }

	void consume() 
	{ 
		if( _onHandResources < _resourcesDivider ) {
			_daysStarving += 1000.0f*(1.0f-((float)_onHandResources/(float)_resourcesDivider));
			//std::cout << "consume of this: " << this << " with days starving: " << _daysStarving << std::endl;
		}
		_onHandResources = 0;
	}
	
	float getDaysStarving() const { return (float)_daysStarving/1000.0f; }

	const Engine::Point2D<int>& getLocation() const { return _mapLocation; }
	void setLocation( Engine::Point2D<int> newLoc ) { _mapLocation = newLoc; }
	
	Engine::IncrementalRaster& getResourcesRaster() { return _resources; }
	const Engine::IncrementalRaster& getResourcesRaster() const { return _resources; }

	MDPAction* availableActions( Problem::action_t actIndex )  { return _availableActions.at(actIndex); }
	const MDPAction* availableActions( Problem::action_t actIndex ) const  { return _availableActions.at(actIndex); }

	unsigned numAvailableActions() const { return _availableActions.size(); }
	
	void	computeHash();
	
	std::vector< Sector* > * getHRActionSectors() const { return _HRActionSectors; }
	std::vector< Sector* > * getLRActionSectors() const { return _LRActionSectors; }
	std::vector< Engine::Point2D<int> > * getHRCellPool() const { return _HRCellPool; }
	std::vector< Engine::Point2D<int> > * getLRCellPool() const { return _LRCellPool; }	
	
	const std::vector< bool > & getOwnerShip() const { return _ownItems;} 
	
	static void clearRefCounterMap();
	
	void registerKnowledgeStructuresAtCounterMap();
	
	void deRegisterFromCounterMapAndDeleteKnowledgeStructures();
	

	
public:
	HunterGatherer * _agentRef;
	HunterGathererMDPConfig	* _config;
	
protected:	
	std::vector< Sector* > * _HRActionSectors;// High Resolution
	std::vector< Sector* > * _LRActionSectors;// Low Resolution
	std::vector< Engine::Point2D<int> > * _HRCellPool;
	std::vector< Engine::Point2D<int> > * _LRCellPool;
	/* _ownItems SEMANTICS 
		if (_ownsItems[0])
			delete _HRActionSectors;;
		if (_ownsItems[1])
			delete _LRActionSectors;;
		if (_ownsItems[2])
			delete _HRCellPool;;
		if (_ownsItems[3])
			delete _LRCellPool;;
	*/
	std::vector< bool > _ownItems;

	/* There is some posibility of interference between threads and
	 * data corruption in _objecUseCounter? RACE CONDITIONS?
	 * Threads belong to a same process, they share same space address
	 * Each threads registers different objects here. Each object with a
	 * different memory address. Will two different threads use the same
	 * address from a MDPState object? I dont think so. 
	 */
	std::map<long unsigned int,long> * _objectUseCounter;
	omp_lock_t * _mapLock;
	
	unsigned		_timeIndex;
	Engine::Point2D<int>	_mapLocation;
	int			_onHandResources;
	
	Engine::IncrementalRaster	_resources;
	Engine::HashKey		_hashKey;
	std::vector<MDPAction*> _availableActions;
	int			_maxResources;
	int			_resourcesDivider;
	int 		_daysStarving;
};



inline std::ostream& operator<<( std::ostream& os, const HunterGathererMDPState& s )
{
	s.print(os);
	return os;	
}

}

#endif // HunterGathererMDPState.hxx
