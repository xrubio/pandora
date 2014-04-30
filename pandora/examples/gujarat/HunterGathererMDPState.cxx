
#include <Exceptions.hxx>
#include <omp.h>

#include <HunterGathererMDPState.hxx>

namespace Gujarat
{


HunterGathererMDPState::HunterGathererMDPState( const HunterGathererMDPState& s )
: _timeIndex( s._timeIndex )
, _mapLocation( s._mapLocation )
, _onHandResources( s._onHandResources )
, _resources( s._resources )
, _hashKey( s._hashKey )
, _maxResources( s._maxResources)
, _resourcesDivider( s._resourcesDivider )
, _daysStarving( s._daysStarving )
, _HRActionSectors(s._HRActionSectors)
, _LRActionSectors(s._LRActionSectors)
, _HRCellPool(s._HRCellPool)
, _LRCellPool(s._LRCellPool)
, _objectUseCounter(s._objectUseCounter)
, _mapLock(s._mapLock)
, _agentRef(s._agentRef)
,_config(s._config)
{
	std::stringstream logName;
	logName << "logMDPStates_"	<< _agentRef->getWorld()->getId() << "_" << _agentRef->getId();
	
	_dni=dniTicket ();
	
	_creator=1;
	_constructors.push_back(_creator);
	
	_ownItems.resize(s._ownItems.size());
	for(unsigned int i = 0; i < _ownItems.size(); i++)
	{
		_ownItems[i] = s._ownItems[i];
	}
	
	for ( unsigned k = 0; k < _availableActions.size(); k++ )
	{
		delete _availableActions[k];
	}	
	_availableActions.clear();
	for ( unsigned k = 0; k < s._availableActions.size(); k++ )
	{
		addAction( s._availableActions[k]->copy() ); // avoiding segm fault through copy
	}
	assert( s._availableActions.size() == _availableActions.size() );
	
	registerKnowledgeStructuresAtCounterMap();
}


HunterGathererMDPState::HunterGathererMDPState( const HunterGathererMDPState& s
					, const Engine::Point2D<int> loc
					, std::vector< Sector* > * HRActionSectors
					, std::vector< Sector* > * LRActionSectors
					, std::vector< Engine::Point2D<int> > * HRCellPool
					, std::vector< Engine::Point2D<int> > * LRCellPool
					, std::vector< bool > ownItems
				        , const std::vector<MDPAction *>& actionList)
: _timeIndex( s._timeIndex )
, _mapLocation( loc )
, _onHandResources( s._onHandResources )
, _resources( s._resources )
, _hashKey( s._hashKey )
, _maxResources( s._maxResources)
, _resourcesDivider( s._resourcesDivider )
, _daysStarving( s._daysStarving )
, _HRActionSectors(HRActionSectors)
, _LRActionSectors(LRActionSectors)
, _HRCellPool(HRCellPool)
, _LRCellPool(LRCellPool)
, _objectUseCounter(s._objectUseCounter)
, _mapLock(s._mapLock)
, _agentRef(s._agentRef)
, _config(s._config)
, _availableActions(actionList)
{
	std::stringstream logName;
	logName << "logMDPStates_"	<< _agentRef->getWorld()->getId() << "_" << _agentRef->getId();
	
		
	_dni=dniTicket ();
	
	
	_ownItems.resize(ownItems.size());
	for(int i = 0; i < ownItems.size(); i++)
	{
		_ownItems[i] = ownItems[i];
	}
	
	registerKnowledgeStructuresAtCounterMap();
	
	computeHash();
	
	_creator=2;
	_constructors.push_back(_creator);
}



HunterGathererMDPState::HunterGathererMDPState( 
			HunterGatherer * agentRef
			, HunterGathererMDPConfig * config
			, const Engine::Point2D<int> loc
			, int initResources
			, Engine::Raster& resourcesRaster
			, int maxResources
			, int divider 
			, std::vector< Sector* > * HRActionSectors
			, std::vector< Sector* > * LRActionSectors
			, std::vector< Engine::Point2D<int> > * HRCellPool
			, std::vector< Engine::Point2D<int> > * LRCellPool
			, std::vector< bool > ownItems
			, std::map<unsigned long,long> * objectUseCounter
			, omp_lock_t * mapLock
			, const std::vector<MDPAction *>&  actionList)

	: _timeIndex(0)
	, _mapLocation( loc )
	, _onHandResources( initResources )
	, _resources( resourcesRaster )
	, _maxResources( maxResources )
	, _resourcesDivider( divider )
	, _HRActionSectors( HRActionSectors )
	, _LRActionSectors( LRActionSectors )	
	, _HRCellPool( HRCellPool )
	, _LRCellPool( LRCellPool )
	, _ownItems( ownItems )
	, _daysStarving( 0 )
	, _objectUseCounter(objectUseCounter)
	, _mapLock(mapLock)
	, _agentRef(agentRef)
	,_config(config)
	,_availableActions(actionList)
{
	std::stringstream logName;
	logName << "logMDPStates_"	<< _agentRef->getWorld()->getId() << "_" << _agentRef->getId();
	

	_dni=dniTicket ();
	
	_creator=3;	
	_constructors.push_back(_creator);

	_ownItems.resize(ownItems.size());
	for(unsigned int i = 0; i < ownItems.size(); i++)
	{
		_ownItems[i] = ownItems[i];
	}
	
	computeHash();	
	registerKnowledgeStructuresAtCounterMap();
}



const HunterGathererMDPState& HunterGathererMDPState::operator=( const HunterGathererMDPState& s )
{	
	std::stringstream logName;
	logName << "logMDPStates_"	<< _agentRef->getWorld()->getId() << "_" << _agentRef->getId();
	

	deRegisterFromCounterMapAndDeleteKnowledgeStructures();
	
	_dni=dniTicket ();
	
	_creator=4;
	_constructors.push_back(_creator);
	
	_timeIndex 		 = s._timeIndex;
	_mapLocation 	 = s._mapLocation;
	_onHandResources = s._onHandResources;
	_resources 		 = s._resources;
	_hashKey 		 = s._hashKey;
	_maxResources 	 = s._maxResources;
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
	
	for ( unsigned k = 0; k < _availableActions.size(); k++ )
	{
		delete _availableActions[k];
	}
	_availableActions.clear();
	for ( unsigned k = 0; k < s._availableActions.size(); k++ )
	{
		addAction( s._availableActions[k]->copy() );
	}
	assert( s._availableActions.size() == _availableActions.size() );
	
	registerKnowledgeStructuresAtCounterMap();	
	
	_agentRef = s._agentRef;
	
	_config = s._config;
	
	return *this;
}

	

HunterGathererMDPState::~HunterGathererMDPState()
{
	std::stringstream logName;
	// look out: if 'this' is an _initial of a HGMDPModel && step==361 && _agentRef killed at 360, then the next line produces segm fault :
	//logName << "logMDPStates_"	<< _agentRef->getWorld()->getId() << "_" << _agentRef->getId();
	
	for ( unsigned int k = 0; k < _availableActions.size(); k++ )
	{
		delete _availableActions[k];
	}
	
	
	deRegisterFromCounterMapAndDeleteKnowledgeStructures();
}

void	HunterGathererMDPState::addAction( MDPAction* a )
{
	_availableActions.push_back(a);
}


void	HunterGathererMDPState::computeHash()
{
	_hashKey.add( _timeIndex );
	
	GujaratWorld * gw = (GujaratWorld*)_agentRef->getWorld();	
	Engine::Point2D<int> mapLocationLR;
	gw->worldCell2LowResCell(_mapLocation,mapLocationLR);
	
	_hashKey.add( mapLocationLR._x );
	_hashKey.add( mapLocationLR._y );
	
	/*_hashKey.add( _mapLocation._x );
	_hashKey.add( _mapLocation._y );*/
	_hashKey.add( _agentRef->reductionResourcesToCategory(_onHandResources) );
	_hashKey.add( _daysStarving );	
	
	for ( Engine::IncrementalRaster::ChangeIterator it = _resources.firstChange();
		it != _resources.endOfChanges(); it++ )
	{
		const Engine::Point2D<int>& p = it->first;
		_hashKey.add( p._x );
		_hashKey.add( p._y );
		_hashKey.add( _agentRef->reductionResourcesToCategory(_agentRef->convertBiomassToCalories(it->second)) );
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
		if ( _agentRef->reductionResourcesToCategory(_agentRef->convertBiomassToCalories(i->second)) 
			!= 
			_agentRef->reductionResourcesToCategory(_agentRef->convertBiomassToCalories(j->second)) ) return false;
	}
	return true;
}


bool	HunterGathererMDPState::operator==( const HunterGathererMDPState& s ) const
{
	// due to the term condition ( _resources == s._resources ) 			
	// we need introduce the "reductionResourcesToCategory" to comparison
	
	GujaratWorld * gw = (GujaratWorld*)_agentRef->getWorld();
	
	Engine::Point2D<int> mapLocationLR;
	gw->worldCell2LowResCell(_mapLocation,mapLocationLR);
		
	Engine::Point2D<int> sMapLocationLR;
	gw->worldCell2LowResCell(s._mapLocation,sMapLocationLR);	
	
	bool result = ( _timeIndex == s._timeIndex ) 
		&&
			( _agentRef->reductionResourcesToCategory(_onHandResources) 
			== _agentRef->reductionResourcesToCategory(s._onHandResources) ) 
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
			<< "\n dni's :" << _dni << "," << s._dni
			<< "\n sectors's :" << _LRActionSectors->size() << "," << s._LRActionSectors->size()
			<< "\n constructor " << _creator << " for state " << _dni
			<< "\n constructor " << s._creator << " for state " << s._dni
			;
		
		throw Engine::Exception(oss.str());
	}
	
	
	return result;
}

bool	HunterGathererMDPState::operator!=( const HunterGathererMDPState& s ) const { return !(*this == s); }

void	HunterGathererMDPState::print( std::ostream& os ) const
{
	os << "<addr = " << this << ", ";
	os << "dni = " << _dni << ", ";
	os << "constructor = " << _creator << ", ";
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
			<< _objectUseCounter->count((unsigned long)_LRActionSectors)
			<< " ; the state was created with constructor " << _creator
			<< " ; the state visited as much constructors as "<< _constructors.size() << ":";
			for(std::vector<int>::iterator it = _constructors.begin(); it !=_constructors.end(); ++it) oss<< _constructors.size();
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
	
	
	
	
}

