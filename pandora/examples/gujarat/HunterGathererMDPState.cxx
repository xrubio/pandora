
#include <HunterGathererMDPState.hxx>



#include <Exceptions.hxx>

//#include <mutex>

#include <omp.h>

namespace Gujarat
{

/*
HunterGathererMDPState::HunterGathererMDPState()
{
	 throw Engine::Exception("Forbidden Constructor HunterGathererMDPState::HunterGathererMDPState() has been called\n");	
}*/



HunterGathererMDPState::HunterGathererMDPState( const HunterGathererMDPState& s )
: _timeIndex( s._timeIndex )
, _mapLocation( s._mapLocation )
, _onHandResources( s._onHandResources )
, _resources( s._resources )
, _hashKey( s._hashKey )
, _maxResources( s._maxResources)
, _resourcesDivider( s._resourcesDivider )
, _daysStarving( s._daysStarving )
, _isCopy(true)
, _HRActionSectors(s._HRActionSectors)
, _LRActionSectors(s._LRActionSectors)
, _HRCellPool(s._HRCellPool)
, _LRCellPool(s._LRCellPool)
, _objectUseCounter(s._objectUseCounter)
, _mapLock(s._mapLock)
, _agentRef(s._agentRef)
,_config(s._config)
,_numAvailableActionsWhenBorn(s._availableActions.size())
//,_availableActions(s._availableActions)
{
	std::stringstream logName;
	logName << "logMDPStates_"	<< _agentRef->getWorld()->getId() << "_" << _agentRef->getId();
	
	_dni=dniTicket ();
	
	_creator=1;
	_constructors.push_back(_creator);

	_info = 1;
	
	_ownItems.resize(s._ownItems.size());
	for(unsigned int i = 0; i < _ownItems.size(); i++)
	{
		//_ownItems[i] = false;
		_ownItems[i] = s._ownItems[i];
	}
	
	for ( unsigned k = 0; k < _availableActions.size(); k++ )
	{
		delete _availableActions[k];
	}	
	_availableActions.clear();
	for ( unsigned k = 0; k < s._availableActions.size(); k++ )
	{
		// avoiding segm fault through copy
		addAction( s._availableActions[k]->copy() );
		//addAction( s._availableActions[k] );
	}
	assert( s._availableActions.size() == _availableActions.size() );
	
	registerKnowledgeStructuresAtCounterMap();
	
	/*std::cout << "NET: edge "	
		<< s._dni 
		<< " "
		<< _dni 
		<< " a"
		<< std::endl;
	*/
	//std::cout << "herencia1:" << _dni << " at " <<  _mapLocation << " receives " << _LRActionSectors->size() << " sectors"<< std::endl;
	
	
	//log_INFO(logName.str(),"XXXX CREA 1:" << s._dni << "->" << _dni);
	//log_INFO(logName.str(),"herencia1:" << _dni << " at " <<  _mapLocation << " receives " << _LRActionSectors->size() << " sectors");
	
	
	assert(_mapLocation == s._mapLocation);
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
, _isCopy(true)
, _HRActionSectors(HRActionSectors)
, _LRActionSectors(LRActionSectors)
, _HRCellPool(HRCellPool)
, _LRCellPool(LRCellPool)
, _objectUseCounter(s._objectUseCounter)
, _mapLock(s._mapLock)
, _agentRef(s._agentRef)
, _config(s._config)
, _availableActions(actionList)
, _numAvailableActionsWhenBorn(actionList.size())
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
	
	/*std::cout << "NET: edge "	
		<< s._dni 
		<< " "
		<< _dni 
		<< " b"
		<< std::endl;
*/
	//std::cout << "herencia2:" << _dni << " at " <<  _mapLocation << " receives " << _LRActionSectors->size() << " sectors"<< std::endl;

	
	//log_INFO(logName.str(),"XXXX CREA 2:" << s._dni << "->" << _dni);
	_creator=2;
	_constructors.push_back(_creator);

	_info=2;
	//log_INFO(logName.str(),"herencia2:" << _dni << " at " <<  _mapLocation << " receives " << _LRActionSectors->size() << " sectors");
		
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
	, _isCopy(false)
	, _objectUseCounter(objectUseCounter)
	, _mapLock(mapLock)
	, _agentRef(agentRef)
	,_config(config)
	,_availableActions(actionList)
	,_numAvailableActionsWhenBorn(actionList.size())
{
	std::stringstream logName;
	logName << "logMDPStates_"	<< _agentRef->getWorld()->getId() << "_" << _agentRef->getId();
	

	_dni=dniTicket ();
	
	_creator=3;	
	_constructors.push_back(_creator);

	_info=3;

	_ownItems.resize(ownItems.size());
	for(unsigned int i = 0; i < ownItems.size(); i++)
	{
		_ownItems[i] = ownItems[i];
	}
	
	computeHash();	
	registerKnowledgeStructuresAtCounterMap();
	
	//std::cout << "herencia3:" << _dni << " at " <<  _mapLocation << " receives " << _LRActionSectors->size() << " sectors"<< std::endl;

	
	//log_INFO(logName.str(),"XXXX CREA 3:" << _dni);
	//log_INFO(logName.str(),"herencia3:" << _dni << " at " <<  _mapLocation << " receives " << _LRActionSectors->size() << " sectors");
	
}



const HunterGathererMDPState& HunterGathererMDPState::operator=( const HunterGathererMDPState& s )
{	
	std::stringstream logName;
	logName << "logMDPStates_"	<< _agentRef->getWorld()->getId() << "_" << _agentRef->getId();
	
	
	
	/*std::cout << "NET: edge "	
		<< s._dni 
		<< " "
		<< _dni 
		<< " (="
		<< std::endl;
	*/
	
//#ifdef REDUCC	
	//return s;
//#endif
		
	deRegisterFromCounterMapAndDeleteKnowledgeStructures();
	
	_dni=dniTicket ();
	
	/*std::cout << "NET: edge "	
		<< s._dni 
		<< " "
		<< _dni 
		<< " =)"
		<< std::endl;
	*/
	
	_creator=4;
	_constructors.push_back(_creator);

	_info=4;
	
	_timeIndex 		 = s._timeIndex;
	_mapLocation 	 = s._mapLocation;
	_onHandResources = s._onHandResources;
	_resources 		 = s._resources;
	_hashKey 		 = s._hashKey;
	_maxResources 	 = s._maxResources;
	_resourcesDivider = s._resourcesDivider;
	_daysStarving 	 = s._daysStarving;
	_isCopy 		 = true;	

	_objectUseCounter = s._objectUseCounter;
	_mapLock = s._mapLock;
	
	
//	deRegisterFromCounterMapAndDeleteKnowledgeStructures();
	
	_HRActionSectors = s._HRActionSectors;
	_LRActionSectors = s._LRActionSectors;
	_HRCellPool = s._HRCellPool;
	_LRCellPool = s._LRCellPool;


	_ownItems.resize(s._ownItems.size());
	for(unsigned int i = 0; i < _ownItems.size(); i++)
	{
	//	_ownItems[i] = false;
		_ownItems[i] = s._ownItems[i];
	}
	
	for ( unsigned k = 0; k < _availableActions.size(); k++ )
	{
		delete _availableActions[k];
	}
	_availableActions.clear();
	_numAvailableActionsWhenBorn = s._availableActions.size();
	for ( unsigned k = 0; k < s._availableActions.size(); k++ )
	{
		addAction( s._availableActions[k]->copy() );
	}
	assert( s._availableActions.size() == _availableActions.size() );
	
	registerKnowledgeStructuresAtCounterMap();	
	
	_agentRef = s._agentRef;
	
	_config = s._config;
	
	//std::cout << "herencia4:" << _dni << " at " <<  _mapLocation << " receives " << _LRActionSectors->size() << " sectors"<< std::endl;
	
	//log_INFO(logName.str(),"XXXX CREA 4:" << s._dni << "->" << _dni);	
	//log_INFO(logName.str(), "herencia4:" << _dni << " at " <<  _mapLocation << " receives " << _LRActionSectors->size() << " sectors");	
	
	return *this;
	
}

	

HunterGathererMDPState::~HunterGathererMDPState()
{
	std::stringstream logName;
	// look out: if 'this' is an _initial of a HGMDPModel && step==361 && _agentRef killed at 360, then the next line produces segm fault :
	//logName << "logMDPStates_"	<< _agentRef->getWorld()->getId() << "_" << _agentRef->getId();
	
	
	//std::cout << "destroying MDPState:" << _dni << std::endl;
	
	/*log_INFO(logName.str(),"DELETE "<< _dni 
					<<" , CREATED WITH:" << _creator 
					<< " , owns:" 	<< _ownItems[0]
									<< _ownItems[1]
									<< _ownItems[2]
									<< _ownItems[3]);	
	*/


	for ( unsigned int k = 0; k < _availableActions.size(); k++ )
	{
		delete _availableActions[k];
	}
	
	
	deRegisterFromCounterMapAndDeleteKnowledgeStructures();
	
}

void	HunterGathererMDPState::addAction( MDPAction* a )
{
	if(_availableActions.size() >= (unsigned int)_numAvailableActionsWhenBorn)
		std::cout << "TOO MUCH actions, created with :" << _creator << std::endl;
	assert(_availableActions.size() < (unsigned int)_numAvailableActionsWhenBorn);
	_availableActions.push_back(a);
}


#ifdef REDUCC
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
#endif


#ifndef REDUCC
void	HunterGathererMDPState::computeHash()
{
	_hashKey.add( _timeIndex );
	_hashKey.add( _mapLocation._x );
	_hashKey.add( _mapLocation._y );
	_hashKey.add( _onHandResources );
	_hashKey.add( _daysStarving );

	for ( Engine::IncrementalRaster::ChangeIterator it = _resources.firstChange();
		it != _resources.endOfChanges(); it++ )
	{
		const Engine::Point2D<int>& p = it->first;
		_hashKey.add( p._x );
		_hashKey.add( p._y );
		_hashKey.add( it->second );
	}
	
}
#endif



unsigned HunterGathererMDPState::hash() const
{
	return _hashKey.code(); 
}



bool HunterGathererMDPState::EqListMatching(const std::vector<Gujarat::MDPAction*> &v, const std::vector<Gujarat::MDPAction*> &w) const
{
	if (v.size() != w.size())
	{
		return false;
	}
	
	std::vector<bool> matched(v.size(),false);
	
	for(unsigned int i=0; i < v.size(); i++)
	{	
		unsigned int j;
		for(j=0; j < w.size(); j++)
		{			
			if(v[i]->equal(w[j],*_agentRef) && !matched[j])
			{
				matched[j] = true;
				break;
			}
		}
		if (j == w.size())
		{
			return false;
		}		
	}
	
	return true;	
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



#ifdef REDUCC
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
		//&&  
			//EqListMatching(_availableActions,s._availableActions);
			//_availableActions.size()==s._availableActions.size();
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
	
	/*
	if(result) 
	{
		std::cout << "REDUCC:" << _dni << "==" << s._dni << std::endl; 
		std::cout << "NET: edge "
		<< s._dni 
		<< " "
		<< _dni
		<< " =="
		<< std::endl;
	}
	else 
	{
		std::cout << "REDUCC:" << _dni << "!=" << s._dni << std::endl;
	}*/
	
	return result;
}
#endif

#ifndef REDUCC
bool	HunterGathererMDPState::operator==( const HunterGathererMDPState& s ) const
{
	// due to the term condition ( _resources == s._resources ) 			
	// we need introduce the "reductionResourcesToCategory" to comparison

	bool result =  ( _timeIndex == s._timeIndex ) 
		&&
			( _onHandResources == s._onHandResources ) 
		&&
			( _mapLocation == s._mapLocation ) 
		&&	
			_resources == s._resources
		&&
			( _daysStarving == s._daysStarving )
		&&  
			EqListMatching(_availableActions,s._availableActions); 

			
	/*if(result)
	{ 
		std::cout << "FULL:" << _dni << "==" << s._dni << std::endl; 
		std::cout << "NET: edge "
	
		<< s._dni 
		<< " "
	
		<< _dni
		<< " =="
		<< std::endl;
	}
	else std::cout << "FULL:" << _dni << "!=" << s._dni << std::endl;
	*/
	
	
	return result;

			
}
#endif




bool	HunterGathererMDPState::operator!=( const HunterGathererMDPState& s ) const
{
	
	return !(*this == s);
	/*return ( _timeIndex != s._timeIndex ) 				OR
			( _onHandResources != s._onHandResources ) 	OR
			( _mapLocation != s._mapLocation ) 			OR
			( _resources != s._resources ) 				OR
			( _daysStarving != s._daysStarving );*/
}

#ifdef REDUCC

bool	HunterGathererMDPState::operator<( const HunterGathererMDPState& s ) const
{		
	
	int simplOnHandResources = _agentRef->reductionResourcesToCategory(_onHandResources);
	int simple_s_onHandResources = _agentRef->reductionResourcesToCategory(s._onHandResources);	
	
	GujaratWorld * gw = (GujaratWorld*)_agentRef->getWorld();	
	Engine::Point2D<int> mapLocationLR;
	gw->worldCell2LowResCell(_mapLocation,mapLocationLR);
	Engine::Point2D<int> sMapLocationLR;
	gw->worldCell2LowResCell(s._mapLocation,sMapLocationLR);
	
	
	
	
	std::cout << "evaluating between " << this << " and: " << &s << std::endl;
	return  ( _timeIndex < s._timeIndex ) 
			||
			( ( _timeIndex == s._timeIndex ) && ( simplOnHandResources < simple_s_onHandResources ) ) 
			||
			( ( _timeIndex == s._timeIndex ) && ( simplOnHandResources == simple_s_onHandResources) && ( mapLocationLR < sMapLocationLR) ) 
			||
			( ( _timeIndex == s._timeIndex ) && ( simplOnHandResources == simple_s_onHandResources) && ( mapLocationLR == sMapLocationLR) && ( _resources < s._resources ) ) 
			||
			( ( _timeIndex == s._timeIndex ) && ( simplOnHandResources == simple_s_onHandResources) && ( mapLocationLR == sMapLocationLR) 
			&& ( equalIncRastersWithReduct(s._resources) ) &&  ( _daysStarving < s._daysStarving )	);
}

#endif


#ifndef REDUCC

bool	HunterGathererMDPState::operator<( const HunterGathererMDPState& s ) const
{		
	
	int simplOnHandResources = _onHandResources;
	int simple_s_onHandResources = s._onHandResources;	
	
	
	std::cout << "evaluating between " << this << " and: " << &s << std::endl;
	return  ( _timeIndex < s._timeIndex ) 
			||
			( ( _timeIndex == s._timeIndex ) && ( simplOnHandResources < simple_s_onHandResources ) ) 
			||
			( ( _timeIndex == s._timeIndex ) && ( simplOnHandResources == simple_s_onHandResources) && ( _mapLocation < s._mapLocation) ) 
			||
			( ( _timeIndex == s._timeIndex ) && ( simplOnHandResources == simple_s_onHandResources) && ( _mapLocation == s._mapLocation) && ( _resources < s._resources ) ) 
			||
			( ( _timeIndex == s._timeIndex ) && ( simplOnHandResources == simple_s_onHandResources) && ( _mapLocation == s._mapLocation) 
			&& ( _resources == s._resources ) &&  ( _daysStarving < s._daysStarving )	);
}

#endif


void	HunterGathererMDPState::print( unsigned long x ) const
{
	HunterGathererMDPState * p = (HunterGathererMDPState *)x;
	std::cout << *p << std::endl;
}




void	HunterGathererMDPState::print( std::ostream& os ) const
{
	os << "<addr = " << this << ", ";
	os << "dni = " << _dni << ", ";
	os << "constructor = " << _creator << ", ";
	os << "numActions = " << _numAvailableActionsWhenBorn << ", ";
	os << "loc=(" << _mapLocation._x << ", " << _mapLocation._y << "), ";
	os << "res=" << _onHandResources << ", ";
	os << "t=" << _timeIndex << ", ";
	os << "starv=" << _daysStarving << ", ";
	os << "actions:";
	for(unsigned int i=0;i<_availableActions.size();i++)
		os << ": " << _availableActions[i]<<" ";
	
	
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
	os << "A(s)=" << _availableActions.size() << ", ";
	os << "copy?=" << ( _isCopy ? "yes" : "no" ) <<  ">" << std::endl;	
}



void HunterGathererMDPState::registerKnowledgeStructuresAtCounterMap()
	{		
		//std::cout << "SIZE OF MAP " << _objectUseCounter->size() << std::endl;
		
		//static std::map<long,long> _objectUseCounter;

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

