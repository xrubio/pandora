
#include <HunterGathererMDPState.hxx>
#include <Logger.hxx>

#include <sstream>

#include <mutex>

#include <omp.h>

namespace Gujarat
{

/*
HunterGathererMDPState::HunterGathererMDPState() : _timeIndex(0), _mapLocation(-1,-1), _onHandResources(0), _maxResources(0), _resourcesDivider(1), _daysStarving(0), _isCopy( false)
, _HRActionSectors( HunterGathererMDPState::_emptySectorVector )
, _LRActionSectors( HunterGathererMDPState::_emptySectorVector )
, _HRCellPool( HunterGathererMDPState::_emptyCellPool )
, _LRCellPool( HunterGathererMDPState::_emptyCellPool )	
{
}*/

//boost::mutex _mtx;
std::map<long,long> HunterGathererMDPState::_objectUseCounter;

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
{
	std::stringstream logName;
	logName << "infoshar";
	
	_dni=dniTicket ();
	
	//log_INFO(logName.str(),"XXXX CREA 1:" << s._dni << "->" << _dni);
	_creator=1;
	
	_ownItems.resize(s._ownItems.size());
	for(int i = 0; i < _ownItems.size(); i++)
	{
		//_ownItems[i] = false;
		_ownItems[i] = s._ownItems[i];
	}
	
	for ( unsigned k = 0; k < s._availableActions.size(); k++ )
	{
		//*?
		addAction( s._availableActions[k]->copy() );
		//addAction( s._availableActions[k] );
	}
	assert( s._availableActions.size() == _availableActions.size() );
	
	registerKnowledgeStructuresAtCounterMap();
}


HunterGathererMDPState::HunterGathererMDPState( const HunterGathererMDPState& s
						, std::vector< Sector* > & HRActionSectors
						, std::vector< Sector* > & LRActionSectors
						, std::vector< Engine::Point2D<int> > & HRCellPool
						, std::vector< Engine::Point2D<int> > & LRCellPool
						, std::vector< bool > ownItems )
: _timeIndex( s._timeIndex )
, _mapLocation( s._mapLocation )
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

/*
, _HRActionSectors((ownership[0])?*(new std::vector<Sector*>):s._HRActionSectors)
, _LRActionSectors((ownership[1])?*(new std::vector<Sector*>):s._LRActionSectors)
, _HRCellPool((ownership[2])?*(new std::vector<Engine::Point2D<int>):s._HRCellPool)
, _LRCellPool((ownership[3])?*(new std::vector<Engine::Point2D<int>):s._LRCellPool)
*/
{
	std::stringstream logName;
	logName << "infoshar";	
		
	_dni=dniTicket ();
	
	//log_INFO(logName.str(),"XXXX CREA 2:" << s._dni << "->" << _dni);
	_creator=2;
	
	_ownItems.resize(ownItems.size());
	for(int i = 0; i < ownItems.size(); i++)
	{
		_ownItems[i] = ownItems[i];
	}
	
//*?
	/*
	for ( unsigned k = 0; k < s._availableActions.size(); k++ )
	{
		//*?
		addAction( s._availableActions[k]->copy() );
		//addAction( s._availableActions[k] );
	}
	assert( s._availableActions.size() == _availableActions.size() );
	*/
	registerKnowledgeStructuresAtCounterMap();
}



HunterGathererMDPState::HunterGathererMDPState( 
			const Engine::Point2D<int> loc
			, int initResources
			, Engine::Raster& resourcesRaster
			, int maxResources
			, int divider 
			, std::vector< Sector* > & HRActionSectors
			, std::vector< Sector* > & LRActionSectors
			, std::vector< Engine::Point2D<int> > & HRCellPool
			, std::vector< Engine::Point2D<int> > & LRCellPool
			, std::vector< bool > ownItems)

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
{
	std::stringstream logName;
	logName << "infoshar";	

	_dni=dniTicket ();
	
	//log_INFO(logName.str(),"XXXX CREA 3:" << _dni);
	_creator=3;

	_ownItems.resize(ownItems.size());
	for(int i = 0; i < ownItems.size(); i++)
	{
		_ownItems[i] = ownItems[i];
	}
	
	computeHash();	
	registerKnowledgeStructuresAtCounterMap();
}


const HunterGathererMDPState& HunterGathererMDPState::operator=( const HunterGathererMDPState& s )
{	
	deRegisterFromCounterMapAndDeleteKnowledgeStructures();
	
	std::stringstream logName;
	logName << "infoshar";	
	
	_dni=dniTicket ();
	
	//log_INFO(logName.str(),"XXXX CREA 4:" << s._dni << "->" << _dni);
	_creator=4;
	
	_timeIndex 		 = s._timeIndex;
	_mapLocation 	 = s._mapLocation;
	_onHandResources = s._onHandResources;
	_resources 		 = s._resources;
	_hashKey 		 = s._hashKey;
	_maxResources 	 = s._maxResources;
	_resourcesDivider = s._resourcesDivider;
	_daysStarving 	 = s._daysStarving;
	_isCopy 		 = true;	

	deRegisterFromCounterMapAndDeleteKnowledgeStructures();
	
	_HRActionSectors = s._HRActionSectors;
	_LRActionSectors = s._LRActionSectors;
	_HRCellPool = s._HRCellPool;
	_LRCellPool = s._LRCellPool;
	
	_ownItems.resize(s._ownItems.size());
	for(int i = 0; i < _ownItems.size(); i++)
	{
	//	_ownItems[i] = false;
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
	
	return *this;
}


/*
// HunterGathererMDPState has a bunch of ref to vector, they cannot be initialized
so HunterGathererMDPState::initializeSuccessor must be disabled.
void	HunterGathererMDPState::initializeSuccessor( HunterGathererMDPState& s,bool ownership[]) const
{
	s._timeIndex 		= _timeIndex;
	s._mapLocation 		= _mapLocation;
	s._onHandResources 	= _onHandResources;
	s._maxResources 	= _maxResources;
	s._resources 		= _resources;
	s._resourcesDivider = _resourcesDivider;
	s._daysStarving 	= _daysStarving;
	
	if(ownership[0])
	{
		s._LRActionSectors = dup _LRActionSectors;
	}
	else
	{
		s._LRActionSectors = _LRActionSectors;
	}
	
	if(ownership[1])
	{
		s._HRActionSectors = dup _HRActionSectors;
	}
	else
	{
		s._HRActionSectors = _HRActionSectors;
	}
	
	if(ownership[2])
	{
		s._HRCellPool = dup _HRCellPool;
	}
	else
	{
		s._HRCellPool = _HRCellPool;
	}
	if(ownership[3])
	{
		s._LRCellPool = dup _LRCellPool;
	}
	else
	{
		s._LRCellPool = _LRCellPool;
	}
	
	for(int i=0; i< _ownItems.size(); i++)
	{
		_ownItems[i] = ownership[i];
	}

}
*/


HunterGathererMDPState::~HunterGathererMDPState()
{
	std::stringstream logName;
	logName << "infoshar";	
	
	
	/*log_INFO(logName.str(),"DELETE "<< _dni 
					<<" , CREATED WITH:" << _creator 
					<< " , owns:" 	<< _ownItems[0]
									<< _ownItems[1]
									<< _ownItems[2]
									<< _ownItems[3]);	
	*/
	for ( int k = 0; k < _availableActions.size(); k++ )
	{
		delete _availableActions[k];
	}
	
	
	deRegisterFromCounterMapAndDeleteKnowledgeStructures();
	
	/*
	if (_ownItems[0])
	{
		log_INFO(logName.str(),"DELETE HRSectors");
		// stop before deleting referenced Points, this happens in the
		// part for Pool deletion if _ownItems allows so.
		for(int i=0;i<_HRActionSectors.size();i++)
		{
			delete _HRActionSectors[i];
		}
		delete &_HRActionSectors;
	}
	if (_ownItems[1])
	{
		log_INFO(logName.str(),"DELETE LRSectors");
		for(int i=0;i<_LRActionSectors.size();i++)
		{
			delete _LRActionSectors[i];
		}
		delete &_LRActionSectors;
	}
	if (_ownItems[2])
	{
		log_INFO(logName.str(),"DELETE HRPool");
		delete &_HRCellPool;
	}
	if (_ownItems[3])
	{
		log_INFO(logName.str(),"DELETE LRPool");
		delete &_LRCellPool;
	}
	*/
}

void	HunterGathererMDPState::addAction( MDPAction* a )
{
	_availableActions.push_back(a);
}

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

unsigned HunterGathererMDPState::hash() const
{
	return _hashKey.code(); 
}

bool	HunterGathererMDPState::operator==( const HunterGathererMDPState& s ) const
{
	return ( _timeIndex == s._timeIndex ) &&
			( _onHandResources == s._onHandResources ) &&
			( _mapLocation == s._mapLocation ) &&
			( _resources == s._resources ) &&
			( _daysStarving == s._daysStarving );
}

bool	HunterGathererMDPState::operator!=( const HunterGathererMDPState& s ) const
{
	//TODO why AND instead of OR???
	
	return ( _timeIndex != s._timeIndex ) &&
			( _onHandResources != s._onHandResources ) &&
			( _mapLocation != s._mapLocation ) &&
			( _resources != s._resources ) &&
			( _daysStarving != s._daysStarving );
}

bool	HunterGathererMDPState::operator<( const HunterGathererMDPState& s ) const
{
	std::cout << "evaluating between " << this << " and: " << &s << std::endl;
	return  ( _timeIndex < s._timeIndex ) ||
			( ( _timeIndex == s._timeIndex ) && ( _onHandResources < s._onHandResources ) ) ||
			( ( _timeIndex == s._timeIndex ) && ( _onHandResources == s._onHandResources) &&
			  ( _mapLocation < s._mapLocation) ) ||
			( ( _timeIndex == s._timeIndex ) && ( _onHandResources == s._onHandResources) &&
			  ( _mapLocation == s._mapLocation) && ( _resources < s._resources ) ) ||
			( ( _timeIndex == s._timeIndex ) && ( _onHandResources == s._onHandResources) &&
			  ( _mapLocation == s._mapLocation) && ( _resources == s._resources ) &&
			  ( _daysStarving < s._daysStarving )	);
}

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
	os << "A(s)=" << _availableActions.size() << ", ";
	os << "copy?=" << ( _isCopy ? "yes" : "no" ) <<  ">" << std::endl;	
}



void HunterGathererMDPState::registerKnowledgeStructuresAtCounterMap()
	{		
		//std::cout << "SIZE OF MAP " << HunterGathererMDPState::_objectUseCounter.size() << std::endl;
		
		//static std::map<long,long> _objectUseCounter;
		
		#pragma omp critical(refmap)
		{
			
		
		if (HunterGathererMDPState::_objectUseCounter.count((long)&_HRActionSectors) > 0)
		{
				HunterGathererMDPState::_objectUseCounter[(long)&_HRActionSectors]++;		
	
		}
		else
		{
				HunterGathererMDPState::_objectUseCounter[(long)&_HRActionSectors]=1;		
		}
		
		if (HunterGathererMDPState::_objectUseCounter.count((long)&_LRActionSectors) > 0)
		{
				HunterGathererMDPState::_objectUseCounter[(long)&_LRActionSectors]++;
		}
		else
		{
				HunterGathererMDPState::_objectUseCounter[(long)&_LRActionSectors]=1;
		}
		
		if (HunterGathererMDPState::_objectUseCounter.count((long)&_HRCellPool) > 0)
		{
				HunterGathererMDPState::_objectUseCounter[(long)&_HRCellPool]++;
		}
		else
		{
				HunterGathererMDPState::_objectUseCounter[(long)&_HRCellPool]=1;
		}
		
		if (HunterGathererMDPState::_objectUseCounter.count((long)&_LRCellPool) > 0)
		{
				HunterGathererMDPState::_objectUseCounter[(long)&_LRCellPool]++;
		}
		else
		{
				HunterGathererMDPState::_objectUseCounter[(long)&_LRCellPool]=1;
		}
	
		}//pragma critical
	
	}
	

void HunterGathererMDPState::deRegisterFromCounterMapAndDeleteKnowledgeStructures()
	{
		//static std::map<long,long> _objectUseCounter;
		
		//std::cout << "DELETING state " << _dni << std::endl;
		std::cout << "DELETING state. thread [" << omp_get_thread_num() <<"]"<< std::endl;
		#pragma omp critical(refmap)
		{
		
		if (HunterGathererMDPState::_objectUseCounter.count((long)&_HRActionSectors) > 1)
		{	
			
				HunterGathererMDPState::_objectUseCounter[(long)&_HRActionSectors]--;
		
		}
		else if (_ownItems[0])
		{
			HunterGathererMDPState::_objectUseCounter.erase((long)&_HRActionSectors);
		
			for(int i=0;i<_HRActionSectors.size();i++)
			{
				delete _HRActionSectors[i];
			}
			
			delete &_HRActionSectors;
		}
	//	}//pragma
	//	#pragma omp critical(refmap)
	//	{
			
		if (HunterGathererMDPState::_objectUseCounter.count((long)&_LRActionSectors) > 0)
		{
		
				HunterGathererMDPState::_objectUseCounter[(long)&_LRActionSectors]--;
		
		}
		else if (_ownItems[1])
		{
			HunterGathererMDPState::_objectUseCounter.erase ((long)&_LRActionSectors);				
		
			for(int i=0;i<_LRActionSectors.size();i++)
			{
				delete _LRActionSectors[i];
			}			
			delete &_LRActionSectors;
		}
		
	//	}//pragma
	//	#pragma omp critical(refmap)
	//	{
			
		if (HunterGathererMDPState::_objectUseCounter.count((long)&_HRCellPool) > 0)
		{
				HunterGathererMDPState::_objectUseCounter[(long)&_HRCellPool]--;
		
		}
		else if (_ownItems[2])
		{
				HunterGathererMDPState::_objectUseCounter.erase((long)&_HRCellPool);
		
			delete &_HRCellPool;
		}
		
	//	}//pragma
	//	#pragma omp critical(refmap)
	//	{
			
		if (HunterGathererMDPState::_objectUseCounter.count((long)&_LRCellPool) > 0)
		{
				HunterGathererMDPState::_objectUseCounter[(long)&_LRCellPool]--;
		
		}
		else if (_ownItems[3])
		{
				HunterGathererMDPState::_objectUseCounter.erase((long)&_LRCellPool);
		
			delete &_LRCellPool;
		}
		
		}//pragma critical
		
	}

	
	void HunterGathererMDPState::clearRefCounterMap() 
	{ 
		//#pragma omp critical(refmap){HunterGathererMDPState::_objectUseCounter.clear();}
	}
	
}

