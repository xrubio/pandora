
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
//	std::cout << "setup model at horizon: " << horizon << std::endl;
	_horizon = horizon;
}

void MDPAgentModel::reset( MDPAgent & agent )
{
//	std::cout << "reset size: " << agent.getWorld()->getDynamicRaster(eResources).getSize() << std::endl;
//	std::cout << "reset model for agent: " << agent << std::endl;
	if(_initial)
	{
		delete _initial;
		_initial = 0;
	}
	_agent= &agent;
	_initial = new MDPAgentState(agent.getPosition(), agent.getResources(), agent.getWorld()->getDynamicRaster(eResources), _horizon, agent.getNeededResources());
	makeActionsForState(*_initial);
//	std::cout << "end reset model for agent: " << agent << std::endl;
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
	float futureValue = state.getResources() + state.getAvailableAction(action).getResourcesToCollect();
	float cost = 10000000.0f;
	if(futureValue>0.0f)
	{
		cost = 1.0f/futureValue;
		cost *= 100;
	}
	//std::cout << "future value: " << futureValue << " cost: " << cost << " for state: " << state << " and action: " << action << " move to pos: " << state.getAvailableAction(action).getNewPosition() << std::endl;
	return cost;
	/*
	float cost = 0.0f;
	if(state.getResources()<0)
	{
		cost = 1000.0f;
	}
	else
	{
		cost = state.getTimeStep()+1;
	}
	std::cout << "cost for state: " << state << " and action: " << action << " is: " << cost << std::endl;
	return cost;
	*/
}

void MDPAgentModel::next( const MDPAgentState & state, action_t index, OutcomeVector & outcomes ) const
{
//	std::cout << "creating next state" << std::endl;
	MDPAgentState stateNext;
	state.initializeSuccessor(stateNext);
	const MoveAction & moveAction = state.getAvailableAction(index);
	moveAction.executeMDP( *_agent, state, stateNext );
	applyFrameEffects(state, stateNext);
	stateNext.computeHash();	
	makeActionsForState(stateNext);
	outcomes.push_back(std::make_pair(stateNext, 1.0));
//	std::cout << "end creating next state" << std::endl;
}

void MDPAgentModel::applyFrameEffects( const MDPAgentState & state,  MDPAgentState & stateNext ) const
{
	stateNext.eat();
	stateNext.increaseTimeStep();
}

void MDPAgentModel::makeActionsForState( MDPAgentState & state ) const
{
//	std::cout << "making actions for state : " << &state << " at pos: " << state << std::endl;
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
	//			std::cout << "from position: " << state.getPosition() << " discarding position: " << newPosition << std::endl;
				continue;
			}
	//		std::cout << "from position: " << state.getPosition() << " creating new at: " << newPosition << std::endl;
			MoveAction * newAction = new MoveAction(newPosition, state.getRasterResources().getValue(newPosition));
	//		std::cout << "creating new action: " << newAction << std::endl;
			state.addAction(newAction);
		}
	}
	state.randomizeActions();

	for(int i=0; i<state.getNumAvailableActions(); i++)
	{
		const MoveAction & action = state.getAvailableAction(i);
//		std::cout << "from position: " << state.getPosition() << " index: " << i << " move to: " << action.getNewPosition() << " resourcesToCollcet: " << action.getResourcesToCollect() << std::endl;

	}
	assert(state.getNumAvailableActions()>0);
//	std::cout << "end making actions for state: " << state << std::endl;
}

} // namespace Examples

