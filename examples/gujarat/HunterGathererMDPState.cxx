#include <HunterGathererMDPState.hxx>

namespace Gujarat
{

HunterGathererMDPState::HunterGathererMDPState() : _timeIndex(0), _mapLocation(-1,-1), _onHandResources(0), _maxResources(0), _resourcesDivider(1), _daysStarving(0), _isCopy( false)
{
}

HunterGathererMDPState::HunterGathererMDPState(Engine::Point2D<int> loc, int initResources, const Engine::DynamicRaster& resourcesRaster, int maxResources, int divider )
	: _timeIndex(0), _mapLocation( loc ), _onHandResources( initResources ), _resources( resourcesRaster ), _maxResources( maxResources ), _resourcesDivider( divider ), _daysStarving( 0 ), _isCopy(false)
{
	computeHash();
}

HunterGathererMDPState::HunterGathererMDPState( const HunterGathererMDPState& s )
	: _timeIndex( s._timeIndex ), _mapLocation( s._mapLocation ), _onHandResources( s._onHandResources ),
	_resources( s._resources ), _hashKey( s._hashKey ), _maxResources( s._maxResources), 
	_resourcesDivider( s._resourcesDivider ), _daysStarving( s._daysStarving ), _isCopy(true)
{
	for ( unsigned k = 0; k < s._availableActions.size(); k++ )
	{
		addAction( s._availableActions[k]->copy() );
	}
	assert( s._availableActions.size() == _availableActions.size() );
}

const HunterGathererMDPState& HunterGathererMDPState::operator=( const HunterGathererMDPState& s )
{
	_timeIndex = s._timeIndex;
	_mapLocation = s._mapLocation;
	_onHandResources = s._onHandResources;
	_resources = s._resources;
	_hashKey = s._hashKey;
	_maxResources = s._maxResources;
	_resourcesDivider = s._resourcesDivider;
	_daysStarving = s._daysStarving;
	_isCopy = true;	

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
	return *this;
}

void	HunterGathererMDPState::initializeSuccessor( HunterGathererMDPState& s ) const
{
	s._timeIndex = _timeIndex;
	s._mapLocation = _mapLocation;
	s._onHandResources = _onHandResources;
	s._maxResources = _maxResources;
	s._resources = _resources;
	s._resourcesDivider = _resourcesDivider;
	s._daysStarving = _daysStarving;
}

HunterGathererMDPState::~HunterGathererMDPState()
{
	for ( unsigned k = 0; k < _availableActions.size(); k++ )
		delete _availableActions[k];
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


}
