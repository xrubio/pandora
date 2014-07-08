
#include "ForagerState.hxx"
#include "MoveAction.hxx"
#include "ForageAction.hxx"

namespace QuantumExperiment
{

ForagerState::ForagerState(const Engine::Point2D<int> & position, int neededResources, const Engine::DynamicRaster& knowledgeMap, const Engine::DynamicRaster& resourcesMap, int timeStep) : _timeStep(timeStep), _position(position), _foragedResources(0), _starvation(0.0f), _knowledgeMap(knowledgeMap), _resourcesMap(resourcesMap), _hashKey(0), _neededResources(neededResources), _isCopy(false)
{
//	std::cout << "creating state by params: " << _rasterResources.getSize() << " original: " << resourcesRaster.getSize() << std::endl;
	computeHash();
}

ForagerState::ForagerState( const ForagerState & state ) : _timeStep(state._timeStep), _position(state._position), _foragedResources(state._foragedResources), _starvation(state._starvation), _knowledgeMap(state._knowledgeMap), _resourcesMap(state._resourcesMap), _hashKey(0), _neededResources(state._neededResources), _isCopy(true)
{
//	std::cout << "creating state by copy with size: " << _rasterResources.getSize() << " from: " << state._rasterResources.getSize() << std::endl;
	for (int i=0; i<state.getNumAvailableActions(); i++)
	{
		addAction(state.getAvailableAction(i).copy());
	}
	computeHash();
	assert(state.getNumAvailableActions()==getNumAvailableActions());
	assert(state.hash()==hash());
}

ForagerState::~ForagerState()
{
	clearActions();
	if(_hashKey)
	{
		delete _hashKey;
	}
}

unsigned int ForagerState::hash() const
{
	if(!_hashKey)
	{
		return 0;
	}
	return _hashKey->code();
}

bool ForagerState::operator==( const ForagerState & state ) const
{
	return (hash()==state.hash());
}

const ForagerState & ForagerState::operator=(const ForagerState & state )
{
	_timeStep = state._timeStep;
	_position = state._position;
	_foragedResources = state._foragedResources;
	_starvation = state._starvation;
	
	_knowledgeMap = state._knowledgeMap;
	_resourcesMap = state._resourcesMap;

	_neededResources = state._neededResources;
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
	computeHash();
	assert( state._availableActions.size() == _availableActions.size() );
	assert( state.hash()==hash());
	return *this;
}

void ForagerState::increaseTimeStep()
{
	_timeStep++;
}

int ForagerState::getTimeStep() const
{
	return _timeStep;
}

int ForagerState::getNumAvailableActions() const
{
	return _availableActions.size();
}

void ForagerState::addAction( BaseAction * action )
{
	_availableActions.push_back(action);
}

const BaseAction & ForagerState::getAvailableAction(Problem::action_t index) const
{
	return *_availableActions.at(index);
}

void ForagerState::consume()
{
	_starvation += (1.0f - float(_foragedResources)/float(_neededResources));
}

void ForagerState::computeHash()
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
	_hashKey->add(_foragedResources);
	_hashKey->add(_starvation);
	_hashKey->add(_neededResources);
	
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
	
const Engine::Point2D<int> & ForagerState::getPosition() const
{
	return _position;
}

void ForagerState::setPosition( const Engine::Point2D<int> & position )
{
	_position = position;
}

int ForagerState::getNeededResources() const
{
	return _neededResources;
}

Engine::IncrementalRaster & ForagerState::getResourcesMap()
{
	return _resourcesMap;
}

const Engine::IncrementalRaster & ForagerState::getResourcesMap() const
{
	return _resourcesMap;
}

const Engine::IncrementalRaster & ForagerState::getKnowledgeMap() const
{
	return _knowledgeMap;
}

Engine::IncrementalRaster & ForagerState::getKnowledgeMap()
{
	return _knowledgeMap;
}

void ForagerState::randomizeActions()
{
	std::random_shuffle(_availableActions.begin(), _availableActions.end());
}

std::ostream & operator<<( std::ostream & stream, ForagerState & state )
{
	return stream << " State - position: " << state._position << " foraged resources: " << state._foragedResources << " needed resources: " << state._neededResources  << " starvation: " << state._starvation << " time step: " << state._timeStep << " num. available actions " << state.getNumAvailableActions();
}

std::ostream & operator<<( std::ostream & stream, const ForagerState & state )
{
	return stream << " State - position: " << state._position << " foraged resources: " << state._foragedResources << " needed resources: " << state._neededResources  << " starvation: " << state._starvation << " time step: " << state._timeStep << " num. available actions " << state.getNumAvailableActions();
}

void ForagerState::clearActions()
{
	for (unsigned i=0; i<_availableActions.size(); i++)
	{
		delete _availableActions.at(i);
	}
	_availableActions.clear();
}

void ForagerState::setForagedResources( int foragedResources )
{
	_foragedResources = foragedResources;
}
	
int ForagerState::getForagedResources() const
{
	return _foragedResources;
}

} // namespace QuantumExperiment

