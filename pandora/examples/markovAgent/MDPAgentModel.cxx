
#include "MDPAgentModel.hxx"
#include "MDPAgent.hxx"
#include "MDPAgentState.hxx"
#include "RandomWorld.hxx"
#include "MoveAction.hxx"

#include <Exceptions.hxx>

using Problem::action_t;

namespace Examples
{

MDPAgentModel::MDPAgentModel() : _agent(0), _initial(0)
{
}

MDPAgentModel::~MDPAgentModel()
{
	if(_initial)
	{
		delete _initial;
	}
}

void MDPAgentModel::setup( const unsigned int & horizon )
{
	_horizon = horizon;
}

void MDPAgentModel::reset( MDPAgent & agent )
{
	if(_initial)
	{
		delete _initial;
		_initial = 0;
	}
	_agent= &agent;
	_initial = new MDPAgentState(agent.getPosition(), agent.getResources(), agent.getWorld()->getDynamicRaster(eResources), _horizon, 2);
	makeActionsForState(*_initial);
}

action_t MDPAgentModel::number_actions( const MDPAgentState & state ) const
{
	return state.getNumAvailableActions();
}

const MDPAgentState & MDPAgentModel::init() const
{
	return *_initial;
}

bool MDPAgentModel::terminal( const MDPAgentState & state ) const
{
	return state.getTimeStep() == _horizon;
}
	
bool MDPAgentModel::dead_end( const MDPAgentState & state ) const
{
	return false;
}

bool MDPAgentModel::applicable( const MDPAgentState & state, action_t action) const
{
	return true;
}

float MDPAgentModel::cost( const MDPAgentState & state, action_t action ) const
{
	// TODO
	return 1.0;
}

void MDPAgentModel::next( const MDPAgentState & state, action_t index, OutcomeVector & outcomes ) const
{
	MDPAgentState stateNext;
	state.initializeSuccessor(stateNext);
	const MoveAction & moveAction = state.getAvailableAction(index);
	moveAction.executeMDP( *_agent, state, stateNext );
	applyFrameEffects(state, stateNext);
	stateNext.computeHash();	
	makeActionsForState(stateNext);
	outcomes.push_back(std::make_pair(stateNext, 1.0));
}

void MDPAgentModel::applyFrameEffects( const MDPAgentState & state,  MDPAgentState & stateNext ) const
{
	stateNext.eat();
	stateNext.increaseTimeStep();
}

void MDPAgentModel::makeActionsForState( MDPAgentState & state ) const
{
	assert(state.getNumAvailableActions()==0);
	//is it necessary?
	//_agentSim->updateKnowledge(state.getPosition(), s.getResources());
	for(int i=state.getPosition()._x-1; i<=state.getPosition()._x+1; i++)
	{
		for(int j=state.getPosition()._y-1; j<=state.getPosition()._y+1; j++)
		{
			Engine::Point2D<int> newPosition(i,j);
			if(!_agent->getWorld()->checkPosition(newPosition))
			{
				continue;
			}
			state.addAction(new MoveAction(newPosition));
		}
	}
	assert(state.getNumAvailableActions()>0);
}

} // namespace Examples

