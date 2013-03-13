
#include "HerderState.hxx"
#include "MoveAction.hxx"

namespace GujaratCellphones
{

HerderState::HerderState(const Engine::Point2D<int> & position, const Engine::Point2D<int> & villagePosition,  int resources, const Engine::Raster & knowledgeMap, const Engine::Raster & resourcesMap, int resourcesToEat, int timeStep) : _timeStep(timeStep), _position(position), _villagePosition(villagePosition), _resources(resources), _starvationDays(0.0f), _knowledgeMap(knowledgeMap), _resourcesMap(resourcesMap), _hashKey(0), _resourcesToEat(resourcesToEat), _isCopy(false)
{
//	std::cout << "creating state by params: " << _rasterResources.getSize() << " original: " << resourcesRaster.getSize() << std::endl;
	computeHash();
}

HerderState::HerderState::HerderState( const HerderState & state ) : _timeStep(state._timeStep), _position(state._position), _villagePosition(state._villagePosition), _resources(state._resources), _starvationDays(state._starvationDays), _knowledgeMap(state._knowledgeMap), _resourcesMap(state._resourcesMap), _hashKey(0), _resourcesToEat(state._resourcesToEat), _isCopy(true)
{
//	std::cout << "creating state by copy with size: " << _rasterResources.getSize() << " from: " << state._rasterResources.getSize() << std::endl;
	for (int i=0; i<state.getNumAvailableActions(); i++)
	{
		addAction(state.getAvailableAction(i).copy());
	}
	computeHash();
	assert(state.getNumAvailableActions()==getNumAvailableActions());
}

HerderState::~HerderState()
{
	clearActions();
	if(_hashKey)
	{
		delete _hashKey;
	}
}

unsigned int HerderState::hash() const
{
	if(!_hashKey)
	{
		return 0;
	}
	return _hashKey->code();
}

bool HerderState::operator==( const HerderState & state ) const
{
	return (_timeStep == state._timeStep) && (_resourcesMap==state._resourcesMap) &&  (_knowledgeMap==state._knowledgeMap) && (_starvationDays==state._starvationDays) && (_position==state._position) && (_villagePosition==state._villagePosition) && (_resources==state._resources) && (_resourcesToEat==state._resourcesToEat);
}

const HerderState & HerderState::operator=(const HerderState & state )
{
	_timeStep = state._timeStep;
	_position = state._position;
	_villagePosition = state._villagePosition;
	_resources = state._resources;
	_starvationDays = state._starvationDays;
	_resourcesMap = state._resourcesMap;
	_knowledgeMap = state._knowledgeMap;
	_resourcesToEat = state._resourcesToEat;
	_isCopy = true;
	
	for(unsigned i=0; i<_availableActions.size(); i++)
	{
		delete _availableActions.at(i);
	}
	_availableActions.clear();

	for(unsigned i=0; i<state._availableActions.size(); i++ )
	{
		addAction(state._availableActions.at(i)->copy());
	}
	assert( state._availableActions.size() == _availableActions.size() );

	computeHash();
	return *this;
}

void HerderState::increaseTimeStep()
{
	_timeStep--;
}

int HerderState::getTimeStep() const
{
	return _timeStep;
}

int HerderState::getNumAvailableActions() const
{
	return _availableActions.size();
}

void HerderState::addAction( MoveAction * action )
{
	_availableActions.push_back(action);
}

const MoveAction & HerderState::getAvailableAction(Problem::action_t index) const
{
	return *_availableActions.at(index);
}

void HerderState::eat()
{
	_starvationDays += float(_resources)/float(_resourcesToEat);
	_resources = 0;
}

void HerderState::computeHash()
{
	if(_hashKey)
	{
		delete _hashKey;
	}
	_hashKey = new Engine::HashKey();
	int foo = hash();
	_hashKey->add(_timeStep);
	_hashKey->add(_position._x);
	_hashKey->add(_position._y);
	_hashKey->add(_villagePosition._x);
	_hashKey->add(_villagePosition._y);
	_hashKey->add(_resources);
	_hashKey->add(_starvationDays);
	_hashKey->add(_resourcesToEat);
	
	for ( Engine::IncrementalRaster::ChangeIterator it = _resourcesMap.firstChange(); it != _resourcesMap.endOfChanges(); it++ )
	{
		const Engine::Point2D<int>& p = it->first;
		_hashKey->add( p._x );
		_hashKey->add( p._y );
		_hashKey->add( it->second );
	}
	for ( Engine::IncrementalRaster::ChangeIterator it = _knowledgeMap.firstChange(); it != _knowledgeMap.endOfChanges(); it++ )
	{
		const Engine::Point2D<int>& p = it->first;
		_hashKey->add( p._x );
		_hashKey->add( p._y );
		_hashKey->add( it->second );
	}

}
	
const Engine::Point2D<int> & HerderState::getPosition() const
{
	return _position;
}

const Engine::Point2D<int> & HerderState::getVillagePosition() const
{
	return _villagePosition;
}
	
void HerderState::setPosition( const Engine::Point2D<int> & position )
{
	_position = position;
}

int HerderState::getResources() const
{
	return _resources;
}

int HerderState::getResourcesToEat() const
{
	return _resourcesToEat;
}

void HerderState::setResources( int resources )
{
	_resources = resources;
}

Engine::IncrementalRaster & HerderState::getResourcesMap()
{
	return _resourcesMap;
}

const Engine::IncrementalRaster & HerderState::getResourcesMap() const
{
	return _resourcesMap;
}

const Engine::IncrementalRaster & HerderState::getKnowledgeMap() const
{
	return _knowledgeMap;
}


void HerderState::randomizeActions()
{
	std::random_shuffle(_availableActions.begin(), _availableActions.end());
}

float HerderState::getStarvationDays() const
{
	return _starvationDays;
}

std::ostream & operator<<( std::ostream & stream, HerderState & state )
{
	return stream << " State - position: " << state._position << " resources: " << state._resources << " starvation: " << state._starvationDays << " time step: " << state._timeStep << " num. available actions " << state.getNumAvailableActions();
}

std::ostream & operator<<( std::ostream & stream, const HerderState & state )
{
	return stream << " State - position: " << state._position << " resources: " << state._resources << " starvation: " << state._starvationDays << " time step: " << state._timeStep << " num. available actions " << state.getNumAvailableActions();
}

void HerderState::clearActions()
{
	for (unsigned i=0; i<_availableActions.size(); i++)
	{
		delete _availableActions.at(i);
	}
	_availableActions.clear();
}

} // namespace GujaratCellphones

