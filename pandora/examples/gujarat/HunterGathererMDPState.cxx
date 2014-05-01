
#include <Exceptions.hxx>
#include <omp.h>

#include <HunterGathererMDPState.hxx>

namespace Gujarat
{

//! Main constructor
HunterGathererMDPState::HunterGathererMDPState( 
			HunterGatherer* agent
			, HunterGathererMDPConfig * config
			, std::vector< Sector* > * HRActionSectors
			, std::vector< Sector* > * LRActionSectors
			, std::vector< Engine::Point2D<int> > * HRCellPool
			, std::vector< Engine::Point2D<int> > * LRCellPool)

	: _timeIndex(0)
	, _mapLocation( agent->getPosition() )
	, _onHandResources( agent->getOnHandResources() )
	, _resources( agent->getLRResourcesRaster() )
	, _resourcesDivider( agent->computeConsumedResources(1) )
	, _HRActionSectors( HRActionSectors )
	, _LRActionSectors( LRActionSectors )	
	, _HRCellPool( HRCellPool )
	, _LRCellPool( LRCellPool )
	, _ownItems( 4, false )
	, _daysStarving( 0 )
	, _objectUseCounter(agent->getObjectUseCounter())
	, _mapLock(agent->getMapLock())
	, _agent(agent)
	,_config(config)
	,_availableActions(0)
{
	generateActions(agent->getLRResourcesRaster(), agent->getPosition());
	computeHash();
	registerKnowledgeStructuresAtCounterMap();
}

//! Copy constructor
HunterGathererMDPState::HunterGathererMDPState( const HunterGathererMDPState& s )
: _timeIndex( s._timeIndex )
, _mapLocation( s._mapLocation )
, _onHandResources( s._onHandResources )
, _resources( s._resources )
, _hashKey( s._hashKey )
, _resourcesDivider( s._resourcesDivider )
, _daysStarving( s._daysStarving )
, _HRActionSectors(s._HRActionSectors)
, _LRActionSectors(s._LRActionSectors)
, _HRCellPool(s._HRCellPool)
, _LRCellPool(s._LRCellPool)
, _ownItems(s._ownItems)
, _objectUseCounter(s._objectUseCounter)
, _mapLock(s._mapLock)
, _agent(s._agent)
,_config(s._config)
,_availableActions(0)
{
	for ( unsigned k = 0; k < s._availableActions.size(); k++ ) {
		_availableActions.push_back( s._availableActions[k]->copy() ); // avoiding segm fault through copy
	}
	
	registerKnowledgeStructuresAtCounterMap();
}

//! Pseudo-copy constructor: uses some information from previous state.
HunterGathererMDPState::HunterGathererMDPState( const HunterGathererMDPState& s
					, const Engine::Point2D<int>& location
					, std::vector< Sector* > * HRActionSectors
					, std::vector< Sector* > * LRActionSectors
					, std::vector< Engine::Point2D<int> > * HRCellPool
					, std::vector< Engine::Point2D<int> > * LRCellPool
					, std::vector< bool > ownItems)
: _timeIndex( s._timeIndex )
, _mapLocation( location )
, _onHandResources( s._onHandResources )
, _resources( s._resources )
, _hashKey( s._hashKey )
, _resourcesDivider( s._resourcesDivider )
, _daysStarving( s._daysStarving )
, _HRActionSectors(HRActionSectors)
, _LRActionSectors(LRActionSectors)
, _HRCellPool(HRCellPool)
, _LRCellPool(LRCellPool)
, _ownItems(ownItems)
, _objectUseCounter(s._objectUseCounter)
, _mapLock(s._mapLock)
, _agent(s._agent)
, _config(s._config)
, _availableActions(0)
{
	generateActions(s.getResourcesRaster(), location);
	computeHash();
	registerKnowledgeStructuresAtCounterMap();
}


const HunterGathererMDPState& HunterGathererMDPState::operator=( const HunterGathererMDPState& s )
{	
	std::stringstream logName;
	logName << "logMDPStates_"	<< _agent->getWorld()->getId() << "_" << _agent->getId();
	

	deRegisterFromCounterMapAndDeleteKnowledgeStructures();
	
	_timeIndex 		 = s._timeIndex;
	_mapLocation 	 = s._mapLocation;
	_onHandResources = s._onHandResources;
	_resources 		 = s._resources;
	_hashKey 		 = s._hashKey;
	_resourcesDivider = s._resourcesDivider;
	_daysStarving 	 = s._daysStarving;

	_objectUseCounter = s._objectUseCounter;
	_mapLock = s._mapLock;
	
	_HRActionSectors = s._HRActionSectors;
	_LRActionSectors = s._LRActionSectors;
	_HRCellPool = s._HRCellPool;
	_LRCellPool = s._LRCellPool;


	_ownItems.resize(s._ownItems.size());
	for(unsigned int i = 0; i < _ownItems.size(); i++)
	{
		_ownItems[i] = s._ownItems[i];
	}
	
	clearAvailableActions();
	
	for ( unsigned k = 0; k < s._availableActions.size(); k++ )
	{
		_availableActions.push_back( s._availableActions[k]->copy() );
	}
	assert( s._availableActions.size() == _availableActions.size() );
	
	registerKnowledgeStructuresAtCounterMap();	
	
	_agent = s._agent;
	
	_config = s._config;
	
	return *this;
}

HunterGathererMDPState::~HunterGathererMDPState()
{
	// std::stringstream logName;
	// look out: if 'this' is an _initial of a HGMDPModel && step==361 && _agent killed at 360, then the next line produces segm fault :
	//logName << "logMDPStates_"	<< _agent->getWorld()->getId() << "_" << _agent->getId();
	
	clearAvailableActions();
	
	deRegisterFromCounterMapAndDeleteKnowledgeStructures();
}

void	HunterGathererMDPState::computeHash()
{
	_hashKey.add( _timeIndex );
	
	GujaratWorld * gw = (GujaratWorld*)_agent->getWorld();	
	Engine::Point2D<int> mapLocationLR;
	gw->worldCell2LowResCell(_mapLocation,mapLocationLR);
	
	_hashKey.add( mapLocationLR._x );
	_hashKey.add( mapLocationLR._y );
	
	_hashKey.add( _agent->reductionResourcesToCategory(_onHandResources) );
	_hashKey.add( _daysStarving );	
	
	for ( Engine::IncrementalRaster::ChangeIterator it = _resources.firstChange();
		it != _resources.endOfChanges(); it++ )
	{
		const Engine::Point2D<int>& p = it->first;
		_hashKey.add( p._x );
		_hashKey.add( p._y );
		_hashKey.add( _agent->reductionResourcesToCategory(_agent->convertBiomassToCalories(it->second)) );
	}
}

unsigned HunterGathererMDPState::hash() const
{
	return _hashKey.code(); 
}


bool HunterGathererMDPState::equalIncRastersWithReduct(const Engine::IncrementalRaster  & other) const
{
	for ( Engine::IncrementalRaster::ChangeIterator i = _resources.firstChange();
		i != _resources.endOfChanges(); i++ )	
	{
		Engine::IncrementalRaster::ChangeIterator j = other.getChanges().find( i->first );
		
		if ( j == other.endOfChanges() ) return false;
		if ( _agent->reductionResourcesToCategory(_agent->convertBiomassToCalories(i->second)) 
			!= 
			_agent->reductionResourcesToCategory(_agent->convertBiomassToCalories(j->second)) ) return false;
	}
	return true;
}


bool	HunterGathererMDPState::operator==( const HunterGathererMDPState& s ) const
{
	// due to the term condition ( _resources == s._resources ) 			
	// we need introduce the "reductionResourcesToCategory" to comparison
	
	GujaratWorld * gw = (GujaratWorld*)_agent->getWorld();
	
	Engine::Point2D<int> mapLocationLR;
	gw->worldCell2LowResCell(_mapLocation,mapLocationLR);
		
	Engine::Point2D<int> sMapLocationLR;
	gw->worldCell2LowResCell(s._mapLocation,sMapLocationLR);	
	
	bool result = ( _timeIndex == s._timeIndex ) 
		&&
			( _agent->reductionResourcesToCategory(_onHandResources) 
			== _agent->reductionResourcesToCategory(s._onHandResources) ) 
		&&
			( mapLocationLR == sMapLocationLR ) //*? TODO should be LR
		&&
			equalIncRastersWithReduct(s._resources)			
		&&
			( _daysStarving == s._daysStarving )
		;

	if(result && _availableActions.size() != s._availableActions.size())
	{
		std::stringstream oss;
		oss << "Incoherence!!! same location, different number of actions:" 
			<< _availableActions.size() << "!=" << s._availableActions.size()
			<< "\n locations : " << _mapLocation << " == " << s._mapLocation 
			<< "\n sectors's :" << _LRActionSectors->size() << "," << s._LRActionSectors->size()
			;
		
		throw Engine::Exception(oss.str());
	}
	
	
	return result;
}

bool	HunterGathererMDPState::operator!=( const HunterGathererMDPState& s ) const { return !(*this == s); }

void	HunterGathererMDPState::print( std::ostream& os ) const
{
	os << "<addr = " << this << ", ";
	os << "loc=(" << _mapLocation._x << ", " << _mapLocation._y << "), ";
	os << "res=" << _onHandResources << ", ";
	os << "t=" << _timeIndex << ", ";
	os << "starv=" << _daysStarving << ", ";
	
	os << "changes=(";
	for ( Engine::IncrementalRaster::ChangeIterator it = _resources.firstChange();
		it != _resources.endOfChanges(); it++ )
	{
		const Engine::Point2D<int>& p = it->first;
		os << "( ( " << p._x << ", ";
		os << p._y << "), ";
		os << it->second << ") ";
	}

	os << "), ";
	os << "actions (total: " << _availableActions.size() << "):";
  	for(unsigned i = 0; i < _availableActions.size(); ++i)
  		os << ": " << _availableActions[i]->describe() <<" ";

	os << std::endl;	
}



void HunterGathererMDPState::registerKnowledgeStructuresAtCounterMap()
{		
		omp_set_lock(_mapLock);		
		
		if (_objectUseCounter->count((unsigned long)_mapLock) > 0)
		{
			(*_objectUseCounter)[(unsigned long)_mapLock]++;			
		}
		else
		{
			(*_objectUseCounter)[(unsigned long)_mapLock]=1;		
		}
		
		
		if (_objectUseCounter->count((unsigned long)_LRActionSectors) > 0)
		{			
			(*_objectUseCounter)[(unsigned long)_LRActionSectors]++;
		}
		else
		{
			(*_objectUseCounter)[(unsigned long)_LRActionSectors]=1;
		}
		
		
		if (_objectUseCounter->count((unsigned long)_LRCellPool) > 0)
		{
			(*_objectUseCounter)[(unsigned long)_LRCellPool]++;
		}
		else
		{
			(*_objectUseCounter)[(unsigned long)_LRCellPool]=1;
		}
	
	
		omp_unset_lock(_mapLock);
}
	

void HunterGathererMDPState::deRegisterFromCounterMapAndDeleteKnowledgeStructures()
	{
		
		//std::cout << "DELETING state. thread [" << omp_get_thread_num() <<"]"<< std::endl;
		omp_set_lock(_mapLock);
		
		
		//{ PreCondition : reader counter LRActionSector = X && X>0 }
		
		if(_objectUseCounter->count((unsigned long)_LRActionSectors) <= 0)
		{
			std::stringstream oss;
			oss << "HunterGathererMDPState::deRegisterFromCounterMapAndDeleteKnowledgeStructures(): "
			<< " Found wrong counter for a LRActionSectors readers/writers:" 
			<< _objectUseCounter->count((unsigned long)_LRActionSectors);
			throw Engine::Exception(oss.str());
			
		}
		
		//{ holds : reader counter LRActionSector = X && X > 0 }
		
		(*_objectUseCounter)[(unsigned long)_LRActionSectors]--;
			
		//{ holds : reader counter LRActionSector = X-1 && X >= 0 }
		
		if((*_objectUseCounter)[(unsigned long)_LRActionSectors] == 0)
		{
			_objectUseCounter->erase((unsigned long)_LRActionSectors);
			
			if (_ownItems[1])
			{				
				for(unsigned int i=0;i<_LRActionSectors->size();i++)
				{		
					delete (*_LRActionSectors)[i];		
				}			
		
				delete _LRActionSectors;
			}
		
		}
		
	
		//{ holds : reader counter of LRActionSector cell pool = X && X > 0 }
	
		if(_objectUseCounter->count((unsigned long)_LRCellPool) <= 0)
		{
			std::stringstream oss;
			oss << "HunterGathererMDPState::deRegisterFromCounterMapAndDeleteKnowledgeStructures(): "
			<< " Found wrong counter for a LRA cell pool readers/writers:" 
			<< _objectUseCounter->count((unsigned long)_LRCellPool);
			throw Engine::Exception(oss.str());
		}	
	
		//{ holds : reader counter of LRActionSector cell pool = X && X > 0 }
		(*_objectUseCounter)[(unsigned long)_LRCellPool]--;
		//{ holds : reader counter of LRActionSector cell pool = X-1 && X >= 0 }
		
		if((*_objectUseCounter)[(unsigned long)_LRCellPool] == 0)
		{
			_objectUseCounter->erase((unsigned long)_LRCellPool);	
			if (_ownItems[3])
			{
				delete _LRCellPool;
			}
		
		}
		
		if (_objectUseCounter->count((unsigned long)_mapLock) > 0)
		{
			(*_objectUseCounter)[(unsigned long)_mapLock]--;
			if((*_objectUseCounter)[(unsigned long)_mapLock] == 0)
			{
				//*?
		//		_objectUseCounter->erase((unsigned long)_mapLock);	
			}
			
		}
		
		omp_unset_lock(_mapLock);
		if ((_objectUseCounter->count((unsigned long)_mapLock) == 0))
		{
		//	omp_destroy_lock(_mapLock);
		//	delete _mapLock;
		}
		
	}	
	
	
	void HunterGathererMDPState::clearRefCounterMap() 
	{ 
		//#pragma omp critical(refmap){_objectUseCounter->clear();}
	}
	
	//! Clear and free the vector of available actions
	void HunterGathererMDPState::clearAvailableActions() {
		for ( unsigned k = 0; k < _availableActions.size(); ++k ) {
			delete _availableActions[k];
		}
		_availableActions.clear();
	}	


/**
 * 
 */
void HunterGathererMDPState::generateActions(const Engine::IncrementalRaster& resourcesRaster, const Engine::Point2D<int>& position)
{
	std::stringstream logName;
	logName << "logMDPStates_"	<< _agent->getWorld()->getId() << "_" << _agent->getId();
	
	// Map from "sector memory address" to "sector integer identifier".
	// After sorting validActionSectors I need to access both the HR and the LR sector
	std::map<unsigned long,int> sectorIdxMap;

	//std::cout << "creating actions for state with time index: " << s.getTimeIndex() << " and resources: " << s.getOnHandResources() << std::endl;
	// Make Do Nothing	
	if ( _config->isDoNothingAllowed() )
		_availableActions.push_back( new DoNothingAction() );	

	std::vector< Sector* > validActionSectors;

	_agent->updateKnowledge( position, resourcesRaster, _HRActionSectors, _LRActionSectors, _HRCellPool, _LRCellPool );
	
	
	// MRJ: Remove empty sectors if any
	for ( unsigned i = 0; i < _LRActionSectors->size(); i++ )
	{
		if ( (*_LRActionSectors)[i]->isEmpty() )
		{
			continue;
		}
		validActionSectors.push_back( (*_LRActionSectors)[i] );
		sectorIdxMap[(unsigned long)(*_LRActionSectors)[i]] = i;
	}	
	std::sort( validActionSectors.begin(), validActionSectors.end(), SectorBestFirstSortPtrVecPredicate() );
	
	// Make Forage actions
	for ( unsigned i = 0; i < validActionSectors.size(); i++ )
	{
		int sectorIdx = sectorIdxMap[(unsigned long)(validActionSectors[i])];
		_availableActions.push_back( new ForageAction( (*_HRActionSectors)[sectorIdx], validActionSectors[i], false ) );	
	}
	
	// Make Move Home
	std::vector< MoveHomeAction* > possibleMoveHomeActions;
	MoveHomeAction::generatePossibleActions( *_agent, position, *_HRActionSectors, validActionSectors, possibleMoveHomeActions );
	unsigned int moveHomeActions = _config->getNumberMoveHomeActions();
	if (  moveHomeActions >=  possibleMoveHomeActions.size() )
	{
		for ( unsigned i = 0; i < possibleMoveHomeActions.size(); i++ )
			_availableActions.push_back( possibleMoveHomeActions[i] );
	}
	else
	{
		for ( unsigned i = 0; i <  moveHomeActions; i++ )
			_availableActions.push_back( possibleMoveHomeActions[i] );
		for ( unsigned i =  moveHomeActions; i < possibleMoveHomeActions.size(); i++ )
			delete possibleMoveHomeActions[i];
	}
	
	assert( _availableActions.size() > 0 );
	sectorIdxMap.clear();
	possibleMoveHomeActions.clear();
	validActionSectors.clear();
	
	// Reference to structures that could reference structures from a MDPState cannot be destroyed. The MDPState's will destroy the ones created and are owned.	
	//_HRActionSectors.clear();
	//LRActionSectors.clear();
	//std::cout << "finished creating actions for state with time index: " << parent.getTimeIndex() << " and resources: " << parent.getOnHandResources() << std::endl;
} 



} // namespace