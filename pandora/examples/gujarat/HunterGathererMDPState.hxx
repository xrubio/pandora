#ifndef __HunterGathererMDPState_hxx__
#define __HunterGathererMDPState_hxx__

#include <Point2D.hxx>
#include <IncrementalRaster.hxx>
#include <HashTable.hxx>
#include <MDPAction.hxx>
#include <engine/problem.h>

#include <map>
//#include <unordered_map>

//#include <boost/thread/mutex.hpp>

#include "Sector.hxx"

namespace Gujarat
{


	
class HunterGathererMDPState
{
	//*?
	//static std::vector< std::unordered_map<long,long> > _objectUseCounter;
	//boost::mutex _mtx;
	static std::map<long,long> _objectUseCounter;
	/* There is some posibility of interference between threads and
	 * data corruption in _objecUseCounter? RACE CONDITIONS?
	 * Threads belong to a same process, they share same space address
	 * Each threads registers different objects here. Each object with a
	 * different memory address. Will two different threads use the same
	 * address from a MDPState object? I dont think so. 
	 */
	
	
public:
	int _creator;
	int _dni;
	std::string _actionName;
	
protected:	
	
	static std::vector< Sector* >				_emptySectorVector;
	static std::vector< Engine::Point2D<int> >  _emptyCellPool;	
	
	std::vector< Sector* > * _HRActionSectors;// High Resolution
	std::vector< Sector* > * _LRActionSectors;// Low Resolution
	std::vector< Engine::Point2D<int> > * _HRCellPool;
	std::vector< Engine::Point2D<int> > * _LRCellPool;
	std::vector< bool > _ownItems;
	
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
	
public:
	// Constructors, I don't want this to be ever invoked
	//explicit HunterGathererMDPState();

	//HunterGathererMDPState( HunterGathererMDPState& s, bool ownership[] );
	
	
	
	// The real one
	HunterGathererMDPState(	const Engine::Point2D<int> loc
							, int initialOnHand
							, Engine::Raster& resourcesRaster
							, int maxResources
							, int divider
							, std::vector< Sector* > * HRActionSectors
							, std::vector< Sector* > * LRActionSectors
							, std::vector< Engine::Point2D<int> > * HRCellPool
							, std::vector< Engine::Point2D<int> > * LRCellPool
							, std::vector< bool > ownsItems);
	
	HunterGathererMDPState( const HunterGathererMDPState& s );
	
	HunterGathererMDPState( const HunterGathererMDPState& s, HunterGathererMDPState *y);
	
	HunterGathererMDPState( const HunterGathererMDPState& s
							, std::vector< Sector* > * HRActionSectors
							, std::vector< Sector* > * LRActionSectors
							, std::vector< Engine::Point2D<int> > * HRCellPool
							, std::vector< Engine::Point2D<int> > * LRCellPool
							, std::vector< bool > ownsItems );
	
	
	const HunterGathererMDPState&	operator=(const HunterGathererMDPState& s );
	
	~HunterGathererMDPState();

	//void		initializeSuccessor( HunterGathererMDPState& s, const std::vector< bool > ownership ) const;

	unsigned	hash() const;
	bool		operator==( const HunterGathererMDPState& s ) const;
	bool		operator!=( const HunterGathererMDPState& s ) const;
	bool		operator<( const HunterGathererMDPState& s ) const;

	void		print( std::ostream& os ) const;

	void		increaseTimeIndex() { _timeIndex++; }
	unsigned	getTimeIndex() const { return _timeIndex; }
	int			getOnHandResources() const { return _onHandResources; }
	void addResources( int amt )
	{
		_onHandResources += amt;
		/*
		if(_onHandResources > _resourcesDivider*_maxResources)
		{
			_onHandResources = _resourcesDivider*_maxResources;
		}
		*/
		/*
		_onHandResources = _onHandResources + (amt / _resourcesDivider); 
		if ( _onHandResources > _maxResources )
			_onHandResources = _maxResources;
			*/
	}

	void consume() 
	{ 
		if( _onHandResources < _resourcesDivider )
		{
			_daysStarving += 1000.0f*(1.0f-((float)_onHandResources/(float)_resourcesDivider));
			//std::cout << "consume of this: " << this << " with days starving: " << _daysStarving << std::endl;
			//_onHandResources -= _resourcesDivider;
			//_daysStarving=0;
		}
		//_onHandResources = 0;
		/*
		else
		{
			_onHandResources = 0;
			_daysStarving++;
		}
		*/
	}
	
	float getDaysStarving() const
	{
		//std::cout << "getDaysStarving for: " << this << " days starving: " << _daysStarving << " fraction: " << (float)_daysStarving/1000.0f << std::endl;
		return (float)_daysStarving/1000.0f;
	}

	//void					spoilage( float v ) { _onHandResources = (float)_onHandResources * v; }
	void setLocation( Engine::Point2D<int> newLoc ) { _mapLocation = newLoc; }
	const Engine::Point2D<int>& 		getLocation() const { return _mapLocation; }
	Engine::IncrementalRaster& 			getResourcesRaster() { return _resources; }
	const Engine::IncrementalRaster& 	getResourcesRaster() const { return _resources; }

	void addAction( MDPAction* a );
	MDPAction* availableActions( Problem::action_t actIndex ) 
		{ return _availableActions.at(actIndex); }
	const MDPAction* availableActions( Problem::action_t actIndex ) const 
		{ return _availableActions.at(actIndex); }

	unsigned	numAvailableActions() const { return _availableActions.size(); }

	void	computeHash();
	
	
	std::vector< Sector* > * getHRActionSectors() const { return _HRActionSectors; }
	std::vector< Sector* > * getLRActionSectors() const { return _LRActionSectors; }
	std::vector< Engine::Point2D<int> > * getHRCellPool() const { return _HRCellPool; }
	std::vector< Engine::Point2D<int> > * getLRCellPool() const { return _LRCellPool; }	
	
	const std::vector< bool > & getOwnerShip() const { return _ownItems;} 
	
	//*?
	int dniTicket () {
		static int counter = 0;
		return counter++;
	}
	
	//static void resetTicket() { HunterGathererMDPState::counter = 0; }
	
	static void clearRefCounterMap();
	
	void registerKnowledgeStructuresAtCounterMap();
	
	void deRegisterFromCounterMapAndDeleteKnowledgeStructures();
	
	
private:
	

private:
	unsigned					_timeIndex;
	Engine::Point2D<int>		_mapLocation;
	int							_onHandResources;
	
	Engine::IncrementalRaster	_resources;
	Engine::HashKey				_hashKey;
	std::vector<MDPAction*> 	_availableActions;
	int							_maxResources;
	int							_resourcesDivider;
	int 						_daysStarving;
	bool						_isCopy;
};

//std::vector<abc> xyz::myvector;

inline std::ostream& operator<<( std::ostream& os, const HunterGathererMDPState& s )
{
	s.print(os);
	return os;	
}

}

#endif // HunterGathererMDPState.hxx
