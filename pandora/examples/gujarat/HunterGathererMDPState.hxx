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
	//! Main constructor
	HunterGathererMDPState(	HunterGatherer* agent
			, HunterGathererMDPConfig* config
			, std::vector< Sector* > * HRActionSectors
			, std::vector< Sector* > * LRActionSectors
			, std::vector< Engine::Point2D<int> > * HRCellPool
			, std::vector< Engine::Point2D<int> > * LRCellPool);
	
	//! Copy constructor
	HunterGathererMDPState( const HunterGathererMDPState& s );
	
	//! Pseudo-copy constructor: uses some information from previous state.
	HunterGathererMDPState( const HunterGathererMDPState& s
							, const Engine::Point2D<int>& location
							, std::vector< Sector* > * HRActionSectors
							, std::vector< Sector* > * LRActionSectors
							, std::vector< Engine::Point2D<int> > * HRCellPool
							, std::vector< Engine::Point2D<int> > * LRCellPool
							, std::vector< bool > ownsItems);
		
	~HunterGathererMDPState();

	
	bool equalIncRastersWithReduct(const Engine::IncrementalRaster  & other)const;

	
	unsigned	hash() const;
	bool		operator==( const HunterGathererMDPState& s ) const;
	bool		operator!=( const HunterGathererMDPState& s ) const;
	
	const HunterGathererMDPState&	operator=(const HunterGathererMDPState& s );	
	
	void		print( std::ostream& os ) const;

	void		increaseTimeIndex() { _timeIndex++; }
	unsigned	getTimeIndex() const { return _timeIndex; }
	int		getOnHandResources() const { return _onHandResources; }
	void addResources( int amt ) { _onHandResources += amt; }

	//! Updates the starvation factor and amount of resources available to the agent at the end of a time step.
	void consume() 
	{ 
		if( _onHandResources < _resourcesDivider ) {
			// If the available resources are less than those that the agent needs, the starvation factor increases.
			_daysStarving += 1000.0f*(1.0f-((float)_onHandResources/(float)_resourcesDivider));
		}
		// At the end of the day, all the resources that have not been consumed are lost.
		_onHandResources = 0;
	}
	
	float getDaysStarving() const { return (float)_daysStarving/1000.0f; }

	const Engine::Point2D<int>& getLocation() const { return _mapLocation; }
	void setLocation(const Engine::Point2D<int>& newLoc ) { _mapLocation = newLoc; }
	
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
	

	
protected:
	HunterGatherer * _agent;
	HunterGathererMDPConfig	* _config;
	
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
	int			_resourcesDivider;
	int 		_daysStarving;
	
	void clearAvailableActions();
	
	void generateActions(const Engine::IncrementalRaster& resourcesRaster, const Engine::Point2D<int>& position);	
};



inline std::ostream& operator<<( std::ostream& os, const HunterGathererMDPState& s )
{
	s.print(os);
	return os;	
}

}

#endif // HunterGathererMDPState.hxx
