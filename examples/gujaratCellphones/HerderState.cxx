
#include "HerderState.hxx"
#include "MoveAction.hxx"

namespace GujaratCellphones
{

HerderState::HerderState(const Engine::Point2D<int> & position, const Engine::DynamicRaster & knowledgeMap, const Engine::DynamicRaster & resourcesMap, int resourcesToEat) : _position(position), _knowledgeMap(knowledgeMap), _resourcesMap(resourcesMap), _hashKey(0), _resourcesToEat(resourcesToEat), _isCopy(false)
{
	computeHash();
}

HerderState::HerderState::HerderState( const HerderState & state ) : _position(state._position), _knowledgeMap(state._knowledgeMap), _resourcesMap(state._resourcesMap), _hashKey(0), _resourcesToEat(state._resourcesToEat), _isCopy(true)
{
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
	return (_resourcesMap==state._resourcesMap) &&  (_knowledgeMap==state._knowledgeMap) && (_position==state._position) && (_resourcesToEat==state._resourcesToEat);
}

const HerderState & HerderState::operator=(const HerderState & state )
{
	_position = state._position;
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

void HerderState::computeHash()
{
	if(_hashKey)
	{
		delete _hashKey;
	}
	_hashKey = new Engine::HashKey();
	int foo = hash();
	_hashKey->add(_position._x);
	_hashKey->add(_position._y);
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

void HerderState::setPosition( const Engine::Point2D<int> & position )
{
	_position = position;
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

std::ostream & operator<<( std::ostream & stream, HerderState & state )
{
	return stream << " State - position: " << state._position << " num. available actions " << state.getNumAvailableActions();
}

std::ostream & operator<<( std::ostream & stream, const HerderState & state )
{
	return stream << " State - position: " << state._position << " num. available actions " << state.getNumAvailableActions();
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

