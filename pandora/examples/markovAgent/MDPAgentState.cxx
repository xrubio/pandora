
#include "MDPAgentState.hxx"
#include "MoveAction.hxx"

namespace Examples
{

MDPAgentState::MDPAgentState() : _timeStep(0), _position(-1,-1), _resources(0), _horizon(0), _isCopy(false)
{
}

MDPAgentState::MDPAgentState(const Engine::Point2D<int> & position, int resources, const Engine::Raster & resourcesRaster, unsigned int horizon, int resourcesToEat) : _timeStep(0), _position(position), _resources(resources), _rasterResources(resourcesRaster), _horizon(horizon), _resourcesToEat(resourcesToEat), _isCopy(false)
{
//	std::cout << "creating state by params: " << _rasterResources.getSize() << " original: " << resourcesRaster.getSize() << std::endl;
	computeHash();
}

MDPAgentState::MDPAgentState::MDPAgentState( const MDPAgentState & state ) : _timeStep(state._timeStep), _position(state._position), _resources(state._resources), _rasterResources(state._rasterResources), _hashKey(state._hashKey), _horizon(state._horizon), _resourcesToEat(state._resourcesToEat), _isCopy(true)
{
//	std::cout << "creating state by copy with size: " << _rasterResources.getSize() << " from: " << state._rasterResources.getSize() << std::endl;
	for (int i=0; i<state.getNumAvailableActions(); i++)
	{
		addAction(state.getAvailableAction(i).copy());
	}
	assert(state.getNumAvailableActions()==getNumAvailableActions());
}

MDPAgentState::~MDPAgentState()
{
	for (unsigned i=0; i<_availableActions.size(); i++)
	{
		delete _availableActions.at(i);
	}
}

void MDPAgentState::initializeSuccessor( MDPAgentState & state ) const
{
	state._timeStep = _timeStep;
	state._position = _position;
	state._resources = _resources;
	state._rasterResources = _rasterResources;
	state._resourcesToEat = _resourcesToEat;
}

unsigned int MDPAgentState::hash() const
{
	return _hashKey.code();
}

bool MDPAgentState::operator==( const MDPAgentState & state ) const
{
	return (_timeStep == state.getTimeStep()) && (_resources==state.getResources()) && (_position==state.getPosition()) && (_rasterResources==state.getRasterResources());
}

const MDPAgentState & MDPAgentState::operator=(const MDPAgentState & state )
{
	_timeStep = state._timeStep;
	_position = state._position;
	_resources = state._resources;
	_hashKey = state._hashKey;
	_rasterResources = state._rasterResources;
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
	return *this;
}

bool MDPAgentState::operator!=( const MDPAgentState& state ) const
{
	if(_timeStep!=state._timeStep)
	{
		return true;
	}
	if(_resources!=state._resources)
	{
		return true;
	}
	if(_position!=state._position)
	{
		return true;
	}
	if(_horizon!=state._horizon)
	{
		return true;
	}
	return false;
}

bool MDPAgentState::operator<( const MDPAgentState & state ) const
{
	if(_timeStep<state._timeStep)
	{
	//	std::cout << "evaluation: " << *this << " is lesser than: " << state << " for time step" << std::endl;
		return true;
	}
	else if(_timeStep>state._timeStep)
	{
	//	std::cout <<  "evaluation: " << *this << " is bigger than: " << state << " for time step" << std::endl;
		return false;
	}
	if(_resources<state._resources)
	{
	//	std::cout <<  "evaluation: " << *this << " is lesser than: " << state << " for resources" << std::endl;
		return true;
	}
	else if(_resources>state._resources)
	{
	//	std::cout <<  "evaluation: " << *this << " is bigger than: " << state << " for resources" << std::endl;
		return false;
	}
	//std::cout <<  "evaluation: " << *this << " is equal than: " << state << std::endl;
	return false;

}

void MDPAgentState::increaseTimeStep()
{
	_timeStep++;
}

int MDPAgentState::getTimeStep() const
{
	return _timeStep;
}

int MDPAgentState::getNumAvailableActions() const
{
	return _availableActions.size();
}

void MDPAgentState::addAction( MoveAction * action )
{
	_availableActions.push_back(action);
}

const MoveAction & MDPAgentState::getAvailableAction(Problem::action_t index) const
{
	return *_availableActions.at(index);
}

void MDPAgentState::eat()
{
	_resources -= _resourcesToEat;
}

void MDPAgentState::computeHash()
{
	_hashKey.add(_timeStep);
	_hashKey.add(_position._x);
	_hashKey.add(_position._y);
	_hashKey.add(_resources);
	
	for ( Engine::IncrementalRaster::ChangeIterator it = _rasterResources.firstChange(); it != _rasterResources.endOfChanges(); it++ )
	{
		const Engine::Point2D<int>& p = it->first;
		_hashKey.add( p._x );
		_hashKey.add( p._y );
		_hashKey.add( it->second );
	}
}
	
const Engine::Point2D<int> & MDPAgentState::getPosition() const
{
	return _position;
}
	
void MDPAgentState::setPosition( const Engine::Point2D<int> & position )
{
	_position = position;
}

int MDPAgentState::getResources() const
{
	return _resources;
}

void MDPAgentState::setResources( int resources )
{
	_resources = resources;
}

const Engine::IncrementalRaster & MDPAgentState::getRasterResources() const
{
	return _rasterResources;
}

Engine::IncrementalRaster & MDPAgentState::getRasterResources()
{
	return _rasterResources;
}

void MDPAgentState::randomizeActions()
{
	std::random_shuffle(_availableActions.begin(), _availableActions.end());
}

std::ostream & operator<<( std::ostream & stream, MDPAgentState & state )
{
	return stream << " State - position: " << state._position << " resources: " << state._resources << " time step: " << state._timeStep << " num. available actions " << state.getNumAvailableActions();
}

std::ostream & operator<<( std::ostream & stream, const MDPAgentState & state )
{
	return stream << " State - position: " << state._position << " resources: " << state._resources << " time step: " << state._timeStep << " num. available actions " << state.getNumAvailableActions();
}

} // namespace Examples

