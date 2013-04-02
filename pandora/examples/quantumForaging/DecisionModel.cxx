
#include "DecisionModel.hxx"
#include "Forager.hxx"
#include "ForagerState.hxx"
#include "MoveAction.hxx"

#include <Exceptions.hxx>

using Problem::action_t;

namespace QuantumExperiment
{

DecisionModel::DecisionModel( Forager & agent ) : _agent(agent), _initial(0)
{
}

DecisionModel::~DecisionModel()
{
	if(_initial)
	{
		delete _initial;
	}
}

void DecisionModel::reset()
{
	if(_initial)
	{
		delete _initial;
		_initial = 0;
	}
	_initial = new ForagerState(_agent.getPosition(), _agent.getNeededResources(), _agent.getWorld()->getDynamicRasterStr(_agent.getKnowledgeMap()), _agent.getWorld()->getDynamicRasterStr(_agent.getResourcesMap()), 0);
	makeActionsForState(*_initial);
}

action_t DecisionModel::number_actions( const ForagerState & state ) const
{
	return state.getNumAvailableActions();
}

const ForagerState & DecisionModel::init() const
{
	return *_initial;
}

bool DecisionModel::terminal( const ForagerState & state ) const
{
	if(state.getTimeStep()>_agent.getHorizon())
	{
		std::cout << "state: " << state << " is terminal due to exceed horizon: " << _agent.getHorizon() << std::endl;
		return true;
	}
	return false;
}
	
bool DecisionModel::dead_end( const ForagerState & state ) const
{
	return false;
}

bool DecisionModel::applicable( const ForagerState & state, action_t action) const
{
	return true;
}

float DecisionModel::cost( const ForagerState & state, action_t action ) const
{
	float cost = 0.0f;
	float foragedResources = std::min(state.getNeededResources(), state.getResourcesMap().getValue(state.getAvailableAction(action).getNewPosition()));

	float neededResources = _agent.getNeededResources();
	if(foragedResources<neededResources)
	{
		cost += (neededResources-foragedResources)/neededResources;
	}
	std::cout << "action: " << action << " from state: " << state << " moving to: " << state.getAvailableAction(action).getNewPosition() << " is getting: " << foragedResources << " resources needed: " << neededResources << " base cost: " << cost << std::endl;
	return cost;
}

void DecisionModel::next( const ForagerState & state, action_t index, OutcomeVector & outcomes ) const
{
//	std::cout << "creating next state" << std::endl;
	ForagerState stateNext(state);
	const MoveAction & moveAction = state.getAvailableAction(index);
	moveAction.executeMDP( _agent, state, stateNext );
	applyFrameEffects(state, stateNext);
	stateNext.computeHash();	
	makeActionsForState(stateNext);
	outcomes.push_back(std::make_pair(stateNext, 1.0));
//	std::cout << "end creating next state" << std::endl;
}

void DecisionModel::applyFrameEffects( const ForagerState & state,  ForagerState & stateNext ) const
{
	stateNext.consume();
	stateNext.increaseTimeStep();
}

void DecisionModel::makeActionsForState( ForagerState & state ) const
{
	state.clearActions();
	assert(state.getNumAvailableActions()==0);
	for(int i=state.getPosition()._x-1; i<=state.getPosition()._x+1; i++)
	{
		for(int j=state.getPosition()._y-1; j<=state.getPosition()._y+1; j++)
		{
			Engine::Point2D<int> newPosition(i,j);
			if(!_agent.getWorld()->checkPosition(newPosition))
			{
				continue;
			}
			//std::cout << "from position: " << state.getPosition() << " creating new at: " << newPosition << std::endl;
			MoveAction * newAction = new MoveAction(newPosition);
			//std::cout << "creating new action: " << newAction << std::endl;
			state.addAction(newAction);
		}
	}
	state.randomizeActions();
	assert(state.getNumAvailableActions()>0);
}

} // namespace QuantumExperiment

