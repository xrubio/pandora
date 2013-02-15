
#include "MDPAgentState.hxx"
#include "MoveAction.hxx"

namespace Examples
{

MDPAgentState::MDPAgentState() : _timeStep(0), _position(-1,-1), _resources(0), _horizon(0), _isCopy(false)
{
}

MDPAgentState::MDPAgentState(const Engine::Point2D<int> & position, int resources, const Engine::Raster& resourcesRaster, unsigned int horizon, int resourcesToEat) : _timeStep(0), _position(position), _resources(resources), _horizon(horizon), _resourcesToEat(resourcesToEat), _isCopy(false)
{
	computeHash();
}

MDPAgentState::MDPAgentState::MDPAgentState( const MDPAgentState & state )
{
	for (int i=0; i<state.getNumAvailableActions(); i++)
	{
		addAction(state.getAvailableAction(i).copy());
	}
	assert( state.getNumAvailableActions()==getNumAvailableActions());
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
/*

const MDPAgentState & MDPAgentState::operator=(const MDPAgentState & state )
{
}

bool MDPAgentState::operator!=( const MDPAgentState& state ) const
{
}

bool MDPAgentState::operator<( const MDPAgentState & state ) const
{
}
*/

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

const Engine::IncrementalRaster & MDPAgentState::getRasterResources() const
{
	return _rasterResources;
}

} // namespace Examples

